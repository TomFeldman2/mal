//
// Created by tomfe on 22/11/2021.
//

#include <numeric>
#include <algorithm>
#include <iostream>
#include "../include/types.h"

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

bool MalObject::operator!=(const MalObject &other) const {
    return not(*this == other);
}


bool MalObject::areSameTypes(const MalObject &first, const MalObject &second) {
    return first.getType() == second.getType() or (first.isListLike() and second.isListLike());
}

std::ostream &operator<<(std::ostream &os, const MalObject &object) {
    os << object.toString();
    return os;
}

bool MalObject::isListLike() const {
    return false;
}

bool MalObject::equals(const MalObject &other) const {
    return &other == this;
}

/**
 *
 * MalAtom
 *
 */

MalAtom::MalAtom(const MalObjectPtr &object) : object(object) {}

MalObject::Type MalAtom::getType() const {
    return Type::ATOM;
}

std::string MalAtom::toString(const bool readable) const {
    return "(atom " + object->toString(readable) + ")";
}

/**
 *
 * MalValue
 *
 */

std::shared_ptr<MalNil> MalNil::nil = nullptr;

std::string MalNil::toString(const bool readable) const {
    return "nil";
}

MalObject::Type MalNil::getType() const {
    return Type::NIL;
}

bool MalNil::isTrue() const {
    return false;
}

const MalNilPtr &MalNil::getInstance() {
    if (nil) {
        return nil;
    }
    nil = MalNilPtr(new MalNil());
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

std::string MalInt::toString(const bool readable) const {
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

MalString::MalString(const std::string &value) : value(value) {}

MalString::MalString(const std::string_view &value) : value(transformString(value)) {}

std::string MalString::transformString(const std::string_view &str) {
    std::string transformed_str;
    bool is_escaped = false;
    size_t i;
    for (i = 1; i < str.size() - 1; ++i) {
        if (str[i] == '\\' and not is_escaped) {
            is_escaped = true;
            continue;
        }

        if (is_escaped) {
            is_escaped = false;
            switch (str[i]) {
                case '\\':
                case '"':
                    transformed_str += str[i];
                    break;
                case 'n':
                    transformed_str += '\n';
                    break;
                default:
                    transformed_str += '\\' + str[i];
            }
        } else {
            transformed_str += str[i];
        }


    }

    return transformed_str;
}

std::string MalString::toString(const bool readable) const {
    if (not readable) {
        return value;
    }

    std::string readable_str;

    for (size_t i = 0; i < value.size(); ++i) {
        switch (value[i]) {
            case '\\':
            case '"':
                readable_str += '\\';
                readable_str += value[i];
                break;
            case '\n':
                readable_str += "\\n";
                break;
            default:
                readable_str += value[i];
        }

    }

    return '"' + readable_str + '"';
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

std::string MalSymbol::toString(const bool readable) const {
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
 * MalKeyword
 *
 */

MalKeyword::MalKeyword(const std::string &value) : MalSymbol(value) {}

MalKeyword::MalKeyword(const std::string_view &value) : MalSymbol(value) {}

MalObject::Type MalKeyword::getType() const {
    return Type::KEYWORD;
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

bool MalBool::equals(const MalObject &other) const {
    return isTrue() == other.isTrue();
}

/**
 *
 * MalTrue
 *
 */

std::shared_ptr<MalTrue> MalTrue::true_instance = nullptr;

std::string MalTrue::toString(const bool readable) const {
    return "true";
}

const std::shared_ptr<MalTrue> &MalTrue::getInstance() {
    if (true_instance) {
        return true_instance;
    }
    true_instance = std::shared_ptr<MalTrue>(new MalTrue());
    return true_instance;
}

/**
 *
 * MalFalse
 *
 */

std::shared_ptr<MalFalse> MalFalse::false_instance = nullptr;

std::string MalFalse::toString(const bool readable) const {
    return "false";
}

bool MalFalse::isTrue() const {
    return false;
}

const std::shared_ptr<MalFalse> &MalFalse::getInstance() {
    if (false_instance) {
        return false_instance;
    }
    false_instance = std::shared_ptr<MalFalse>(new MalFalse());
    return false_instance;
}

/**
 *
 * MalList
 *
 */

MalList::MalList(const bool isList) : is_list(isList) {}

MalList::MalList(const std::deque<MalObjectPtr> &list, const bool as_list) : list(list), is_list(as_list) {}

MalList::MalList(const std::string &macro, const MalObjectPtr &object) : is_list(true) {
    list.push_back(std::make_shared<MalSymbol>(macro));
    list.push_back(object);
}

void MalList::push_back(const MalObjectPtr &object) {
    list.push_back(object);
}

void MalList::push_front(const MalObjectPtr &object) {
    list.push_front(object);
}

std::string MalList::toString(const bool readable) const {
    const auto open = is_list ? "(" : "[";
    const auto close = is_list ? ")" : "]";
    std::string list_str;

    if (not list.empty()) {
        list_str = joinElements(true, readable);
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

    return std::equal(begin(), end(), other_list.begin(), [](auto first, auto second) { return *first == *second; });
}

std::string MalList::joinElements(bool include_first, const bool readable, const std::string &delimiter) const {
    if (not include_first and list.size() == 1) return "";
    auto begin = include_first ? std::next(list.begin()) : std::next(std::next(list.begin()));
    size_t start_index = include_first ? 0 : 1;

    return std::accumulate(begin, list.end(), list[start_index]->toString(readable),
                           [&delimiter, &readable](std::string &str, auto type) {
                               return str + delimiter + type->toString(readable);
                           });
}

bool MalList::isListLike() const {
    return true;
}

void MalList::insert(MalList::iterator position, MalList::iterator first, MalList::iterator last) {
    list.insert(position, first, last);
}

std::shared_ptr<MalList> MalList::cloneAsList() const {
    return std::shared_ptr<MalList>(new MalList(list, true));
}

std::shared_ptr<MalList> MalList::cloneAsVector() const {
    return std::shared_ptr<MalList>(new MalList(list, false));
}

void MalList::pop_back() {
    list.pop_back();
}

/**
 *
 * MalHashMap
 *
 */

bool MalHashMap::EQPredicate::operator()(const MalObjectPtr &a, const MalObjectPtr &b) const {
    return *a == *b;
}

unsigned int MalHashMap::Hash::operator()(const MalObjectPtr &k) const {
    std::hash<std::string> hash_fn;
    return hash_fn(k->toString());
}

MalObjectPtr& MalHashMap::operator[] (const MalObjectPtr& k) {
    return hash_map[k];
}

std::string MalHashMap::toString(const bool readable) const {
    std::string map_str;
    if (not hash_map.empty()) {
        map_str = std::accumulate(std::next(hash_map.begin()), hash_map.end(), pairToString(*hash_map.begin(), readable),
                                  [&readable](std::string &str, auto pair) { return str + " " + pairToString(pair, readable); });
    }


    return "{" + map_str + "}";
}

std::string MalHashMap::pairToString(const std::pair<MalObjectPtr, MalObjectPtr> &pair, const bool readable) {
    return pair.first->toString(readable) + " " + pair.second->toString(readable);
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
    auto &other_map = static_cast<const MalHashMap &>(other);
    if (hash_map.size() != other_map.hash_map.size()) return false;

    return std::equal(begin(), end(), other_map.begin(),
                      [](auto pair1, auto pair2) {
                          return (*pair1.first == *pair2.first) and (*pair1.second == *pair2.second);
                      });
}

void MalHashMap::erase(const MalObjectPtr &key) {
    hash_map.erase(key);
}

const MalObjectPtr &MalHashMap::at(const MalObjectPtr &key) const {
    return hash_map.at(key);
}

bool MalHashMap::contains(const MalObjectPtr &key) const {
    return hash_map.find(key) != end();
}

/**
 *
 * MalFuncBase
 *
 */

std::string MalFuncBase::toString(const bool readable) const {
    return "#<function>";
}

MalObject::Type MalFuncBase::getType() const {
    return Type::FUNCTION;
}

bool MalFuncBase::isCoreFunc() const {
    return true;
}

/**
 *
 * MalCoreFunc
 *
 */

MalCoreFunc::MalCoreFunc(const MalCoreFunc::FuncType &func) : func(func) {}

MalObjectPtr MalCoreFunc::operator()(const MalListPtr &vec) const {
    return func(vec);
}

bool MalCoreFunc::isCoreFunc() const {
    return true;
}

/**
 *
 * MalFunc
 *
 */

bool MalFunc::isCoreFunc() const {
    return false;
}

MalFunc::MalFunc(const MalObjectPtr &ast, const MalListPtr &params,
                 const std::shared_ptr<Environment> &env, const MalCoreFuncPtr &fn)
        : ast(ast),
          params(params),
          env(env), fn(fn) {}

MalObjectPtr MalFunc::operator()(const MalListPtr &vec) const {
    return (*fn)(vec);
}

/**
 *
 * MalException
 *
 */

MalException::MalException(const MalObjectPtr &object) : object(object) {}

MalObject::Type MalException::getType() const {
    return MalObject::Type::EXCEPTION;
}

std::string MalException::toString(bool readable) const {
    return "(Exception " + object->toString(readable) + ")";
}

