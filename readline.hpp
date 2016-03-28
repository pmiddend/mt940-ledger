#ifndef MT940_READLINE_HPP_INCLUDED
#define MT940_READLINE_HPP_INCLUDED

#include <string>
#include <memory>

/*
 * Simple, memory-safe wrapper around ::readline
 */
namespace mt940 {
class readline_result {
std::unique_ptr<std::string> impl;
public:
  explicit
  readline_result(std::unique_ptr<std::string> prompt);

  // Has the user entered C-d?
  bool quit() const;

  // Is the resulting string empty? (undefined if quit == true)
  bool empty() const;

  // The resulting string (undefined if quit == true)
  std::string result() const;
};

readline_result readline(std::string const &);
}

#endif
