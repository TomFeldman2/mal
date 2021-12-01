//
// Created by tomfe on 22/11/2021.
//

#include "../include/Error.h"

const char *Error::what() const noexcept {
    return err_msg.c_str();
}

Error::Error(const std::string &err_msg) : err_msg(err_msg) {}

EOFError::EOFError() : Error("EOF") {}
