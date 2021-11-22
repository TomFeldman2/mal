//
// Created by tomfe on 22/11/2021.
//

#ifndef MAL_READER_H
#define MAL_READER_H


#include <string>
#include <string_view>
#include <vector>
#include "types.h"

class Reader {
private:
    using token = std::string_view;

    const std::vector<token> tokens;
    size_t pos = 0;


public:
    struct Comment {
    };

    explicit Reader(const std::vector<token> &tokens);

    void next();

    token peek() const;

    bool match(const std::string &str);

    bool matchComment();
};

MalType *readStr(const std::string &input);

std::vector<std::string_view> tokenize(const std::string &input);

MalType *readFrom(Reader &reader);

MalList *readList(Reader &reader, bool is_list);

MalHashMap *readMap(Reader &reader);

MalAtom *readAtom(Reader &reader);


#endif //MAL_READER_H
