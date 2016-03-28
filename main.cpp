#include "argument_parser.hpp"
#include "banking_arguments.hpp"
#include "csv.hpp"
#include "templates.hpp"
#include "readline.hpp"
#include <string>
#include <time.h>
#include <unordered_map>
#include <iostream>
#include <ostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <regex>

/*
 * A general note about how data flows through the code. There is a series of 
 * transformations going on:
 * 
 * 1. Read a line from the CSV file into a string.
 * 2. Convert the string to a string array of type "csv_line" (see csv.hpp)
 * 3. Convert "csv_line" to "banking_item", converting strings to numbers 
 *    and dates and such (see "parse_item" in this file)
 * 4. Process a list of these "banking_items", interactively convert to a
 *    ledger string
 */
namespace {
/* 
 * Parse SWIFT format. Currently uses a very weak regular expression to cut
 * out the "SVWZ+" ("Verwendungszweck") and return it.
 */
std::string
parse_swift_string(std::string const &s) {
  std::regex regex{"SVWZ\\+(.*)?(IBAN|BIC|EREF|DEBT|SVWZ|ABWA|ABWE|$)"};
  std::match_results<std::string::const_iterator> results;
  if(!std::regex_search(s.cbegin(),s.cend(),results,regex))
    return s;
  return results[1];
}

/*
 * Item cleaned and converted from CSV
 */
struct banking_item {
  std::string const date;
  std::string const summary;
  std::string const purpose;
  std::string const payer;
  std::string const amount;
  std::string const currency;
};

/*
 * Parse date in "input" using "format", transform to ledger date format
 */
std::string
transform_date(std::string const &input,std::string const &format) {
  struct tm result_time;
  char const *input_c = input.c_str();
  char const *rest_char = strptime(input_c,format.c_str(),&result_time);
  if(rest_char == NULL || rest_char != input_c+input.length())
    throw std::runtime_error("couldn't parse date “"+input+"”");
  std::size_t const max_size = 64;
  char output[max_size];
  strftime(output,max_size,"%Y/%m/%d",&result_time);
  return std::string{output};
}

/*
 * Use the command line parameters and a csv line to produce a clean
 * "banking_item"
 */
banking_item parse_item(mt940::csv_line const &l,mt940::banking_arguments const &ba) {
  using namespace mt940;
  return
    banking_item{
      transform_date(
	find_exceptionally_csv(l,ba.column_date),
	ba.date_format),
      ba.column_summary < 0 ? "" : find_exceptionally_csv(l,ba.column_summary),
      ba.column_purpose < 0 ? "" : parse_swift_string(find_exceptionally_csv(l,ba.column_purpose)),
      ba.column_payer < 0 ? "" : find_exceptionally_csv(l,ba.column_payer),
      find_exceptionally_csv(l,ba.column_amount),
      find_exceptionally_csv(l,ba.column_currency)};
}

enum class process_item_result {
  discontinue,
  proceed
};

/*
 * The main loop. Read one item, ask questions, write out, skip or break
 */
process_item_result
process_item(
  banking_item const &bi,
  std::string const &template_file,
  std::ofstream &output_stream) {
  mt940::replacement_map rep_map{
			  {"date",bi.date},
			  {"summary",bi.summary},
			  {"payer",bi.payer},
			  {"amount",bi.amount},
			  {"currency",bi.currency},
			  {"purpose",bi.purpose}};
  std::cout << mt940::replace_template_file(template_file,rep_map) << "\n";
  std::cout << "\nenter “s” to skip the entry, ctrl+d to exit\n";
  std::string const skip_constant{"s"};
  mt940::readline_result const purpose(mt940::readline("purpose ["+bi.purpose+"] "));
  if(purpose.quit())
    return process_item_result::discontinue;
  if(!purpose.empty()) {
    if(purpose.result() == skip_constant)
      return process_item_result::proceed;
    rep_map["purpose"] = purpose.result();
  }
  rep_map["account"] = "";
  do {
    mt940::readline_result account(mt940::readline("account? "));
    if(account.quit())
      return process_item_result::discontinue;
    if(account.result() == skip_constant)
      return process_item_result::proceed;
    rep_map["account"] = account.result();
  } while(rep_map["account"].empty());
  output_stream << mt940::replace_template_file(template_file,rep_map) << "\n";
  return process_item_result::proceed;
}

/*
 * The main function: use arguments to determine input and output files,
 * parse CSV file, process each item.
 */
void
process_file(mt940::banking_arguments const &ba) {
      std::ifstream input_file{ba.input_file};
      std::ofstream output_file(ba.output_file,std::ios_base::app);

      if(!input_file)
	throw std::runtime_error("couldn't open file “"+ba.input_file+"” for reading");
	
      if(!output_file)
	throw std::runtime_error("couldn't open file “"+ba.input_file+"” for writing");

      std::string line;

      unsigned line_counter = 0l;
      
      if(ba.skip_header) {
	std::getline(input_file,line);
	line_counter++;
      }
      
      while(std::getline(input_file,line)) {
	try {
	  process_item_result process_result =
	    process_item(
	      parse_item(mt940::parse_csv_line(line,ba.separator,'"'),ba),
	      ba.template_file,
	      output_file);
	  line_counter++;
	  switch(process_result) {
	  case process_item_result::discontinue:
	    return;
	  case process_item_result::proceed:
	    break;
	  }
	} catch (std::runtime_error const &e) {
	  throw std::runtime_error("line "+std::to_string(line_counter)+": "+e.what());
	}
      }
}
}

int main(int argc,char *argv[]) try {
  process_file(mt940::parse_banking_arguments(argv,argc));
} catch (std::runtime_error const &e) {
  std::cerr << e.what() << "\n";
  return -1;
}
