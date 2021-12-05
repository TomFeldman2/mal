//
// Created by tomfe on 21/11/2021.
//

#include <iostream>
#include <cassert>
#include "../include/linenoise.hpp"
#include "../include/rep.h"
#include "../include/Reader.h"
#include "../include/Error.h"
#include "../include/core_ns.h"

using std::string_literals::operator""s;

int main(int argc, char *argv[]) {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);


    auto env = getCoreEnv();

    rep("(def! not (fn* (a) (if a false true)))", env);
    rep("(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))", env);
    rep("(defmacro! cond (fn* (& xs) (if (> (count xs) 0) (list 'if (first xs) (if (> (count xs) 1) (nth xs 1) (throw \"odd number of forms to cond\")) (cons 'cond (rest (rest xs)))))))" , env);

    if (argc >= 2) {
        rep("load-file \""s + argv[1] + "\"", env);
    }

    auto mal_argv = std::make_shared<MalList>();
    for (int i = 2; i < argc; ++i) {
        mal_argv->push_back(std::make_shared<MalString>(std::string(argv[i])));
    }

    env->insert({"*ARGV*", mal_argv});

    std::string input;
    while (true) {
        auto quit = linenoise::Readline("user> ", input);

        if (quit) {
            break;
        }

        try {
            std::cout << rep(input, env) << std::endl;
        } catch (const Error &e) {
            std::cout << e.what() << std::endl;
        } catch (const MalException& e) {
            std::cout << e.toString(false) << std::endl;
        }
        catch (const Reader::Comment &) {}
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);
    return 0;
}
