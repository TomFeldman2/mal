//
// Created by tomfe on 22/11/2021.
//

#ifndef MAL_TYPES_H
#define MAL_TYPES_H


#include <vector>
#include <string>
#include <string_view>
#include <map>

struct MalType {
    virtual std::string toString() const = 0;

    virtual ~MalType() = default;
};


class MalAtom : public MalType {
protected:
    const std::string_view value;

public:
    explicit MalAtom(const std::string_view &value);

    std::string toString() const override;

};
//
//class MalInt : public MalAtom {
//public:
//    explicit MalInt(const std::string_view &value);
//};
//
//class MalString : public MalAtom {
//public:
//    explicit MalString(const std::string_view &value);
//};

class MalList : public MalType {
private:
    std::vector<MalType *> list;
    const bool is_list;

public:
    explicit MalList(const bool isList);

    virtual ~MalList();

    std::string toString() const override;

    void append(MalType *type);


};

class MalHashMap : public MalType {
private:
    std::map<MalType*, MalType*> hash_map;

    std::string pairToString(const std::pair<MalType*, MalType*> &pair) const;

public:
    void insert(MalType* key, MalType* value);

    std::string toString() const override;

    ~MalHashMap();


};

class MalMacro : public MalType {
private:
    const std::string macro_name;
    MalType *type;

public:
    MalMacro(const std::string &macroName, MalType *type);

    std::string toString() const override;
};

#endif //MAL_TYPES_H
