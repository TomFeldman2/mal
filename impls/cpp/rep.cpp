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

std::shared_ptr<MalObject> EVAL(const std::shared_ptr<MalObject> &ast, const std::shared_ptr<Environment> &env) {
    if (ast->getType() == MalObject::Type::LIST) {
        auto list = std::static_pointer_cast<MalList>(ast);
        if (list->empty())
            return ast;
        
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
                for(size_t i = 0; i < binding_list->size() / 2; ++i) {
                    auto key = std::dynamic_pointer_cast<MalSymbol>(binding_list->at(i * 2));
                    auto value = EVAL(binding_list->at(i*2 + 1), new_env);
                    new_env->insert({key->value, value});
                }
                return EVAL(list->at(2), new_env);
            }

            if (symbol->value == "do") {
                assert(list->size() > 1);
                for (size_t i = 1; i < list->size() - 1; ++i) {
                    EVAL(list->at(i), env);
                }

                return EVAL(list->at(list->size() - 1), env);
            }

            if (symbol->value == "if") {
                auto size = list->size();
                assert(size == 3 or size == 4);
                auto condition = EVAL(list->at(1), env);
                if (condition->isTrue()) {
                    return EVAL(list->at(2), env);
                }

                if (size == 3) return MalNil::getInstance();
                return EVAL(list->at(3), env);
            }

            if (symbol->value == "fn*") {
                assert(list->size() == 3);

                auto func = [=](const auto &exprs){
                    auto  closure = std::make_shared<Environment>(env, std::dynamic_pointer_cast<MalList>(list->at(1)), exprs);
                    return EVAL(list->at(2), closure);
                };

                return std::make_shared<MalFunc>(func);
            }
        }
    }

    auto result = evalAst(ast, env);
    return result;
}

std::shared_ptr<MalObject> evalAst(const std::shared_ptr<MalObject> &ast, const std::shared_ptr<Environment> &env) {
    switch (ast->getType()) {
        case MalObject::Type::SYMBOL:
            return env->at(std::static_pointer_cast<MalSymbol>(ast)->value);

        case MalObject::Type::LIST:
        case MalObject::Type::VECTOR: {
            auto eval_list = evalList(std::static_pointer_cast<MalList>(ast), env);
            if (eval_list->is_list) {
                auto func = std::static_pointer_cast<MalFunc>(eval_list->at(0));
                return (*func)(eval_list);
            }
            return eval_list;
        }

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


