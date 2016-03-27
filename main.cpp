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

namespace {
struct banking_item {
  std::string const date;
  std::string const summary;
  std::string const purpose;
  std::string const payer;
  std::string const amount;
  std::string const currency;
};

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

banking_item parse_item(
			mt940::csv_line const &l,
  mt940::banking_arguments const &ba) {
  using namespace mt940;
  return
    banking_item{
      transform_date(
	find_exceptionally_csv(l,ba.column_date),
	ba.date_format),
      find_exceptionally_csv(l,ba.column_summary),
      find_exceptionally_csv(l,ba.column_purpose),
      find_exceptionally_csv(l,ba.column_payer),
      find_exceptionally_csv(l,ba.column_amount),
      find_exceptionally_csv(l,ba.column_currency)};
}


void process_item(
  banking_item const &bi,
  std::string const &template_file,
  std::ofstream &) {
  std::cout << mt940::replace_template_file(
			template_file,
			mt940::replacement_map{
			  {"date",bi.date},
			    {"summary",bi.summary},
			      {"payer",bi.payer},
				{"amount",bi.amount},
				  {"currency",bi.currency},
			  {"purpose",bi.purpose}}) << "\n";
  std::string const purpose(mt940::readline("purpose ["+bi.purpose+"] "));
  std::string const account(mt940::readline("account? "));
  /*
  std::cout << bi.date << " $purpose\n";
  std::cout << "    ; summary: " << bi.summary << "\n";
  std::cout << "    ; purpose: " << bi.purpose << "\n";
  std::cout << "    ; payer: " << bi.payer << "\n";
  std::cout << "    $account        " << bi.amount << " " << bi.currency << "\n";
  std::cout << "which account?\n";
  std::string account;
  std::cin >> account;
  std::cout << "which purpose?\n";
  std::string purpose;
  std::cin >> purpose;*/
}

void
process_file(mt940::banking_arguments const &ba) {
      std::ifstream input_file{ba.input_file};
      std::ofstream output_file{ba.output_file};

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
	  process_item(parse_item(mt940::parse_csv_line(line,ba.separator,'"'),ba),ba.template_file,output_file);
	  line_counter++;
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
