//
// Created by tomfe on 23/11/2021.
//

#ifndef MAL_REP_H
#define MAL_REP_H

#include "types.h"
#include "Environment.h"

std::shared_ptr<MalType> READ(const std::string &input);

std::shared_ptr<MalType> EVAL(const std::shared_ptr<MalType> &ast, Environment &env);

std::string PRINT(const std::shared_ptr<MalType> &ast);

std::string rep(const std::string &input, Environment &env);

#endif //MAL_REP_H
