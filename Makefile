CXX = /usr/bin/g++
SOURCEFILES = main.cpp argument_parser.cpp banking_arguments.cpp csv.cpp
HEADERS = argument_parser.hpp banking_arguments.hpp csv.hpp

mt940-ledger: $(SOURCEFILES) $(HEADERFILES)
	$(CXX) -Wall -Wextra -std=c++11 -o mt940-ledger $(SOURCEFILES)
