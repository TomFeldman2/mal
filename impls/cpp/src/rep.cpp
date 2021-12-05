//
// Created by tomfe on 23/11/2021.
//

#include <functional>
#include <cassert>
#include <iostream>
#include "../include/rep.h"
#include "../include/Reader.h"
#include "../include/Error.h"

using std::string_literals::operator ""s;

MalObjectPtr evalAst(const MalObjectPtr &ast, const EnvironmentPtr &env);

MalListPtr evalList(const MalListPtr &list, const EnvironmentPtr &env);

MalHashMapPtr evalMap(const std::shared_ptr<MalHashMap> &map, const EnvironmentPtr &env);

bool isListStartsWithSymbol(const MalListPtr &list, const std::string &value);

MalObjectPtr quasiQuote(const MalObjectPtr &object);

MalListPtr quasiQuoteListHandler(const MalListPtr &list);

MalObjectPtr doDef(EnvironmentPtr &env, const std::shared_ptr<MalList> &list, bool set_macro);

MalFuncPtr isMacroCall(const MalObjectPtr &ast, const EnvironmentPtr &env);

MalObjectPtr macroExpand(MalObjectPtr &ast, const EnvironmentPtr &env);


MalObjectPtr EVAL(MalObjectPtr ast, EnvironmentPtr env) {
    while (true) {
        ast = macroExpand(ast, env);
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
                    return doDef(env, list, false);
                }

                if (symbol->value == "defmacro!") {
                    return doDef(env, list, true);
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

                if (symbol->value == "quote") {
                    assert(list->size() >= 2);
                    return list->at(1);
                }

                if (symbol->value == "quasiquote") {
                    assert(list->size() >= 2);
                    ast = quasiQuote(list->at(1));
                    continue;
                }

                if (symbol->value == "quasiquoteexpand") {
                    assert(list->size() >= 2);
                    return quasiQuote(list->at(1));
                }

                if (symbol->value == "macroexpand") {
                    assert(list->size() >= 2);
                    auto macro = list->at(1);
                    return macroExpand(macro, env);
                }

                if (symbol->value == "try*") {
                    assert(list->size() >= 2);
                    try {
                        return EVAL(list->at(1), env);
                    } catch (const MalException& e){
                        if (list->size() == 2) {
                            throw;
                        }

                        auto catch_exp = std::dynamic_pointer_cast<MalList>(list->at(2));

                        assert(catch_exp);
                        if (not isListStartsWithSymbol(catch_exp, "catch*")) {
                            throw;
                        }

                        auto new_env = std::make_shared<Environment>(env);

                        assert(catch_exp->size() == 3);
                        auto catch_symbol = std::dynamic_pointer_cast<MalSymbol>(catch_exp->at(1));
                        assert(catch_symbol);
                        new_env->insert({catch_symbol->value, e.object});
                        env = new_env;
                        ast = catch_exp->at(2);
                        continue;
                    }
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
        } else {
            return evalAst(ast, env);
        }
    }
}

MalObjectPtr doDef(EnvironmentPtr &env, const std::shared_ptr<MalList> &list, const bool set_macro) {
    assert(list->size() == 3);
    auto key = std::dynamic_pointer_cast<MalSymbol>(list->at(1));
    auto value = EVAL(list->at(2), env);
    if (set_macro) {
        auto macro_func = std::dynamic_pointer_cast<MalFunc>(value);
        assert(macro_func);
        auto clone = std::shared_ptr<MalFunc>(macro_func->clone());
        clone->is_macro = true;
        value = clone;
    }

    env->insert({key->value, value});
    return value;
}

MalObjectPtr quasiQuote(const MalObjectPtr &object) {
    switch (object->getType()) {
        case MalObject::Type::LIST: {
            auto list = std::static_pointer_cast<MalList>(object);

            if (list->empty()) return list;

            if (isListStartsWithSymbol(list, "unquote")) {
                assert(list->size() >= 2);
                return list->at(1);
            }

            return quasiQuoteListHandler(list);
        }
        case MalObject::Type::VECTOR: {
            auto result_list = std::make_shared<MalList>();
            result_list->push_back(std::make_shared<MalSymbol>("vec"s));
            auto vec = std::static_pointer_cast<MalList>(object);
            result_list->push_back(quasiQuoteListHandler(vec));
            return result_list;
        }

        case MalObject::Type::MAP:
        case MalObject::Type::SYMBOL: {
            auto result_list = std::make_shared<MalList>();
            result_list->push_back(std::make_shared<MalSymbol>("quote"s));
            result_list->push_back(object);
            return result_list;
        }

        default:
            return object;
    }
}

MalListPtr quasiQuoteListHandler(const MalListPtr &list) {
    auto result_list = std::make_shared<MalList>();
    for (int i = list->size() - 1; i >= 0; --i) {
        auto elt = list->at(i);
        if (elt->getType() == MalObject::Type::LIST) {
            auto elt_list = std::static_pointer_cast<MalList>(elt);
            if (isListStartsWithSymbol(elt_list, "splice-unquote")) {
                assert(elt_list->size() >= 2);
                auto concat_list = std::make_shared<MalList>();
                concat_list->push_back(std::make_shared<MalSymbol>("concat"s));

                concat_list->push_back(elt_list->at(1));
                concat_list->push_back(result_list);
                result_list = concat_list;
                continue;
            }
        }

        auto cons_list = std::make_shared<MalList>();
        cons_list->push_back(std::make_shared<MalSymbol>("cons"s));
        cons_list->push_back(quasiQuote(elt));
        cons_list->push_back(result_list);

        result_list = cons_list;
    }

    return result_list;
}

bool isListStartsWithSymbol(const MalListPtr &list, const std::string &value) {
    if (list->empty()) return false;
    auto first = list->at(0);

    return first->getType() == MalObject::Type::SYMBOL and std::static_pointer_cast<MalSymbol>(first)->value == value;

}

MalFuncPtr isMacroCall(const MalObjectPtr &ast, const EnvironmentPtr &env) {
        if (ast->getType() != MalObject::Type::LIST) {
            return nullptr;
        }

        auto list = std::static_pointer_cast<MalList>(ast);

        if (list->empty() or list->at(0)->getType() != MalObject::Type::SYMBOL) {
            return nullptr;
        }

        auto symbol = std::static_pointer_cast<MalSymbol>(list->at(0));
        try {
            auto func = std::dynamic_pointer_cast<MalFunc>(env->at(symbol->value));
            if (func and func->is_macro) {
                return func;
            }
        } catch (const MalException&) {}

        return nullptr;
}

MalObjectPtr macroExpand(MalObjectPtr &ast, const EnvironmentPtr &env) {
    MalFuncPtr func = isMacroCall(ast, env);
    while (func) {
        ast = (*func)(std::static_pointer_cast<MalList>(ast));
        func = isMacroCall(ast, env);
    }
    return ast;
}

MalObjectPtr evalAst(const MalObjectPtr &ast, const EnvironmentPtr &env) {
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

MalListPtr evalList(const MalListPtr &list, const EnvironmentPtr &env) {
    auto eval_list = std::make_shared<MalList>(list->is_list);
    std::transform(list->begin(), list->end(), std::back_inserter(*eval_list),
                   [&env](auto type) { return EVAL(type, env); });

    return eval_list;
}

std::shared_ptr<MalHashMap> evalMap(const std::shared_ptr<MalHashMap> &map, const EnvironmentPtr &env) {
    auto eval_map = std::make_shared<MalHashMap>();
    std::transform(map->begin(), map->end(), std::inserter(*eval_map, eval_map->begin()),
                   [&env](auto &pair) { return MalHashMap::value_type{pair.first, EVAL(pair.second, env)}; });
    return eval_map;
}

MalObjectPtr READ(const std::string &input) {
    return readStr(input);
}

std::string PRINT(const MalObjectPtr &ast) {
    auto str = ast->toString();
    return str;
}

std::string rep(const std::string &input, const EnvironmentPtr &env) {

    auto ast = READ(input);
    auto result = EVAL(ast, env);
    return PRINT(result);
}
