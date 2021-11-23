//
// Created by tomfe on 22/11/2021.
//

#include <numeric>
#include <algorithm>
#include "types.h"

/**
 *
 * MalAtom
 *
 */

MalInt::MalInt(const int value) : value(value) {}


std::string MalInt::toString() const {
    return std::to_string(value);
}

MalType::Type MalInt::getType() const {
    return MalType::Type::Integer;
}

MalString::MalString(const std::string_view &value) : value(value) {}

std::string MalString::toString() const {
    return std::string(value);
}

MalType::Type MalString::getType() const {
    return MalType::Type::String;
}


MalSymbol::MalSymbol(const std::string &value) : value(value) {}

MalSymbol::MalSymbol(const std::string_view &value) : value(std::string(value)) {}

std::string MalSymbol::toString() const {
    return value;
}

MalType::Type MalSymbol::getType() const {
    return MalType::Type::Symbol;
}


/**
 *
 * MalList
 *
 */

MalList::MalList(const bool isList) : is_list(isList) {}

MalList::MalList(const std::string &macro,const std::shared_ptr<MalType> &type) : is_list(true) {
    list.push_back(std::make_shared<MalSymbol>(macro));
    list.push_back(type);
}


void MalList::push_back(const std::shared_ptr<MalType> &type) {
    list.push_back(type);
}

std::string MalList::toString() const {
    const auto open = is_list ? "(" : "[";
    const auto close = is_list ? ")" : "]";
    std::string list_str;

    if (not list.empty()) {
        list_str = std::accumulate(std::next(list.begin()), list.end(), list[0]->toString(),
                                   [](std::string &str, auto type) { return str + " " + type->toString(); });
    }

    return open + list_str + close;
}

bool MalList::empty() const {
    return list.empty();
}

MalList::iterator MalList::begin() const {
    return list.begin();
}

MalList::iterator MalList::end() const {
    return list.end();
}

MalType::Type MalList::getType() const {
    return MalType::Type::Iterable;
}

MalList::value_type const &MalList::at(size_t n) const {
    return list.at(n);
}

size_t MalList::size() const {
    return list.size();
}

/**
 *
 * MalHashMap
 *
 */

void MalHashMap::insert(const value_type &pair) {
    hash_map.insert(pair);
}

std::string MalHashMap::toString() const {
    std::string map_str;
    if (not hash_map.empty()) {
        map_str = std::accumulate(std::next(hash_map.begin()), hash_map.end(), pairToString(*hash_map.begin()),
                                  [](std::string &str, auto pair) { return str + " " + pairToString(pair); });
    }


    return "{" + map_str + "}";
}

std::string MalHashMap::pairToString(const std::pair<std::shared_ptr<MalType>, std::shared_ptr<MalType>> &pair) {
    return pair.first->toString() + " " + pair.second->toString();
}

MalType::Type MalHashMap::getType() const {
    return MalType::Type::Map;
}

MalHashMap::iterator MalHashMap::begin() const {
    return hash_map.begin();
}

MalHashMap::iterator MalHashMap::end() const {
    return hash_map.end();
}

MalHashMap::iterator MalHashMap::insert(MalHashMap::iterator position, const MalHashMap::value_type &val) {
    return hash_map.insert(position, val);
}


