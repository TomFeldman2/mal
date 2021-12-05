//
// Created by tomfe on 30/11/2021.
//

#include <cassert>
#include <iostream>
#include <fstream>
#include <chrono>
#include "../include/core_ns.h"
#include "../include/rep.h"
#include "../include/Reader.h"
#include "../include/Error.h"

MalBoolPtr isType(const MalListPtr &params, MalObject::Type type);

MalBoolPtr isBoolLiteral(const MalListPtr &params, bool is_true);

MalHashMapPtr addElementsToMap(const MalHashMapPtr &map, const MalListPtr &params, size_t start_pos);

EnvironmentPtr getCoreEnv() {
    auto env = std::make_shared<Environment>();

    auto plus_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return std::make_shared<MalNumber>(std::dynamic_pointer_cast<MalNumber>(params->at(1))->value
                                                   + std::dynamic_pointer_cast<MalNumber>(params->at(2))->value);
            });

    auto minus_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return std::make_shared<MalNumber>(std::dynamic_pointer_cast<MalNumber>(params->at(1))->value
                                                   - std::dynamic_pointer_cast<MalNumber>(params->at(2))->value);
            });

    auto mul_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return std::make_shared<MalNumber>(std::dynamic_pointer_cast<MalNumber>(params->at(1))->value
                                                   * std::dynamic_pointer_cast<MalNumber>(params->at(2))->value);
            });

    auto div_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return std::make_shared<MalNumber>(std::dynamic_pointer_cast<MalNumber>(params->at(1))->value
                                                   / std::dynamic_pointer_cast<MalNumber>(params->at(2))->value);
            });

    auto prn_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                std::cout << params->joinElements(false, true) << std::endl;
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

    auto is_list_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isType(params, MalObject::Type::LIST);
            });

    auto is_empty_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                return MalBool::getInstance(std::dynamic_pointer_cast<MalList>(params->at(1))->empty());
            });

    auto count_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                if (params->at(1)->getType() == MalObject::Type::NIL) return std::make_shared<MalNumber>(0);
                return std::make_shared<MalNumber>(std::dynamic_pointer_cast<MalList>(params->at(1))->size());
            });

    auto eq_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                return MalBool::getInstance(*params->at(1) == *params->at(2));
            });

    auto gt_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto first = std::dynamic_pointer_cast<MalNumber>(params->at(1));
                auto second = std::dynamic_pointer_cast<MalNumber>(params->at(2));
                return MalBool::getInstance(first->value > second->value);
            });

    auto gte_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto first = std::dynamic_pointer_cast<MalNumber>(params->at(1));
                auto second = std::dynamic_pointer_cast<MalNumber>(params->at(2));
                return MalBool::getInstance(first->value >= second->value);
            });

    auto lt_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto first = std::dynamic_pointer_cast<MalNumber>(params->at(1));
                auto second = std::dynamic_pointer_cast<MalNumber>(params->at(2));
                return MalBool::getInstance(first->value < second->value);
            });

    auto lte_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto first = std::dynamic_pointer_cast<MalNumber>(params->at(1));
                auto second = std::dynamic_pointer_cast<MalNumber>(params->at(2));
                return MalBool::getInstance(first->value <= second->value);
            });

    // TODO: maybe the env should be passed as parameter
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
                return isType(params, MalObject::Type::ATOM);
            });


    auto is_nil_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isType(params, MalObject::Type::NIL);
            });


    auto is_symbol_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isType(params, MalObject::Type::SYMBOL);
            });

    auto is_keyword_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isType(params, MalObject::Type::KEYWORD);
            });

    auto is_vector_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isType(params, MalObject::Type::VECTOR);
            });

    auto is_map_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isType(params, MalObject::Type::MAP);
            });

    auto is_string_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isType(params, MalObject::Type::STRING);
            });

    auto is_fn_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto elem = params->at(1);

                if (elem->getType() == MalObject::Type::FUNCTION) {
                    auto func = std::static_pointer_cast<MalFuncBase>(elem);
                    return MalBool::getInstance(not func->isMacro());
                }
                return MalBool::getInstance(false);
            });

    auto is_macro_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto elem = params->at(1);

                if (elem->getType() == MalObject::Type::FUNCTION) {
                    auto func = std::static_pointer_cast<MalFuncBase>(elem);
                    return MalBool::getInstance(func->isMacro());
                }
                return MalBool::getInstance(false);
            });

    auto is_number_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isType(params, MalObject::Type::NUMBER);
            });

    auto is_seq_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                switch (params->at(1)->getType()) {
                    case MalObject::Type::LIST:
                    case MalObject::Type::VECTOR:
                        return MalBool::getInstance(true);
                    default:
                        return MalBool::getInstance(false);
                }
            });

    auto is_true_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isBoolLiteral(params, true);
            });

    auto is_false_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                return isBoolLiteral(params, false);
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
                auto func_params = std::make_shared<MalList>();
                func_params->push_back(func);
                func_params->push_back(atom->object);
                for (size_t i = 3; i < params->size(); ++i) {
                    func_params->push_back(params->at(i));
                }

                atom->object = (*func)(func_params);
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

                for (size_t i = 1; i < params->size(); ++i) {
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
            });

    auto nth_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto list = std::dynamic_pointer_cast<MalList>(params->at(1));
                assert(list);
                auto index = std::dynamic_pointer_cast<MalNumber>(params->at(2));
                assert(index);
                assert(index->value >= 0);
                if (list->size() <= static_cast<size_t>(index->value)) {
                    throw MalException(std::make_shared<MalSymbol>(
                            "Out of bounds in nth function. Got index " + std::to_string(index->value)
                            + ", but list size is " + std::to_string(list->size())));
                }
                return list->at(index->value);
            });

    auto first_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto param = params->at(1);
                const auto &nil = std::static_pointer_cast<MalObject>(MalNil::getInstance());
                if (param == nil) {
                    return nil;
                }
                auto list = std::dynamic_pointer_cast<MalList>(param);
                assert(list);
                if (list->empty()) {
                    return nil;
                }
                return list->at(0);
            });

    auto rest_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto param = params->at(1);
                auto rest_list = std::make_shared<MalList>();
                if (param == MalNil::getInstance()) {
                    return rest_list;
                }
                auto list = std::dynamic_pointer_cast<MalList>(param);
                assert(list);
                if (not list->empty()) {
                    rest_list->insert(rest_list->begin(), std::next(list->begin()), list->end());
                }

                return rest_list;
            });

    auto apply_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() >= 3);
                auto apply_list = std::make_shared<MalList>();
                auto func = std::dynamic_pointer_cast<MalFuncBase>(params->at(1));
                assert(func);
                apply_list->push_back(func);

                for (size_t i = 2; i < params->size() - 1; ++i) {
                    apply_list->push_back(params->at(i));
                }

                auto list = std::dynamic_pointer_cast<MalList>(params->at(params->size() - 1));
                assert(list);
                apply_list->insert(apply_list->end(), list->begin(), list->end());

                return (*func)(apply_list);
            });

    auto map_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto func = std::dynamic_pointer_cast<MalFuncBase>(params->at(1));
                assert(func);

                auto list = std::dynamic_pointer_cast<MalList>(params->at(2));
                assert(list);

                auto result_list = std::make_shared<MalList>();

                auto tmp_param_list = std::make_shared<MalList>();
                tmp_param_list->push_back(func);

                for (size_t i = 0; i < list->size(); ++i) {
                    tmp_param_list->push_back(list->at(i));
                    auto result = (*func)(tmp_param_list);
                    tmp_param_list->pop_back();
                    result_list->push_back(result);
                }

                return result_list;
            });

    auto keyword_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);

                auto elem = params->at(1);
                if (elem->getType() == MalObject::Type::KEYWORD) {
                    return std::static_pointer_cast<MalKeyword>(elem);
                }
                auto str = std::dynamic_pointer_cast<MalString>(elem);
                assert(str);
                return std::make_shared<MalKeyword>(":" + str->value);
            });

    auto symbol_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto str = std::dynamic_pointer_cast<MalString>(params->at(1));
                assert(str);
                return std::make_shared<MalSymbol>(str->value);
            });

    auto vector_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                auto vec = std::make_shared<MalList>(false);

                if (params->size() > 1) {
                    vec->insert(vec->begin(), std::next(params->begin()), params->end());
                }
                return vec;
            });

    auto hash_map_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                auto map = std::make_shared<MalHashMap>();
                return addElementsToMap(map, params, 1);
            });

    auto assoc_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() >= 2);
                auto map = std::dynamic_pointer_cast<MalHashMap>(params->at(1));
                assert(map);
                return addElementsToMap(map, params, 2);
            });

    auto dissoc_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() >= 2);
                auto map = std::dynamic_pointer_cast<MalHashMap>(params->at(1));
                assert(map);
                auto mutable_map = std::make_shared<MalHashMap>(*map);

                for (size_t i = 2; i < params->size(); ++i) {
                    mutable_map->erase(params->at(i));
                }
                return mutable_map;
            });

    auto get_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                const MalObjectPtr &nil = MalNil::getInstance();

                auto elem = params->at(1);
                if (elem == nil) {
                    return nil;
                }

                auto map = std::dynamic_pointer_cast<MalHashMap>(elem);
                assert(map);
                auto key = params->at(2);
                if (map->contains(key)) {
                    return map->at(key);
                }

                return nil;
            });

    auto contains_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                const MalObjectPtr &nil = MalNil::getInstance();

                auto elem = params->at(1);
                if (elem == nil) {
                    return nil;
                }

                auto map = std::dynamic_pointer_cast<MalHashMap>(elem);
                assert(map);
                return std::static_pointer_cast<MalObject>(MalBool::getInstance(map->contains(params->at(2))));
            });

    auto keys_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                const MalObjectPtr &nil = MalNil::getInstance();

                auto elem = params->at(1);
                if (elem == nil) {
                    return nil;
                }

                auto map = std::dynamic_pointer_cast<MalHashMap>(elem);
                assert(map);

                auto keys = std::make_shared<MalList>();
                for (const auto&[key, _]: *map) {
                    keys->push_back(key);
                }
                return std::static_pointer_cast<MalObject>(keys);
            });

    auto vals_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                const MalObjectPtr &nil = MalNil::getInstance();

                auto elem = params->at(1);
                if (elem == nil) {
                    return nil;
                }

                auto map = std::dynamic_pointer_cast<MalHashMap>(elem);
                assert(map);

                auto vals = std::make_shared<MalList>();
                for (const auto&[_, value]: *map) {
                    vals->push_back(value);
                }
                return std::static_pointer_cast<MalObject>(vals);
            });

    auto throw_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                throw MalException(params->at(1));
                return nullptr;
            });

    auto readline_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto prompt = std::dynamic_pointer_cast<MalString>(params->at(1));
                assert(prompt);
                std::cout << prompt->toString(false);

                MalObjectPtr result = nullptr;
                std::string line;
                if (std::getline(std::cin, line)) {
                    result = std::make_shared<MalString>(line);
                } else {
                    result = MalNil::getInstance();
                }

                return result;
            });

    auto time_ms_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 1);
                auto time = std::chrono::system_clock::now();
                auto ms = time.time_since_epoch().count();
                return std::make_shared<MalNumber>(ms);
            });

    auto seq_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 2);
                auto elem = params->at(1);

                MalObjectPtr nil = MalNil::getInstance();
                MalObjectPtr result = nullptr;

                switch (elem->getType()) {
                    case MalObject::Type::LIST:
                    case MalObject::Type::VECTOR: {
                        auto list = std::static_pointer_cast<MalList>(elem);
                        if (list->empty()) {
                            return nil;
                        }

                        if (list->is_list) {
                            result = list;
                        } else {
                            result = list->cloneAsList();
                        }
                        break;
                    }
                    case MalObject::Type::STRING: {
                        auto str = std::static_pointer_cast<MalString>(elem);
                        if (str->value.empty()) {
                            return nil;
                        }

                        auto list = std::make_shared<MalList>();
                        for (const auto &c: str->value) {
                            list->push_back(std::make_shared<MalString>(std::string(1, c)));
                        }
                        result = list;
                        break;
                    }
                    case MalObject::Type::NIL:
                        return nil;
                    default:
                        assert(false);
                }

                return result;
            });

    auto conj_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() >= 3);
                auto list = std::dynamic_pointer_cast<MalList>(params->at(1));
                assert(list);
                auto clone = std::make_shared<MalList>(*list);
                auto insertion_method = clone->is_list ? &MalList::push_front : &MalList::push_back;
                for (size_t i = 2; i < params->size(); ++i) {
                    ((*clone).*insertion_method)(params->at(i));
                }
                return clone;
            });

    auto meta_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size()== 2);
                auto object = std::dynamic_pointer_cast<MalWithMeta>(params->at(1));
                assert(object);
                return object->getMetadata();
            });

    auto with_meta_func = std::make_shared<MalCoreFunc>(
            [](auto params) {
                assert(params->size() == 3);
                auto object = std::static_pointer_cast<MalWithMeta>(params->at(1));
                assert(object);
                auto clone = std::shared_ptr<MalWithMeta>(object->clone());
                clone->setMetadata(params->at(2));
                return clone;
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
    env->insert({"nil?", is_nil_func});
    env->insert({"true?", is_true_func});
    env->insert({"false?", is_false_func});
    env->insert({"symbol?", is_symbol_func});
    env->insert({"keyword?", is_keyword_func});
    env->insert({"vector?", is_vector_func});
    env->insert({"sequential?", is_seq_func});
    env->insert({"map?", is_map_func});
    env->insert({"fn?", is_fn_func});
    env->insert({"string?", is_string_func});
    env->insert({"number?", is_number_func});
    env->insert({"macro?", is_macro_func});
    env->insert({"deref", deref_func});
    env->insert({"reset!", reset_func});
    env->insert({"swap!", swap_func});
    env->insert({"cons", cons_func});
    env->insert({"concat", concat_func});
    env->insert({"vec", vec_func});
    env->insert({"nth", nth_func});
    env->insert({"first", first_func});
    env->insert({"rest", rest_func});
    env->insert({"apply", apply_func});
    env->insert({"map", map_func});
    env->insert({"symbol", symbol_func});
    env->insert({"keyword", keyword_func});
    env->insert({"vector", vector_func});
    env->insert({"hash-map", hash_map_func});
    env->insert({"assoc", assoc_func});
    env->insert({"dissoc", dissoc_func});
    env->insert({"get", get_func});
    env->insert({"contains?", contains_func});
    env->insert({"keys", keys_func});
    env->insert({"vals", vals_func});
    env->insert({"throw", throw_func});
    env->insert({"readline", readline_func});
    env->insert({"time-ms", time_ms_func});
    env->insert({"seq", seq_func});
    env->insert({"conj", conj_func});
    env->insert({"meta", meta_func});
    env->insert({"with-meta", with_meta_func});

    return env;
}

MalBoolPtr isType(const MalListPtr &params, const MalObject::Type type) {
    assert(params->size() == 2);
    return MalBool::getInstance(params->at(1)->getType() == type);
}

MalBoolPtr isBoolLiteral(const MalListPtr &params, const bool is_true) {
    assert(params->size() == 2);
    auto value = params->at(1);
    if (value->getType() != MalObject::Type::BOOLEAN) {
        return MalBool::getInstance(false);
    }

    return MalBool::getInstance(value->isTrue() == is_true);
}

MalHashMapPtr addElementsToMap(const MalHashMapPtr &map, const MalListPtr &params, const size_t start_pos) {
    assert((params->size() - start_pos) % 2 == 0);

    auto mutable_map = std::make_shared<MalHashMap>(*map);
    for (size_t i = 0; i < (params->size() - start_pos) / 2; ++i) {
        auto key = params->at(start_pos + 2 * i);
        auto value = params->at(start_pos + 1 + 2 * i);
        (*mutable_map)[key] = value;
    }

    return mutable_map;
}