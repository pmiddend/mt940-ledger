#include <string>
#include <time.h>
#include <unordered_map>
#include <iostream>
#include <ostream>
#include <vector>
#include <stdexcept>
#include <fstream>

namespace {
struct banking_arguments {
  std::string const input_file;
  std::string const output_file;
  char const separator;
  bool const skip_header;
  std::string const date_format;
  unsigned const column_date;
  unsigned const column_summary;
  unsigned const column_purpose;
  unsigned const column_payer;
  unsigned const column_amount;
  unsigned const column_currency;
};

struct arguments {
  typedef
  std::unordered_map<std::string,std::string>
  argument_map_type;

  typedef
  std::vector<std::string>
  free_argument_type;

  argument_map_type const argument_map;
  free_argument_type const free_arguments;
};

arguments parse_arguments(char *argv[], int const argc) {
  arguments::argument_map_type argument_map;
  arguments::free_argument_type free_arguments;
  for(int i = 1; i < argc; ++i) {
    std::string const strargv{argv[i]};
    if(strargv.empty())
      continue;
    if(strargv[0] == '-') {
      if(strargv.length() == 1 || strargv[1] != '-')
	throw std::runtime_error("free arguments cannot begin with -, did you mean “-"+strargv+"”?");
      std::string::size_type const equals_pos{strargv.find('=')};
      if(equals_pos == std::string::npos)
	throw std::runtime_error("argument without value found, did you mean “"+strargv+"=...”?");
      std::string const argument_name = strargv.substr(2,equals_pos-2);
      if(argument_map.find(argument_name) != argument_map.end())
      	throw std::runtime_error("duplicate argument “"+strargv+"” found");
      argument_map[argument_name] = strargv.substr(equals_pos+1);
    } else {
      free_arguments.push_back(strargv);
    }
  }
  return arguments{argument_map,free_arguments};
}

std::string
argument_map_to_string(arguments::argument_map_type const &argmap) {
  std::string result;
  for(arguments::argument_map_type::value_type const &r : argmap)
    result += "\""+r.first+"\" => \""+r.second+"\" ";
  return result;
}

std::string
find_exceptionally(
  arguments::argument_map_type const &argmap,
  std::string const &argname) {
  arguments::argument_map_type::const_iterator const result = argmap.find(argname);
  if(result == argmap.end())
    throw std::runtime_error("cannot find argument “"+argname+"”, parsed "+argument_map_to_string(argmap));
  return result->second;
}

char parse_char_arg(
  std::string const &arg) {
  if(arg.length() != 1)
    throw std::runtime_error("invalid char argument “"+arg+"”");
  return arg[0];
}

unsigned parse_unsigned_arg(
  std::string const &arg) {
  try {
    int const r{std::stoi(arg)};
    if(r < 0)
      throw std::runtime_error("invalid unsigned argument “"+arg+"” (not non-negative)");
    return r;
  } catch (std::invalid_argument const &e) {
      throw std::runtime_error("invalid unsigned argument “"+arg+"” (invalid argument)");
  } catch (std::out_of_range const &e) {
      throw std::runtime_error("invalid unsigned argument “"+arg+"” (out of range)");
  }
}

bool parse_bool_arg(
  std::string const &arg) {
  if(arg == "1" || arg == "true" || arg == "yes" || arg == "y")
    return true;
  if(arg == "0" || arg == "false" || arg == "no" || arg == "n")
    return false;
  throw std::runtime_error("invalid bool argument “"+arg+"”");
}

banking_arguments const parse_banking_arguments(char *argv[],int const argc) {
  arguments const parsed_args{parse_arguments(argv,argc)};
  if(parsed_args.free_arguments.size() != 2)
    throw std::runtime_error("found "+std::to_string(parsed_args.free_arguments.size())+" free arguments, expected 2 (input and output)");
  return
    banking_arguments{
      parsed_args.free_arguments[0],
      parsed_args.free_arguments[1],
      parse_char_arg(find_exceptionally(parsed_args.argument_map,"separator")),
      parse_bool_arg(find_exceptionally(parsed_args.argument_map,"skip-header")),
      find_exceptionally(parsed_args.argument_map,"date-format"),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-date")),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-summary")),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-purpose")),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-payer")),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-amount")),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-currency"))};
}

typedef
std::vector<std::string>
csv_line;

struct csv_field_result {
  std::string const field;
  std::string const rest;
};

csv_field_result
parse_csv_field(std::string const &s,char const separator,char const quote) {
    if(s.empty())
      return csv_field_result{"",""};
    if(s[0] != quote) {
      std::string::size_type const commapos = s.find(separator);
      if(commapos == std::string::npos)
	return csv_field_result{s,""};
      return csv_field_result{s.substr(0,commapos),s.substr(commapos+1)};
    }
    std::string field_string;
    for(std::string::size_type i{1}; i < s.length(); ++i) {
      if(s[i] == quote) {
	if(i+1 == s.length())
	  return csv_field_result{field_string,""};
	if(s[i+1] != quote) {
	  if(s[i+1] == separator)
	    return csv_field_result{field_string,s.substr(i+2)};
	  throw std::runtime_error(std::string{"expected ‘"}+separator+"’ after quote, got ‘"+s[i+1]+"’");
	}
	++i;
      }
      field_string += s[i];
    }
    throw std::runtime_error("quote not terminated");
}

csv_line parse_csv_line(std::string const &s,char const separator,char const quote) {
  csv_line result;
  std::string rest{s};
  do {
    auto field_result = parse_csv_field(rest,separator,quote);
    result.push_back(field_result.field);
    rest = field_result.rest;
  } while(!rest.empty());
  return result;
}

struct banking_item {
  std::string const date;
  std::string const summary;
  std::string const purpose;
  std::string const payer;
  std::string const amount;
  std::string const currency;
};

std::string
find_exceptionally_csv(csv_line const &l,unsigned const column) {
  if(l.size() <= column)
    throw std::runtime_error("tried to find column "+std::to_string(column)+", but have only "+std::to_string(l.size())+" columns");
  return l[column];
}

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
  csv_line const &l,
  banking_arguments const &ba) {
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
  std::ofstream &) {
  std::cout << "summary is: " << bi.summary << "\n";
  std::cout << "purpose is: " << bi.purpose << "\n";
  std::cout << "payer is: " << bi.payer << "\n";
  std::cout << "amount is: " << bi.amount << " " << bi.currency << "\n";
}

void
process_file(banking_arguments const &ba) {
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
	  process_item(parse_item(parse_csv_line(line,ba.separator,'"'),ba),output_file);
	  line_counter++;
	} catch (std::runtime_error const &e) {
	  throw std::runtime_error("line "+std::to_string(line_counter)+": "+e.what());
	}
      }
}
}

int main(int argc,char *argv[]) try {
  process_file(parse_banking_arguments(argv,argc));
} catch (std::runtime_error const &e) {
  std::cerr << e.what() << "\n";
  return -1;
}
