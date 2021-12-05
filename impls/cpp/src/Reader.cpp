//
// Created by tomfe on 22/11/2021.
//

#include <cassert>
#include <algorithm>
#include <iostream>
#include "../include/Reader.h"
#include "../include/Lexer.h"
#include "../include/Error.h"

Reader::Reader(const std::vector<token> &tokens) : tokens(tokens) {}

void Reader::next() {
    ++pos;
}

Reader::token Reader::peek() const{
    if (pos >= tokens.size()) throw EOFError();
    return tokens[pos];
}

bool Reader::match(const std::string &str) {
    const auto &token = peek();
    if (token == str) {
        next();
        return true;
    }
    return false;
}

bool Reader::matchComment() {
    const auto &token = peek();
    if (token[0] == ';') {
        next();
        return true;
    }
    return false;
}

/**
 *
 * utils
 *
 */

MalObjectPtr readStr(const std::string &input) {
    Reader reader{tokenize(input)};
    return readFrom(reader);
}


std::vector<std::string_view> tokenize(const std::string &input) {
    std::vector<std::string_view> tokens;
    std::string_view token;

    auto lexer = Lexer(input);
    token = lexer.next();
    while (not token.empty()) {
        tokens.push_back(token);
        token = lexer.next();
    }

    return tokens;
}

MalObjectPtr readFrom(Reader &reader) {

    if (reader.matchComment()) {
        throw Reader::Comment();
    }

    if (reader.match("("))
        return readList(reader, true);

    if (reader.match("["))
        return readList(reader, false);

    if (reader.match("{"))
        return readMap(reader);

    if (reader.match("{"))
        return readMap(reader);

    if (reader.match("'"))
        return std::make_shared<MalList>("quote", readFrom(reader));

    if (reader.match("`"))
        return std::make_shared<MalList>("quasiquote", readFrom(reader));

    if (reader.match("~"))
        return std::make_shared<MalList>("unquote", readFrom(reader));

    if (reader.match("@"))
        return std::make_shared<MalList>("deref", readFrom(reader));

    if (reader.match("~@"))
        return std::make_shared<MalList>("splice-unquote", readFrom(reader));

    if (reader.match("^")) {
        auto second = readFrom(reader);
        auto first = readFrom(reader);
        auto list = std::make_shared<MalList>("with-meta", first);
        list->push_back(second);
        return list;
    }


    return readAtom(reader);
}

MalListPtr readList(Reader &reader, const  bool is_list) {
    auto list = std::make_shared<MalList>(is_list);

    const auto closing_paren = is_list ? ")" : "]";
    while (not reader.match(closing_paren)) {
        try {
            list->push_back(readFrom(reader));
        } catch (const Reader::Comment) {}
    }
    return list;

}

MalHashMapPtr readMap(Reader &reader) {
    auto map = std::make_shared<MalHashMap>();

    while (not reader.match("}")) {
        try {
            const auto key = readFrom(reader);
            const auto value = readFrom(reader);
            (*map)[key] = value;
        } catch (const Reader::Comment&) {
            throw Error("Encountered comment inside hashmap");
        }
    }
    return map;
}


MalObjectPtr readAtom(Reader &reader) {
    const auto &token = reader.peek();
    reader.next();

    try {
        auto number = std::stoi(std::string(token));
        return std::make_shared<MalNumber>(number);
    } catch (const std::invalid_argument&) {}

    if (token[0] == '"' and token[token.size() - 1] == '"') {
        return std::make_shared<MalString>(token);
    }

    if (token[0] == ':') {
        return std::make_shared<MalKeyword>(token);
    }

    if (token == "nil"){
        return MalNil::getInstance();
    }

    const bool is_true = token == "true";
    if (is_true or token == "false") {
        return MalBool::getInstance(is_true);
    }

    return std::make_shared<MalSymbol>(token);
}
