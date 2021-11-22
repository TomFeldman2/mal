//
// Created by tomfe on 21/11/2021.
//

#include <iostream>
#include "../linenoise.hpp"

const std::string &READ(const std::string &input) {
    return input;
}

const std::string &EVAL(const std::string &input) {
    return input;
}

const std::string &PRINT(const std::string &input) {
    return input;
}

const std::string &rep(const std::string &input) {
    const auto &ast = READ(input);
    const auto &result = EVAL(ast);
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

        std::cout << rep(input) << std::endl;
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);
}
