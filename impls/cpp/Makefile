CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra -Wno-unused-parameter
EXE = step9_try

build: step9_try

clean:
	rm -f $(EXE)

step9_try: src/step9_try.cpp src/rep.cpp src/Environment.cpp src/Lexer.cpp src/Reader.cpp src/types.cpp src/Error.cpp src/core_ns.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
