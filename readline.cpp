#include "readline.hpp"
#include <readline/readline.h>
#include <cstdlib>

mt940::readline_result::readline_result(std::unique_ptr<std::string> impl)
: impl(std::move(impl))
{
}

bool
mt940::readline_result::quit() const {
  return impl == nullptr;
}

bool
mt940::readline_result::empty() const {
  return impl->empty();
}

std::string
mt940::readline_result::result() const {
  return *impl;
}

mt940::readline_result mt940::readline(std::string const &prompt) {
  char *result = ::readline(prompt.c_str());
  if(result == 0)
    return readline_result(std::unique_ptr<std::string>());
  std::string result_string(result);
  std::free(result);
  return readline_result(std::unique_ptr<std::string>(new std::string(result_string)));
}
