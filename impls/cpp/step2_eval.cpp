//
// Created by tomfe on 21/11/2021.
//

#include <iostream>
#include "linenoise.hpp"
#include "rep.h"
#include "Reader.h"
#include "Error.h"



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
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        } catch (const Reader::Comment &) {}
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);
}
