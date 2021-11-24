//
// Created by tomfe on 24/11/2021.
//

#ifndef MAL_ENVIRONMENT_H
#define MAL_ENVIRONMENT_H


#include <memory>
#include <map>
#include <functional>
#include "types.h"

class Environment {
private:
    Environment *outer;
    std::map<std::string, std::shared_ptr<MalType>> env;

public:
    using value_type = decltype(env)::value_type;

    using iterator = decltype(env)::iterator;

    explicit Environment(Environment *outer = nullptr);

    void insert(const value_type &pair);

    const std::shared_ptr<MalType> &at(const std::string &key) const;

    void setOuter(Environment *_outer);
};

#endif //MAL_ENVIRONMENT_H
