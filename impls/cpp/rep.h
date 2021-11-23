//
// Created by tomfe on 23/11/2021.
//

#ifndef MAL_REP_H
#define MAL_REP_H

#include "types.h"

using Func = std::function<std::shared_ptr<MalType>(const std::shared_ptr<MalList> &)>;
using EnvValue = MalFunc<Func>;
using Env = std::map<std::string, std::shared_ptr<EnvValue>>;

std::shared_ptr<MalType> READ(const std::string &input);

std::shared_ptr<MalType> EVAL(const std::shared_ptr<MalType> &ast, const Env &env);

std::string PRINT(const std::shared_ptr<MalType> &ast);

std::string rep(const std::string &input);

#endif //MAL_REP_H
