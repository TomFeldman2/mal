//
// Created by tomfe on 23/11/2021.
//

#include <functional>
#include <cassert>
#include "rep.h"
#include "Reader.h"
#include "Error.h"

std::shared_ptr<MalType> evalAst(const std::shared_ptr<MalType> &ast, const Env &env);

std::shared_ptr<MalType> evalList(const std::shared_ptr<MalList> &list, const Env &env);

std::shared_ptr<MalHashMap> evalMap(const std::shared_ptr<MalHashMap> &map, const Env &env);

std::shared_ptr<MalType> READ(const std::string &input) {
    return readStr(input);
}

std::shared_ptr<MalType> EVAL(const std::shared_ptr<MalType> &ast, const Env &env) {
    if (ast->getType() == MalType::Type::Iterable) {
        auto list = std::static_pointer_cast<MalList>(ast);
        if (list->empty())
            return ast;
    }

    auto result = evalAst(ast, env);
    return result;
}

std::shared_ptr<MalType> evalAst(const std::shared_ptr<MalType> &ast, const Env &env) {
    switch (ast->getType()) {
        case MalType::Type::Symbol: {
            const auto value = std::static_pointer_cast<MalSymbol>(ast)->value;
            try {
                return env.at(value);
            } catch (const std::out_of_range &) {
                throw Error(value + " wasn't found");
            }
        }


        case MalType::Type::Iterable:
            return evalList(std::static_pointer_cast<MalList>(ast), env);

        case MalType::Type::Map:
            return evalMap(std::static_pointer_cast<MalHashMap>(ast), env);

        default:
            return ast;
    }
}


std::shared_ptr<MalType> evalList(const std::shared_ptr<MalList> &list, const Env &env) {
    auto eval_list = std::make_shared<MalList>(list->is_list);
    std::transform(list->begin(), list->end(), std::back_inserter(*eval_list),
                   [env](auto type) { return EVAL(type, env); });

    if (list->is_list) {
        auto func = std::static_pointer_cast<EnvValue>(eval_list->at(0));
        auto result = (*func)(eval_list);
        return result;
    }

    return eval_list;
}

std::shared_ptr<MalHashMap> evalMap(const std::shared_ptr<MalHashMap> &map, const Env &env) {
    auto eval_map = std::make_shared<MalHashMap>();
    std::transform(map->begin(), map->end(), std::inserter(*eval_map, eval_map->begin()),
                   [env](auto &pair) { return MalHashMap::value_type{pair.first, EVAL(pair.second, env)}; });
    return eval_map;
}


std::string PRINT(const std::shared_ptr<MalType> &ast) {
    auto str = ast->toString();
    return str;
}

std::string rep(const std::string &input) {
    static Env env{
            {"+", std::make_shared<EnvValue>(
                    [](auto list) {
                        assert(list->size() == 3);
                        return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                        + std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
                    })},
            {"-", std::make_shared<EnvValue>(
                    [](auto list) {
                        assert(list->size() == 3);
                        return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                        - std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
                    })},
            {"*", std::make_shared<EnvValue>(
                    [](auto list) {
                        assert(list->size() == 3);
                        return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                        * std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
                    })},
            {"/", std::make_shared<EnvValue>(
                    [](auto list) {
                        assert(list->size() == 3);
                        return std::make_shared<MalInt>(std::dynamic_pointer_cast<MalInt>(list->at(1))->value
                                                        / std::dynamic_pointer_cast<MalInt>(list->at(2))->value);
                    })},
    };

    auto ast = READ(input);
    auto result = EVAL(ast, env);
    return PRINT(result);
}


