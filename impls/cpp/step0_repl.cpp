//
// Created by tomfe on 21/11/2021.
//

#include <iostream>
#include "step0_repl.h"

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
    std::string input;
    while (true) {
        std::cout << "user> ";
        if (not std::getline(std::cin, input)) break;

        std::cout << rep(input) << std::endl;
    }
}
