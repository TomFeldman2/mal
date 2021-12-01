//
// Created by tomfe on 24/11/2021.
//

#include <cassert>
#include "../include/Environment.h"
#include "../include/Error.h"

Environment::Environment(const std::shared_ptr<Environment> &outer) : outer(outer) {}

Environment::Environment(const std::shared_ptr<Environment> &outer, const std::shared_ptr<MalList> &binds,
                         const std::shared_ptr<MalList> &exprs) : outer(outer) {

    size_t i;
    for (i = 0; i < binds->size(); ++i) {
        auto symbol = std::dynamic_pointer_cast<MalSymbol>(binds->at(i));
        if (symbol->value == "&") break;

        assert(i + 1 < exprs->size());
        insert({symbol->value, exprs->at(i + 1)});
    }

    if (i != binds->size()) {
        assert(i + 2 == binds->size());
        auto symbol = std::dynamic_pointer_cast<MalSymbol>(binds->at(++i));

        auto list = std::make_shared<MalList>(true);
        for (; i < exprs->size(); ++i) {
            list->push_back(exprs->at(i));
        }
        insert({symbol->value, list});
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
