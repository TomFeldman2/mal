//
// Created by tomfe on 22/11/2021.
//

#include <numeric>
#include "types.h"

/**
 *
 * MalAtom
 *
 */

//MalAtom::~MalAtom() = default;

MalAtom::MalAtom(const std::string &value) : value(value) {}

MalAtom::MalAtom(const std::string_view &value) : value(value) {}

std::string MalAtom::toString() const {
    return std::string(value);
}

//MalInt::MalInt(const std::string_view &value) : MalAtom(value) {}
//
//MalString::MalString(const std::string_view &value) : MalAtom(value) {}

/**
 *
 * MalList
 *
 */

MalList::MalList(const bool isList) : is_list(isList) {}

MalList::MalList(const std::string &macro, MalType *type) : is_list(true) {
    list.push_back(new MalAtom(macro));
    list.push_back(type);
}

MalList::~MalList() {
    for (auto type: list) {
        delete type;
    }
}

void MalList::append(MalType *type) {
    list.push_back(type);
}

std::string MalList::toString() const {
    const auto open = is_list ? "(" : "[";
    const auto close = is_list ? ")" : "]";
    std::string list_str;

    if (not list.empty()) {
        list_str = std::accumulate(std::next(list.begin()), list.end(), list[0]->toString(),
                                   [](std::string &str, MalType *type) { return str + " " + type->toString(); });
    }

    return open + list_str + close;
}

void MalHashMap::insert(MalType *key, MalType *value) {
    hash_map.emplace(key, value);
}

std::string MalHashMap::toString() const {
    std::string map_str;
    if (not hash_map.empty()) {
        map_str = std::accumulate(std::next(hash_map.begin()), hash_map.end(), pairToString(*hash_map.begin()),
                                   [this](std::string &str, auto pair) { return str + " " + pairToString(pair); });
    }


    return "{" + map_str + "}";
}

std::string MalHashMap::pairToString(const std::pair<MalType*, MalType*> &pair) const {
    return pair.first->toString() + " " + pair.second->toString();
}

MalHashMap::~MalHashMap() {
    for(const auto &[key, value]: hash_map) {
        delete key;
        delete value;
    }
}
