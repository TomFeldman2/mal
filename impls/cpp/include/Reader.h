//
// Created by tomfe on 22/11/2021.
//

#ifndef MAL_READER_H
#define MAL_READER_H


#include <string>
#include <string_view>
#include <vector>
#include <memory>
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

std::vector<std::string_view> tokenize(const std::string &input);

MalObjectPtr readStr(const std::string &input);

MalObjectPtr readFrom(Reader &reader);

MalListPtr readList(Reader &reader, bool is_list);

MalHashMapPtr readMap(Reader &reader);

MalObjectPtr readAtom(Reader &reader);


#endif //MAL_READER_H
