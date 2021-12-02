//
// Created by tomfe on 23/11/2021.
//

#ifndef MAL_REP_H
#define MAL_REP_H

#include "types.h"
#include "Environment.h"

std::string rep(const std::string &input, const EnvironmentPtr &env);

MalObjectPtr EVAL(MalObjectPtr ast, EnvironmentPtr env);

#endif //MAL_REP_H
