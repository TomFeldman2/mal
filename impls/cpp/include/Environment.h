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

class Environment;

using EnvironmentPtr = std::shared_ptr<Environment>;

class Environment {
private:
    EnvironmentPtr outer;
    std::map<std::string, MalObjectPtr> env;

public:
    using value_type = decltype(env)::value_type;

    using iterator = decltype(env)::iterator;

    explicit Environment(const EnvironmentPtr &outer = nullptr);

    Environment(const EnvironmentPtr &outer, const MalListPtr &binds, const MalListPtr &exprs);

    void insert(const value_type &pair);

    const MalObjectPtr &at(const std::string &key) const;

    friend std::ostream &operator<<(std::ostream &os, const Environment &environment);
};

#endif //MAL_ENVIRONMENT_H
