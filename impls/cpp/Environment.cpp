//
// Created by tomfe on 24/11/2021.
//

#include <cassert>
#include "Environment.h"
#include "Error.h"

Environment::Environment(Environment *outer) : outer(outer) {}

void Environment::insert(const Environment::value_type &pair) {
    env[pair.first] = pair.second;
}

const std::shared_ptr<MalType> &Environment::at(const std::string &key) const {
    try {
        return env.at(key);
    } catch (const std::out_of_range &) {
        if (outer) {
            return outer->at(key);
        }

        throw Error(key + " not found");
    }
}

void Environment::setOuter(Environment *_outer) {
    outer = _outer;
}
