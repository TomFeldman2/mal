CXX = g++
CXXFLAGS = -std=c++17 -g -Wall -Wextra -Wno-unused-parameter
EXE = step0_repl step1_read_print step2_eval step3_env step4_if_fn_do step5_tco step6_file step7_quote

build: step7_quote

clean:
	rm -f $(EXE)


#step0_repl: src/step0_repl.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step1_read_print: step1_read_print.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step2_eval: step2_eval.cpp rep.cpp environment.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step3_env: step3_env.cpp rep.cpp Environment.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step4_if_fn_do: step4_if_fn_do.cpp rep.cpp Environment.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step5_tco: step5_tco.cpp rep.cpp Environment.cpp Lexer.cpp Reader.cpp types.cpp Error.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

#step6_file: src/step6_file.cpp src/rep.cpp src/Environment.cpp src/Lexer.cpp src/Reader.cpp src/types.cpp src/Error.cpp src/core_ns.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $^

step7_quote: src/step7_quote.cpp src/rep.cpp src/Environment.cpp src/Lexer.cpp src/Reader.cpp src/types.cpp src/Error.cpp src/core_ns.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
