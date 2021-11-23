//
// Created by tomfe on 22/11/2021.
//

#include <cassert>
#include <algorithm>
#include <utility>
#include "Reader.h"
#include "Lexer.h"
#include "Error.h"

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

MalType* readStr(const std::string &input) {
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

MalType* readFrom(Reader &reader) {

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
        return new MalList("quote", readFrom(reader));

    if (reader.match("`"))
        return new MalList("quasiquote", readFrom(reader));

    if (reader.match("~"))
        return new MalList("unquote", readFrom(reader));

    if (reader.match("@"))
        return new MalList("deref", readFrom(reader));

    if (reader.match("~@"))
        return new MalList("splice-unquote", readFrom(reader));

    if (reader.match("^")) {
        auto second = readFrom(reader);
        auto first = readFrom(reader);
        auto list = new MalList("with-meta", first);
        list->append(second);
        return list;
    }


    return readAtom(reader);
}

MalList* readList(Reader &reader, const  bool is_list) {
    auto list = new MalList(is_list);

    const auto closing_paren = is_list ? ")" : "]";
    while (not reader.match(closing_paren)) {
        try {
            list->append(readFrom(reader));
        } catch (const Reader::Comment) {
            throw Error(std::string("Encountered comment inside ") + (is_list ? "list" : "vector"));
        }
    }
    return list;

}

MalHashMap *readMap(Reader &reader) {
    auto map = new MalHashMap();

    while (not reader.match("}")) {
        try {
            const auto key = readFrom(reader);
            const auto value = readFrom(reader);
            map->insert(key, value);
        } catch (const Reader::Comment&) {
            throw Error("Encountered comment inside hashmap");
        }
    }
    return map;
}


MalAtom* readAtom(Reader &reader) {
    const auto &token = reader.peek();
    reader.next();

    return new MalAtom(token);
//    MalAtom* atom = nullptr;
//
//    if (std::all_of(token.begin(), token.end(), [](const auto c){ return std::isdigit(c); })) {
//        atom = new MalInt(token);
//    } else {
//        atom = new MalString(token);
//    }
//
//    reader.next();
//    return atom;
}
