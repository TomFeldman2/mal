//
// Created by tomfe on 23/11/2021.
//

#include <functional>
#include <cassert>
#include "rep.h"
#include "Reader.h"
#include "Error.h"

std::shared_ptr<MalType> evalAst(const std::shared_ptr<MalType> &ast, Environment &env);

std::shared_ptr<MalList> evalList(const std::shared_ptr<MalList> &list, Environment &env);

std::shared_ptr<MalHashMap> evalMap(const std::shared_ptr<MalHashMap> &map, Environment &env);

std::shared_ptr<MalType> READ(const std::string &input) {
    return readStr(input);
}

std::shared_ptr<MalType> EVAL(const std::shared_ptr<MalType> &ast, Environment &env) {
    if (ast->getType() == MalType::Type::List) {
        auto list = std::static_pointer_cast<MalList>(ast);
        if (list->empty())
            return ast;
        
        if (list->at(0)->getType() == MalType::Type::Symbol) {
            auto symbol = std::static_pointer_cast<MalSymbol>(list->at(0));
            
            if (symbol->value == "def!") {
                assert(list->size() == 3);
                auto key = std::dynamic_pointer_cast<MalSymbol>(list->at(1));
                auto value = EVAL(list->at(2), env);
                env.insert({key->value, value});
                return value;
            }
            
            if (symbol->value == "let*") {
                assert(list->size() == 3);
                Environment new_env{&env};
                auto binding_list = std::dynamic_pointer_cast<MalList>(list->at(1));
                assert(binding_list->size() % 2 == 0);
                for(size_t i = 0; i < binding_list->size() / 2; ++i) {
                    auto key = std::dynamic_pointer_cast<MalSymbol>(binding_list->at(i * 2));
                    auto value = EVAL(binding_list->at(i*2 + 1), new_env);
                    new_env.insert({key->value, value});
                }
                return EVAL(list->at(2), new_env);
            }
        }
    }

    auto result = evalAst(ast, env);
    return result;
}

std::shared_ptr<MalType> evalAst(const std::shared_ptr<MalType> &ast, Environment &env) {
    switch (ast->getType()) {
        case MalType::Type::Symbol:
            return env.at(std::static_pointer_cast<MalSymbol>(ast)->value);

        case MalType::Type::List:
        case MalType::Type::Vector: {
            auto eval_list = evalList(std::static_pointer_cast<MalList>(ast), env);
            if (eval_list->is_list) {
                auto func = std::static_pointer_cast<MalFunc>(eval_list->at(0));
                return (*func)(eval_list);
            }
            return eval_list;
        }

        case MalType::Type::Map:
            return evalMap(std::static_pointer_cast<MalHashMap>(ast), env);

        default:
            return ast;
    }
}


std::shared_ptr<MalList> evalList(const std::shared_ptr<MalList> &list, Environment &env) {
    auto eval_list = std::make_shared<MalList>(list->is_list);
    std::transform(list->begin(), list->end(), std::back_inserter(*eval_list),
                   [&env](auto type) { return EVAL(type, env); });

    return eval_list;
}


std::shared_ptr<MalHashMap> evalMap(const std::shared_ptr<MalHashMap> &map, Environment &env) {
    auto eval_map = std::make_shared<MalHashMap>();
    std::transform(map->begin(), map->end(), std::inserter(*eval_map, eval_map->begin()),
                   [&env](auto &pair) { return MalHashMap::value_type{pair.first, EVAL(pair.second, env)}; });
    return eval_map;
}


std::string PRINT(const std::shared_ptr<MalType> &ast) {
    auto str = ast->toString();
    return str;
}

std::string rep(const std::string &input, Environment &env) {

    auto ast = READ(input);
    auto result = EVAL(ast, env);
    return PRINT(result);
}


