//
// Created by tomfe on 24/11/2021.
//

#include <cassert>
#include "../include/Environment.h"
#include "../include/Error.h"

Environment::Environment(const EnvironmentPtr &outer) : outer(outer) {}

Environment::Environment(const EnvironmentPtr &outer, const MalListPtr &binds,
                         const std::shared_ptr<MalList> &exprs) : outer(outer) {

    size_t i = 0;
    for (; i < binds->size(); ++i) {
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

const MalObjectPtr &Environment::at(const std::string &key) const {
    try {
        return env.at(key);
    } catch (const std::out_of_range &) {
        if (outer) {
            return outer->at(key);
        }

        throw MalException(std::make_shared<MalString>("'" + key + "'" + " not found"));
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
