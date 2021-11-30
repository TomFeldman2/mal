CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra -Wno-unused-parameter

build: step5_tco

clean:
	rm -f step0_repl step1_read_print step2_eval step3_env step4_if_fn_do

#step1_read_print: step1_read_print.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step2_eval: step2_eval.cpp rep.cpp environment.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step3_env: step3_env.cpp rep.cpp Environment.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step4_if_fn_do: step4_if_fn_do.cpp rep.cpp Environment.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

step5_tco: step5_tco.cpp rep.cpp Environment.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
