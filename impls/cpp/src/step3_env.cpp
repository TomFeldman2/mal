//
// Created by tomfe on 21/11/2021.
//

#include <iostream>
#include <cassert>
#include "../include/linenoise.hpp"
#include "../include/rep.h"
#include "../include/Reader.h"
#include "../include/Error.h"



int main() {
    const auto history_path = "history.txt";
    linenoise::LoadHistory(history_path);

    auto plus_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                + std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
            });

    auto minus_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                - std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
            });

    auto mul_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                * std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
            });

    auto div_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                / std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
            });

    Environment env{};
    env.insert({"+", plus_func});
    env.insert({"-", minus_func});
    env.insert({"*", mul_func});
    env.insert({"/", div_func});


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
        } catch (const Reader::Comment &) {}
        linenoise::AddHistory(input.c_str());
    }

    linenoise::SaveHistory(history_path);
}
