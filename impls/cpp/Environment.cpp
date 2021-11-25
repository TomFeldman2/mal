//
// Created by tomfe on 24/11/2021.
//

#include <cassert>
#include "Environment.h"
#include "Error.h"

Environment::Environment(const std::shared_ptr<Environment> &outer) : outer(outer) {}

Environment::Environment(const std::shared_ptr<Environment> &outer, const std::shared_ptr<MalList> &binds,
                         const std::shared_ptr<MalList> &exprs) : outer(outer) {
    assert(binds->size() == exprs->size() - 1);
    for (size_t i = 0; i < binds->size(); ++i) {
        auto symbol = std::dynamic_pointer_cast<MalSymbol>(binds->at(i));
        insert({symbol->value, exprs->at(i + 1)});
    }
}

void Environment::insert(const Environment::value_type &pair) {
    env[pair.first] = pair.second;
}

const std::shared_ptr<MalObject> &Environment::at(const std::string &key) const {
    try {
        return env.at(key);
    } catch (const std::out_of_range &) {
        if (outer) {
            return outer->at(key);
        }

        throw Error(key + " not found");
    }
}

std::ostream &operator<<(std::ostream &os, const Environment &environment) {
    for (const auto &[key, value]: environment.env) {
        os << key << " " << *value << "\n";
        std::flush(os);
    }
    if (environment.outer) {
        os << *environment.outer;
    }
    return os;
}
