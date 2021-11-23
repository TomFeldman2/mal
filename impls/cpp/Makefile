CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra -Werror

#step1_read_print: step1_read_print.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

step2_eval: step2_eval.cpp rep.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
