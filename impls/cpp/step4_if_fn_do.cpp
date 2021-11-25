//
// Created by tomfe on 21/11/2021.
//

#include <iostream>
#include <cassert>
#include "linenoise.hpp"
#include "rep.h"
#include "Reader.h"
#include "Error.h"



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

    auto prn_func = std::make_shared<MalFunc>(
            [](auto list) {
                std::cout << list->joinElements(false) << std::endl;
                return MalNil::getInstance();
            });

    auto println_func = std::make_shared<MalFunc>(
            [](auto list) {
                std::cout << list->joinElements(false, false) << std::endl;
                return MalNil::getInstance();
            });

    auto prn_str_func = std::make_shared<MalFunc>(
            [](auto list) {
                return std::make_shared<MalString>(list->joinElements(false));
            });

    auto str_func = std::make_shared<MalFunc>(
            [](auto list) {
                return std::make_shared<MalString>(list->joinElements(false, false, ""));
            });


    auto list_func = std::make_shared<MalFunc>(
            [](auto list) {
                auto new_list = std::make_shared<MalList>(list->is_list);
                std::copy(std::next(list->begin()), list->end(), std::back_inserter(*new_list));
                return new_list;
            });

    auto is_list_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 2);
                return MalBool::getInstance(list->at(1)->getType() == MalObject::Type::LIST);
            });

    auto is_empty_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 2);
                return MalBool::getInstance(std::dynamic_pointer_cast<MalList>(list->at(1))->empty());
            });

    auto count_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 2);
                if (list->at(1)->getType() == MalObject::Type::NIL) return std::make_shared<MalInt>(0);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalList>(list->at(1))->size());
            });

    auto eq_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return MalBool::getInstance(*list->at(1) == *list->at(2));
            });

    auto gt_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(list->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(list->at(2));
                return MalBool::getInstance(first->value > second->value);
            });

    auto gte_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(list->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(list->at(2));
                return MalBool::getInstance(first->value >= second->value);
            });

    auto lt_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(list->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(list->at(2));
                return MalBool::getInstance(first->value < second->value);
            });

    auto lte_func = std::make_shared<MalFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(list->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(list->at(2));
                return MalBool::getInstance(first->value <= second->value);
            });





    auto env = std::make_shared<Environment>();
    env->insert({"+", plus_func});
    env->insert({"-", minus_func});
    env->insert({"*", mul_func});
    env->insert({"/", div_func});
    env->insert({"prn", prn_func});
    env->insert({"pr-str", prn_str_func});
    env->insert({"println", println_func});
    env->insert({"str", str_func});
    env->insert({"list", list_func});
    env->insert({"list?", is_list_func});
    env->insert({"empty?", is_empty_func});
    env->insert({"count", count_func});
    env->insert({"=", eq_func});
    env->insert({">", gt_func});
    env->insert({">=", gte_func});
    env->insert({"<", lt_func});
    env->insert({"<=", lte_func});

    rep("(def! not (fn* (a) (if a false true)))", env);

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
