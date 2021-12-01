//
// Created by tomfe on 22/11/2021.
//

#ifndef MAL_ERROR_H
#define MAL_ERROR_H

#include <exception>
#include <string>

class Error : public std::exception {
private:
    const std::string err_msg;

public:
    const char *what() const noexcept override;

    explicit Error(const std::string &err_msg);
};

class EOFError : public Error {
public:
    explicit EOFError();
};


#endif //MAL_ERROR_H
