#include "templates.hpp"
#include <stdexcept>
#include <fstream>
#include <streambuf>

std::string
mt940::read_file_to_string(std::string const &filename) {
  std::ifstream t(filename);
  if(!t)
    throw std::runtime_error("cannot open “"+filename+"”");
  return std::string(std::istreambuf_iterator<char>(t),std::istreambuf_iterator<char>());
}

std::string mt940::replace_all_substrings(
				   std::string subject,
				   std::string const &search,
				   std::string const &replace) {
  size_t pos{0};
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}
  
std::string
mt940::replace_template_file(
		      std::string const &filename,
		      replacement_map const &replacements) {
  std::string result{read_file_to_string(filename)};
  for(auto replacement : replacements) {
    result = replace_all_substrings(result,"${"+replacement.first+"}",replacement.second);
  }
  return result;
}
