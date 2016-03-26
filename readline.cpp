#include "readline.hpp"
#include <readline/readline.h>
#include <cstdlib>

std::string mt940::readline(std::string const &prompt) {
  char *result = ::readline(prompt.c_str());
  std::string result_string(result);
  std::free(result);
  return result_string;
}
