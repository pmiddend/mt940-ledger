#ifndef MT940_ARGUMENT_PARSER_HPP_INCLUDED
#define MT940_ARGUMENT_PARSER_HPP_INCLUDED

#include <string>
#include <vector>
#include <unordered_map>

/*
 * Parse command line arguments in a general way (not mt940-ledger specific).
 * 
 * The format is very strict, it doesn't allow short arguments like "-f foo"
 * or "-c120" or to omit the equals sign like "--foo bar". This is 
 * deliberately so. I find most command line programs very confusing.
 *
 * There are also variably many "free arguments", meaning those without a
 * "--foo=" in front of them.
 */
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

// This is just for debug output
std::string
argument_map_to_string(arguments::argument_map_type const &);

// Find argument in map or throw an exception
std::string
find_exceptionally(
  arguments::argument_map_type const &argmap,
  std::string const &);
}

#endif
