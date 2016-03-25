#ifndef MT940_BANKING_ARGUMENTS_HPP_INCLUDED
#define MT940_BANKING_ARGUMENTS_HPP_INCLUDED

#include <string>

namespace mt940 {
struct banking_arguments {
  std::string const input_file;
  std::string const output_file;
  std::string const template_file;
  char const separator;
  bool const skip_header;
  std::string const date_format;
  unsigned const column_date;
  unsigned const column_summary;
  unsigned const column_purpose;
  unsigned const column_payer;
  unsigned const column_amount;
  unsigned const column_currency;
};

banking_arguments
parse_banking_arguments(char *argv[],int const argc);
}

#endif
