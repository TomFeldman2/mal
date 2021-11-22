//
// Created by tomfe on 21/11/2021.
//

#ifndef MAL_LEXER_H
#define MAL_LEXER_H


#include <string>

class Lexer {
    static const std::string special_chars;

    const std::string_view input;

    size_t pos = 0;

    char current;

    static const int eol = -1;

    inline bool hasChar();

    inline void consume();

    inline bool isSpecialChar() const;

    void stringLiteral();

    void comment();

    void symbol();

    std::string_view substr(size_t start) const;

public:
    std::string_view next();

    explicit Lexer(const std::string &input);

};


#endif //MAL_LEXER_H
