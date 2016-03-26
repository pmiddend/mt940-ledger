#ifndef MT940_TEMPLATES_HPP_INCLUDED
#define MT940_TEMPLATES_HPP_INCLUDED

#include <unordered_map>
#include <string>

namespace mt940 {
typedef
std::unordered_map<std::string,std::string>
replacement_map;

std::string
read_file_to_string(std::string const &filename);

std::string replace_all_substrings(
				   std::string subject,
				   std::string const &search,
				   std::string const &replace);
  
std::string
replace_template_file(
		      std::string const &filename,
		      replacement_map const &replacements);
}

#endif
