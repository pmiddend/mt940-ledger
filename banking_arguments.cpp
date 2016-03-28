#include "banking_arguments.hpp"
#include "argument_parser.hpp"
#include <stdexcept>

namespace {
char parse_char_arg(
  std::string const &arg) {
  if(arg.length() != 1)
    throw std::runtime_error("invalid char argument “"+arg+"”");
  return arg[0];
}

int parse_signed_arg(
  std::string const &arg) {
  try {
    int const r{std::stoi(arg)};
    return r;
  } catch (std::invalid_argument const &e) {
      throw std::runtime_error("invalid unsigned argument “"+arg+"” (invalid argument)");
  } catch (std::out_of_range const &e) {
      throw std::runtime_error("invalid unsigned argument “"+arg+"” (out of range)");
  }
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
}

mt940::banking_arguments mt940::parse_banking_arguments(char *argv[],int const argc) {
  arguments const parsed_args(parse_arguments(argv,argc));
  if(parsed_args.free_arguments.size() != 2)
    throw std::runtime_error("found "+std::to_string(parsed_args.free_arguments.size())+" free arguments, expected 2 (input and output)");
  return
    banking_arguments{
      parsed_args.free_arguments[0],
      parsed_args.free_arguments[1],
      find_exceptionally(parsed_args.argument_map,"template-file"),
      parse_char_arg(find_exceptionally(parsed_args.argument_map,"separator")),
      parse_bool_arg(find_exceptionally(parsed_args.argument_map,"skip-header")),
      find_exceptionally(parsed_args.argument_map,"date-format"),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-date")),
      parse_signed_arg(find_exceptionally(parsed_args.argument_map,"column-summary")),
      parse_signed_arg(find_exceptionally(parsed_args.argument_map,"column-purpose")),
      parse_signed_arg(find_exceptionally(parsed_args.argument_map,"column-payer")),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-amount")),
      parse_unsigned_arg(find_exceptionally(parsed_args.argument_map,"column-currency"))};
}
