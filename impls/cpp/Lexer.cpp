//
// Created by tomfe on 21/11/2021.
//

#include <cassert>
#include <iostream>
#include "Lexer.h"
#include "Error.h"

const std::string Lexer::special_chars = "[]{}()'`~\",;";

Lexer::Lexer(const std::string &input) : input(input) {
    current = input[0];
}

bool Lexer::hasChar() {
    return current != eol;
}


void Lexer::consume() {
    ++pos;
    if(pos == input.size()) current = eol;
    else current = input[pos];
}

std::string_view Lexer::next() {

    while(hasChar()) {
        const auto starting_pos = pos;
        switch (current) {
            case ' ':
            case '\t':
            case '\r':
            case '\f':
            case '\n':
            case ',':
                consume();
                continue;
            case '~': {
                consume();
                if (hasChar() and current == '@') {
                    consume();
                }
                break;
            }
            case '[':
            case ']':
            case '{':
            case '}':
            case '(':
            case ')':
            case '\'':
            case '`':
            case '^':
            case '@':
                consume();
                break;
            case '"':
                stringLiteral();
                break;
            case ';':
                comment();
                break;
            default:
                symbol();
        }
        return substr(starting_pos);
    }

    return substr(pos);
}



void Lexer::stringLiteral() {
    bool last_is_escape = false;

    consume();
    while(hasChar()) {
        if (current == '"' and not last_is_escape) {
            break;
        }
        if (current == '\\' and not last_is_escape) {
            last_is_escape = true;
        } else {
            last_is_escape = false;
        }

        consume();
    }

    if (current != '"') throw Error("unbalanced closing quote");
    consume();
}

std::string_view Lexer::substr(const size_t start) const {
    return input.substr(start, pos - start);
}

void Lexer::comment() {
    consume();
    while(hasChar() and current != '\n') {
        consume();
    }

}

void Lexer::symbol() {
    size_t cnt = 0;
    while(hasChar() and not (isSpecialChar() or std::isspace(current))) {
        ++cnt;
        consume();
    }
}

bool Lexer::isSpecialChar() const {
    return special_chars.find(current) != std::string::npos;
}

