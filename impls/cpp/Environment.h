//
// Created by tomfe on 24/11/2021.
//

#ifndef MAL_ENVIRONMENT_H
#define MAL_ENVIRONMENT_H


#include <memory>
#include <map>
#include <functional>
#include <ostream>
#include "types.h"

class Environment {
private:
    std::shared_ptr<Environment> outer;
    std::map<std::string, std::shared_ptr<MalObject>> env;

public:
    using value_type = decltype(env)::value_type;

    using iterator = decltype(env)::iterator;

    explicit Environment(const std::shared_ptr<Environment> &outer = nullptr);

    Environment(const std::shared_ptr<Environment> &outer, const std::shared_ptr<MalList> &binds, const std::shared_ptr<MalList> &exprs);

    void insert(const value_type &pair);

    const std::shared_ptr<MalObject> &at(const std::string &key) const;

    friend std::ostream &operator<<(std::ostream &os, const Environment &environment);
};

#endif //MAL_ENVIRONMENT_H
