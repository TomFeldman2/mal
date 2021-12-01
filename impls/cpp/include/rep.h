//
// Created by tomfe on 23/11/2021.
//

#ifndef MAL_REP_H
#define MAL_REP_H

#include "types.h"
#include "Environment.h"

std::string rep(const std::string &input, const std::shared_ptr<Environment> &env);

std::shared_ptr<MalObject> EVAL(std::shared_ptr<MalObject> ast, std::shared_ptr<Environment> env);

#endif //MAL_REP_H
