#include "argument_parser.hpp"
#include <stdexcept>

std::string
mt940::argument_map_to_string(arguments::argument_map_type const &argmap) {
  std::string result;
  for(arguments::argument_map_type::value_type const &r : argmap)
    result += "\""+r.first+"\" => \""+r.second+"\" ";
  return result;
}

mt940::arguments mt940::parse_arguments(char *argv[], int const argc) {
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
mt940::find_exceptionally(
  arguments::argument_map_type const &argmap,
  std::string const &argname) {
  arguments::argument_map_type::const_iterator const result = argmap.find(argname);
  if(result == argmap.end())
    throw std::runtime_error("cannot find argument “"+argname+"”, parsed "+argument_map_to_string(argmap));
  return result->second;
}
