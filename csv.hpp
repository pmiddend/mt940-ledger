#ifndef MT940_CSV_HPP_INCLUDED
#define MT940_CSV_HPP_INCLUDED

#include <vector>
#include <string>

namespace mt940 {
typedef
std::vector<std::string>
csv_line;

csv_line
parse_csv_line(
	       std::string const &s,
	       char const separator,
	       char const quote);

std::string
find_exceptionally_csv(csv_line const &l,unsigned const column);
}

#endif
