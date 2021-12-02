CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra -Wno-unused-parameter
EXE = step8_macros

build: step8_macros

clean:
	rm -f $(EXE)

step8_macros: src/step8_macros.cpp src/rep.cpp src/Environment.cpp src/Lexer.cpp src/Reader.cpp src/types.cpp src/Error.cpp src/core_ns.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
