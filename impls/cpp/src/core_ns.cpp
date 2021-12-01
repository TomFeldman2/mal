//
// Created by tomfe on 30/11/2021.
//

#include <cassert>
#include <iostream>
#include <fstream>
#include "../include/core_ns.h"
#include "../include/rep.h"
#include "../include/Reader.h"

std::shared_ptr<Environment> getCoreEnv() {
    auto env = std::make_shared<Environment>();

    auto plus_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                + std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
            });

    auto minus_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                - std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
            });

    auto mul_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                * std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
            });

    auto div_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                / std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
            });

    auto prn_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                std::cout << list->joinElements(false) << std::endl;
                return MalNil::getInstance();
            });

    auto println_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                std::cout << list->joinElements(false, false) << std::endl;
                return MalNil::getInstance();
            });

    auto prn_str_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                return std::make_shared<MalString>(list->joinElements(false));
            });

    auto str_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                return std::make_shared<MalString>(list->joinElements(false, false, ""));
            });

    auto read_string_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 2);
                auto str = std::dynamic_pointer_cast<MalString>(list->at(1));
                assert(str);
                return readStr(str->value);
            });

    auto slurp_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 2);
                auto str = std::dynamic_pointer_cast<MalString>(list->at(1));
                assert(str);
                std::ifstream file(str->value);
                std::string file_str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                return std::make_shared<MalString>(file_str);
            });


    auto list_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                auto new_list = std::make_shared<MalList>(list->is_list);
                std::copy(std::next(list->begin()), list->end(), std::back_inserter(*new_list));
                return new_list;
            });

    auto is_list_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 2);
                return MalBool::getInstance(list->at(1)->getType() == MalObject::Type::LIST);
            });

    auto is_empty_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 2);
                return MalBool::getInstance(std::dynamic_pointer_cast<MalList>(list->at(1))->empty());
            });

    auto count_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 2);
                if (list->at(1)->getType() == MalObject::Type::NIL) return std::make_shared<MalInt>(0);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalList>(list->at(1))->size());
            });

    auto eq_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                return MalBool::getInstance(*list->at(1) == *list->at(2));
            });

    auto gt_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(list->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(list->at(2));
                return MalBool::getInstance(first->value > second->value);
            });

    auto gte_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(list->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(list->at(2));
                return MalBool::getInstance(first->value >= second->value);
            });

    auto lt_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(list->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(list->at(2));
                return MalBool::getInstance(first->value < second->value);
            });

    auto lte_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(list->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(list->at(2));
                return MalBool::getInstance(first->value <= second->value);
            });

    auto eval_func = std::make_shared<MalCoreFunc>(
            [&env](auto list) {
                assert(list->size() == 2);
                return EVAL(list->at(1), env);
            });

    auto atom_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 2);
                return std::make_shared<MalAtom>(list->at(1));
            });

    auto is_atom_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 2);
                return MalBool::getInstance(list->at(1)->getType() == MalObject::Type::ATOM);
            });

    auto deref_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 2);
                auto atom = std::dynamic_pointer_cast<MalAtom>(list->at(1));
                assert(atom);
                return atom->object;
            });

    auto reset_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() == 3);
                auto atom = std::dynamic_pointer_cast<MalAtom>(list->at(1));
                assert(atom);
                atom->object = list->at(2);
                return atom->object;
            });

    auto swap_func = std::make_shared<MalCoreFunc>(
            [](auto list) {
                assert(list->size() >= 3);
                auto atom = std::dynamic_pointer_cast<MalAtom>(list->at(1));
                assert(atom);
                auto func = std::dynamic_pointer_cast<MalFuncBase>(list->at(2));
                assert(func);
                auto params = std::make_shared<MalList>();
                params->push_back(nullptr); // since first value is always ignored (it's usually the function itself)
                params->push_back(atom->object);
                for (size_t i = 3; i < list->size(); ++i) {
                    params->push_back(list->at(i));
                }

                atom->object = (*func)(params);
                return atom->object;
            });

    env->insert({"+", plus_func});
    env->insert({"-", minus_func});
    env->insert({"*", mul_func});
    env->insert({"/", div_func});
    env->insert({"prn", prn_func});
    env->insert({"pr-str", prn_str_func});
    env->insert({"println", println_func});
    env->insert({"str", str_func});
    env->insert({"read-string", read_string_func});
    env->insert({"slurp", slurp_func});
    env->insert({"list", list_func});
    env->insert({"list?", is_list_func});
    env->insert({"empty?", is_empty_func});
    env->insert({"count", count_func});
    env->insert({"=", eq_func});
    env->insert({">", gt_func});
    env->insert({">=", gte_func});
    env->insert({"<", lt_func});
    env->insert({"<=", lte_func});
    env->insert({"eval", eval_func});
    env->insert({"atom", atom_func});
    env->insert({"atom?", is_atom_func});
    env->insert({"deref", deref_func});
    env->insert({"reset!", reset_func});
    env->insert({"swap!", swap_func});

    rep("(def! not (fn* (a) (if a false true)))", env);
    rep("(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))", env);

    return env;
}