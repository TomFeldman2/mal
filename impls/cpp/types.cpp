//
// Created by tomfe on 22/11/2021.
//

#include <numeric>
#include <algorithm>
#include <iostream>
#include "types.h"

/**
 *
 * MalObject
 *
 */

bool MalObject::isTrue() const {
    return true;
}

bool MalObject::operator==(const MalObject &other) const {
    if (not areSameTypes(*this, other)) return false;
    return equals(other);
}

bool MalObject::areSameTypes(const MalObject &first, const MalObject &second) {
    return first.getType() == second.getType();
}

std::ostream &operator<<(std::ostream &os, const MalObject &object) {
    os << object.toString();
    return os;
}


/**
 *
 * MalAtom
 *
 */

std::shared_ptr<MalNil> MalNil::nil = nullptr;

std::string MalNil::toString() const {
    return "nil";
}

MalObject::Type MalNil::getType() const {
    return Type::NIL;
}

bool MalNil::isTrue() const {
    return false;
}

const std::shared_ptr<MalNil> &MalNil::getInstance() {
    if (nil) {
        return nil;
    }
    nil =  std::shared_ptr<MalNil>(new MalNil());
    return nil;
}

bool MalNil::equals(const MalObject &other) const {
    return true;
}

/**
 *
 * MalInt
 *
 */

MalInt::MalInt(const int value) : value(value) {}


std::string MalInt::toString() const {
    return std::to_string(value);
}

MalObject::Type MalInt::getType() const {
    return MalObject::Type::INTEGER;
}

bool MalInt::equals(const MalObject &other) const {
    return static_cast<const MalInt &>(other).value == value;
}

/**
 *
 * MalString
 *
 */

MalString::MalString(const std::string_view &value) : value(value) {}

std::string MalString::toString() const {
    return std::string(value);
}

MalObject::Type MalString::getType() const {
    return Type::STRING;
}

bool MalString::equals(const MalObject &other) const {
    return static_cast<const MalString &>(other).value == value;
}

/**
 *
 * MalSymbol
 *
 */

MalSymbol::MalSymbol(const std::string &value) : value(value) {}

MalSymbol::MalSymbol(const std::string_view &value) : value(std::string(value)) {}

std::string MalSymbol::toString() const {
    return value;
}

MalObject::Type MalSymbol::getType() const {
    return Type::SYMBOL;
}

bool MalSymbol::equals(const MalObject &other) const {
    return static_cast<const MalSymbol &>(other).value == value;
}

/**
 *
 * MalBool
 *
 */

MalObject::Type MalBool::getType() const {
    return Type::BOOLEAN;
}

std::shared_ptr<MalBool> MalBool::getInstance(const bool value) {
    if (value) {
        return MalTrue::getInstance();
    }

    return MalFalse::getInstance();

}

bool MalBool::equals(const MalObject& other) const {
    return isTrue() == other.isTrue();
}

/**
 *
 * MalTrue
 *
 */

std::shared_ptr<MalTrue> MalTrue::true_instance = nullptr;

std::string MalTrue::toString() const {
    return "true";
}

const std::shared_ptr<MalTrue> &MalTrue::getInstance() {
    if (true_instance) {
        return true_instance;
    }
    true_instance =  std::shared_ptr<MalTrue>(new MalTrue());
    return true_instance;
}


/**
 *
 * MalFalse
 *
 */

std::shared_ptr<MalFalse> MalFalse::false_instance = nullptr;

std::string MalFalse::toString() const {
    return "false";
}

bool MalFalse::isTrue() const {
    return false;
}

const std::shared_ptr<MalFalse> &MalFalse::getInstance() {
    if (false_instance) {
        return false_instance;
    }
    false_instance =  std::shared_ptr<MalFalse>(new MalFalse());
    return false_instance;
}


/**
 *
 * MalList
 *
 */

MalList::MalList(const bool isList) : is_list(isList) {}

MalList::MalList(const std::string &macro,const std::shared_ptr<MalObject> &type) : is_list(true) {
    list.push_back(std::make_shared<MalSymbol>(macro));
    list.push_back(type);
}


void MalList::push_back(const std::shared_ptr<MalObject> &type) {
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

MalObject::Type MalList::getType() const {
    return is_list ? Type::LIST : Type::VECTOR;
}

MalList::value_type const &MalList::at(size_t n) const {
    return list.at(n);
}

size_t MalList::size() const {
    return list.size();
}

bool MalList::equals(const MalObject &other) const {
    auto other_list = static_cast<const MalList &>(other);
    if (size() != other_list.size()) return false;

    return std::equal(begin(), end(), other_list.begin(), [](auto first, auto second){ return *first == *second; });
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

std::string MalHashMap::pairToString(const std::pair<std::shared_ptr<MalObject>, std::shared_ptr<MalObject>> &pair) {
    return pair.first->toString() + " " + pair.second->toString();
}

MalObject::Type MalHashMap::getType() const {
    return Type::MAP;
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

bool MalHashMap::equals(const MalObject &other) const {
    auto other_map = static_cast<const MalHashMap &>(other);
    if (hash_map.size() != other_map.hash_map.size()) return false;

    return std::equal(begin(), end(), other_map.begin(),
          [](auto pair1, auto pair2){
                    return (*pair1.first == *pair2.first) and (*pair1.second == *pair2.second);
                    });
}

/**
 *
 * MalFunc
 *
 */

MalFunc::MalFunc(const MalFunc::FuncType &func) : func(func) {}

std::string MalFunc::toString() const {
    return "#<function>";
}

MalObject::Type MalFunc::getType() const {
    return Type::FUNCTION;
}

std::shared_ptr<MalObject> MalFunc::operator()(const std::shared_ptr<MalList> &vec) const{
    return func(vec);
}

bool MalFunc::equals(const MalObject &other) const {
    return this == &other;
}


