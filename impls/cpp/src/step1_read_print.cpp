//
// Created by tomfe on 21/11/2021.
//

#include <iostream>
#include "../include/linenoise.hpp"
#include "../include/types.h"
#include "../include/Reader.h"
#include "../include/Error.h"

MalType *READ(const std::string &input) {
    return readStr(input);
}

MalType *EVAL(MalType *ast) {
    return ast;
}

std::string PRINT(MalType *ast) {
    auto str = ast->toString();
    delete ast;
    return str;
}

std::string rep(const std::string &input) {
    auto ast = READ(input);
    auto result = EVAL(ast);
    return PRINT(result);
}

int main() {
    const auto history_path = "history.txt";

    linenoise::LoadHistory(history_path);
    std::string input;
    while (true) {
        auto quit = linenoise::Readline("user> ", input);

        if (quit) {
            break;
        }

        try {
            std::cout << rep(input) << std::endl;
        } catch (const Error &e) {
            std::cout << e.what() << std::endl;
        } catch (const Reader::Comment &) {}
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);
}
