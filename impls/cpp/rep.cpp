//
// Created by tomfe on 23/11/2021.
//

#include <functional>
#include <cassert>
#include <iostream>
#include "rep.h"
#include "Reader.h"
#include "Error.h"

std::shared_ptr<MalObject> evalAst(const std::shared_ptr<MalObject> &ast, const std::shared_ptr<Environment> &env);

std::shared_ptr<MalList> evalList(const std::shared_ptr<MalList> &list, const std::shared_ptr<Environment> &env);

std::shared_ptr<MalHashMap> evalMap(const std::shared_ptr<MalHashMap> &map, const std::shared_ptr<Environment> &env);

std::shared_ptr<MalObject> READ(const std::string &input) {
    return readStr(input);
}

std::shared_ptr<MalObject> EVAL(std::shared_ptr<MalObject> ast, std::shared_ptr<Environment> env) {
    while (true) {
        if (ast->getType() == MalObject::Type::LIST) {
            auto list = std::static_pointer_cast<MalList>(ast);
            if (list->empty())
                return ast;

            if (not list->is_list) {
                return evalAst(ast, env);
            }

            if (list->at(0)->getType() == MalObject::Type::SYMBOL) {
                auto symbol = std::static_pointer_cast<MalSymbol>(list->at(0));

                if (symbol->value == "def!") {
                    assert(list->size() == 3);
                    auto key = std::dynamic_pointer_cast<MalSymbol>(list->at(1));
                    auto value = EVAL(list->at(2), env);
                    env->insert({key->value, value});
                    return value;
                }

                if (symbol->value == "let*") {
                    assert(list->size() == 3);
                    auto new_env = std::make_shared<Environment>(env);
                    auto binding_list = std::dynamic_pointer_cast<MalList>(list->at(1));
                    assert(binding_list->size() % 2 == 0);
                    for (size_t i = 0; i < binding_list->size() / 2; ++i) {
                        auto key = std::dynamic_pointer_cast<MalSymbol>(binding_list->at(i * 2));
                        auto value = EVAL(binding_list->at(i * 2 + 1), new_env);
                        new_env->insert({key->value, value});
                    }

                    ast = list->at(2);
                    env = new_env;
                    continue;
                }

                if (symbol->value == "do") {
                    assert(list->size() > 1);
                    for (size_t i = 1; i < list->size() - 1; ++i) {
                        EVAL(list->at(i), env);
                    }

                    ast = list->at(list->size() - 1);
                    continue;
                }

                if (symbol->value == "if") {
                    auto size = list->size();
                    assert(size == 3 or size == 4);
                    auto condition = EVAL(list->at(1), env);
                    if (condition->isTrue()) {
                        ast = list->at(2);
                        continue;
                    }

                    if (size == 3) return MalNil::getInstance();
                    ast = list->at(3);
                    continue;
                }

                if (symbol->value == "fn*") {
                    assert(list->size() == 3);

                    auto params = std::dynamic_pointer_cast<MalList>(list->at(1));
                    auto body = list->at(2);

                    auto func = [=](const auto &exprs) {
                        auto closure = std::make_shared<Environment>(env, params, exprs);
                        return EVAL(body, closure);
                    };

                    auto fn = std::make_shared<MalCoreFunc>(func);
                    return std::make_shared<MalFunc>(body, params, env, fn);
                }

            }
            auto eval_list = std::static_pointer_cast<MalList>(evalAst(ast, env));
            auto func = std::static_pointer_cast<MalFuncBase>(eval_list->at(0));
            if (func->isCoreFunc()) {
                auto core_func = std::static_pointer_cast<MalCoreFunc>(func);
                return (*core_func)(eval_list);
            }
            auto defined_func = std::static_pointer_cast<MalFunc>(func);
            ast = defined_func->ast;
            env = std::make_shared<Environment>(defined_func->env, defined_func->params, eval_list);
        }
        else {
            return evalAst(ast, env);
        }
    }
}

std::shared_ptr<MalObject> evalAst(const std::shared_ptr<MalObject> &ast, const std::shared_ptr<Environment> &env) {
    switch (ast->getType()) {
        case MalObject::Type::SYMBOL:
            return env->at(std::static_pointer_cast<MalSymbol>(ast)->value);

        case MalObject::Type::LIST:
        case MalObject::Type::VECTOR:
            return evalList(std::static_pointer_cast<MalList>(ast), env);

        case MalObject::Type::MAP:
            return evalMap(std::static_pointer_cast<MalHashMap>(ast), env);

        default:
            return ast;
    }
}


std::shared_ptr<MalList> evalList(const std::shared_ptr<MalList> &list, const std::shared_ptr<Environment> &env) {
    auto eval_list = std::make_shared<MalList>(list->is_list);
    std::transform(list->begin(), list->end(), std::back_inserter(*eval_list),
                   [&env](auto type) { return EVAL(type, env); });

    return eval_list;
}


std::shared_ptr<MalHashMap> evalMap(const std::shared_ptr<MalHashMap> &map, const std::shared_ptr<Environment> &env) {
    auto eval_map = std::make_shared<MalHashMap>();
    std::transform(map->begin(), map->end(), std::inserter(*eval_map, eval_map->begin()),
                   [&env](auto &pair) { return MalHashMap::value_type{pair.first, EVAL(pair.second, env)}; });
    return eval_map;
}


std::string PRINT(const std::shared_ptr<MalObject> &ast) {
    auto str = ast->toString();
    return str;
}

std::string rep(const std::string &input, const std::shared_ptr<Environment> &env) {

    auto ast = READ(input);
    auto result = EVAL(ast, env);
    return PRINT(result);
}


