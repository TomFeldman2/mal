//
// Created by tomfe on 30/11/2021.
//

#include <cassert>
#include <iostream>
#include <fstream>
#include "../include/core_ns.h"
#include "../include/rep.h"
#include "../include/Reader.h"

EnvironmentPtr getCoreEnv() {
    auto env = std::make_shared<Environment>();

    auto plus_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(params->at(1))->value
                                                + std::dynamic_pointer_cast<MalInt>(params->at(2))->value);
            });

    auto minus_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(params->at(1))->value
                                                - std::dynamic_pointer_cast<MalInt>(params->at(2))->value);
            });

    auto mul_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(params->at(1))->value
                                                * std::dynamic_pointer_cast<MalInt>(params->at(2))->value);
            });

    auto div_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(params->at(1))->value
                                                / std::dynamic_pointer_cast<MalInt>(params->at(2))->value);
            });

    auto prn_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                std::cout << params->joinElements(false) << std::endl;
                return MalNil::getInstance();
            });

    auto println_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                std::cout << params->joinElements(false, false) << std::endl;
                return MalNil::getInstance();
            });

    auto prn_str_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return std::make_shared<MalString>(params->joinElements(false));
            });

    auto str_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return std::make_shared<MalString>(params->joinElements(false, false, ""));
            });

    auto read_string_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto str = std::dynamic_pointer_cast<MalString>(params->at(1));
                assert(str);
                return readStr(str->value);
            });

    auto slurp_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto str = std::dynamic_pointer_cast<MalString>(params->at(1));
                assert(str);
                std::ifstream file(str->value);
                std::string file_str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                return std::make_shared<MalString>(file_str);
            });


    auto list_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                auto new_params = std::make_shared<MalList>(params->is_list);
                std::copy(std::next(params->begin()), params->end(), std::back_inserter(*new_params));
                return new_params;
            });

    auto is_params_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                return MalBool::getInstance(params->at(1)->getType() == MalObject::Type::LIST);
            });

    auto is_empty_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                return MalBool::getInstance(std::dynamic_pointer_cast<MalList>(params->at(1))->empty());
            });

    auto count_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                if (params->at(1)->getType() == MalObject::Type::NIL) return std::make_shared<MalInt>(0);
                return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalList>(params->at(1))->size());
            });

    auto eq_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return MalBool::getInstance(*params->at(1) == *params->at(2));
            });

    auto gt_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(params->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(params->at(2));
                return MalBool::getInstance(first->value > second->value);
            });

    auto gte_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(params->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(params->at(2));
                return MalBool::getInstance(first->value >= second->value);
            });

    auto lt_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(params->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(params->at(2));
                return MalBool::getInstance(first->value < second->value);
            });

    auto lte_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto first = std::dynamic_pointer_cast<MalInt>(params->at(1));
                auto second = std::dynamic_pointer_cast<MalInt>(params->at(2));
                return MalBool::getInstance(first->value <= second->value);
            });

    auto eval_func = std::make_shared<MalCoreFunc>(
            [&env](auto params) {
                assert(params->size() == 2);
                return EVAL(params->at(1), env);
            });

    auto atom_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                return std::make_shared<MalAtom>(params->at(1));
            });

    auto is_atom_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                return MalBool::getInstance(params->at(1)->getType() == MalObject::Type::ATOM);
            });

    auto deref_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto atom = std::dynamic_pointer_cast<MalAtom>(params->at(1));
                assert(atom);
                return atom->object;
            });

    auto reset_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto atom = std::dynamic_pointer_cast<MalAtom>(params->at(1));
                assert(atom);
                atom->object = params->at(2);
                return atom->object;
            });

    auto swap_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() >= 3);
                auto atom = std::dynamic_pointer_cast<MalAtom>(params->at(1));
                assert(atom);
                auto func = std::dynamic_pointer_cast<MalFuncBase>(params->at(2));
                assert(func);
                auto list = std::make_shared<MalList>();
                params->push_back(nullptr); // since first value is always ignored (it's usually the function itself)
                params->push_back(atom->object);
                for (size_t i = 3; i < params->size(); ++i) {
                    list->push_back(params->at(i));
                }

                atom->object = (*func)(list);
                return atom->object;
            });


    auto cons_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto list = std::dynamic_pointer_cast<MalList>(params->at(2));
                assert(list);
                auto new_list = list->cloneAsList();
                new_list->push_front(params->at(1));
                return new_list;
            });

    auto concat_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                auto concat_list = std::make_shared<MalList>();

                for(size_t i = 1; i < params->size(); ++i) {
                    auto param = params->at(i);
                    auto list = std::dynamic_pointer_cast<MalList>(param);
                    assert(list);
                    concat_list->insert(concat_list->end(), list->begin(), list->end());
                }

                return concat_list;
            });

    auto vec_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto list = std::dynamic_pointer_cast<MalList>(params->at(1));
                assert(list);
                return list->cloneAsVector();
 ;
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
    env->insert({"params?", is_params_func});
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
    env->insert({"cons", cons_func});
    env->insert({"concat", concat_func});
    env->insert({"vec", vec_func});

    rep("(def! not (fn* (a) (if a false true)))", env);
    rep("(def! load-file (fn* (f) (eval (read-string (str \"(do \" (slurp f) \"\nnil)\")))))", env);

    return env;
}