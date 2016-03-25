#ifndef MT940_ARGUMENT_PARSER_HPP_INCLUDED
#define MT940_ARGUMENT_PARSER_HPP_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>

namespace mt940 {
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

arguments parse_arguments(char *argv[], int const argc);

std::string
argument_map_to_string(arguments::argument_map_type const &);

std::string
find_exceptionally(
  arguments::argument_map_type const &argmap,
  std::string const &);
}

#endif
