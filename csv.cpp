#include "csv.hpp"
#include <stdexcept>

namespace {
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
}

mt940::csv_line mt940::parse_csv_line(
				      std::string const &s,
				      char const separator,
				      char const quote) {
  csv_line result;
  std::string rest{s};
  do {
    auto field_result = parse_csv_field(rest,separator,quote);
    result.push_back(field_result.field);
    rest = field_result.rest;
  } while(!rest.empty());
  return result;
}

std::string
mt940::find_exceptionally_csv(csv_line const &l,unsigned const column) {
  if(l.size() <= column)
    throw std::runtime_error("tried to find column "+std::to_string(column)+", but have only "+std::to_string(l.size())+" columns");
  return l[column];
}
