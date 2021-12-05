CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra -Wno-unused-parameter
EXE = stepA_mal

build: stepA_mal

clean:
	rm -f $(EXE)

stepA_mal: src/stepA_mal.cpp src/rep.cpp src/Environment.cpp src/Lexer.cpp src/Reader.cpp src/types.cpp src/Error.cpp src/core_ns.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
