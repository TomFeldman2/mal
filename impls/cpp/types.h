//
// Created by tomfe on 22/11/2021.
//

#ifndef MAL_TYPES_H
#define MAL_TYPES_H


#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <memory>
#include <ostream>

struct MalObject {
    enum class Type {
        NIL,
        BOOLEAN,
        LIST,
        VECTOR,
        MAP,
        INTEGER,
        STRING,
        SYMBOL,
        KEYWORD,
        FUNCTION,
    };


    virtual Type getType() const = 0;

    virtual bool isTrue() const;

    virtual ~MalObject() = default;

    bool operator==(const MalObject &other) const;

    friend std::ostream &operator<<(std::ostream &os, const MalObject &object);

    virtual std::string toString(bool readable = true) const = 0;

private:
    static bool areSameTypes(const MalObject &first, const MalObject &second);

    virtual bool isListLike() const;

    virtual bool equals(const MalObject &other) const = 0;

};


struct MalAtom : public MalObject {
};

struct MalNil : public MalAtom {
public:
    MalNil(const MalNil &) = delete;

    void operator=(const MalNil &) = delete;

    static const std::shared_ptr<MalNil> &getInstance();

    std::string toString(bool readable) const override;

    Type getType() const override;

    bool isTrue() const override;

private:
    static std::shared_ptr<MalNil> nil;

    explicit MalNil() = default;

    bool equals(const MalObject &other) const override;

};


class MalInt : public MalAtom {
public:
    const int value;


    explicit MalInt(int value);

    std::string toString(bool readable) const override;

    Type getType() const override;

private:
    bool equals(const MalObject &other) const override;

};

struct MalString : public MalAtom {

    const std::string value;

    explicit MalString(const std::string &value);

    explicit MalString(const std::string_view &value);

    std::string toString(bool readable) const override;

    Type getType() const override;

private:

    static std::string transformString(const std::string_view &str);

    bool equals(const MalObject &other) const override;
};

struct MalSymbol : public MalAtom {

    const std::string value;

    explicit MalSymbol(const std::string &value);

    explicit MalSymbol(const std::string_view &value);

    std::string toString(bool readable) const override;

    Type getType() const override;

private:

    bool equals(const MalObject &other) const override;

};

struct MalKeyword : public MalAtom {

    const std::string_view value;

    explicit MalKeyword(const std::string_view &value);

    std::string toString(bool readable) const override;

    Type getType() const override;

private:

    bool equals(const MalObject &other) const override;

};

struct MalBool : public MalAtom {
    Type getType() const override;

    static std::shared_ptr<MalBool> getInstance(bool value);

private:

    bool equals(const MalObject &other) const override;
};

struct MalTrue : public MalBool {
public:
    MalTrue(const MalTrue &) = delete;

    void operator=(const MalTrue &) = delete;

    static const std::shared_ptr<MalTrue> &getInstance();

    std::string toString(bool readable) const override;

private:
    explicit MalTrue() = default;

    static std::shared_ptr<MalTrue> true_instance;
};


struct MalFalse : public MalBool {
public:
    MalFalse(const MalFalse &) = delete;

    void operator=(const MalFalse &) = delete;

    static const std::shared_ptr<MalFalse> &getInstance();

    std::string toString(bool readable) const override;

    bool isTrue() const;

private:
    explicit MalFalse() = default;

    static std::shared_ptr<MalFalse> false_instance;
};

class MalList : public MalObject {
private:
    std::vector<std::shared_ptr<MalObject>> list;

    bool equals(const MalObject &other) const override;

    virtual bool isListLike() const override;

public:
    using value_type = typename decltype(list)::value_type;

    using iterator = typename decltype(list)::const_iterator;

    const bool is_list;

    explicit MalList(bool isList);

    MalList(const std::string &macro, const std::shared_ptr<MalObject> &type);

    iterator begin() const;

    iterator end() const;

    std::string toString(bool readable) const override;

    void push_back(const std::shared_ptr<MalObject> &type);

    const value_type &at(size_t n) const;

    size_t size() const;

    bool empty() const;

    Type getType() const override;

    std::string joinElements(bool include_first, bool readable = true, const std::string &delimiter = " ") const;

};

class MalHashMap : public MalObject {
private:
    std::map<std::shared_ptr<MalObject>, std::shared_ptr<MalObject>> hash_map;

    static std::string pairToString(const std::pair<std::shared_ptr<MalObject>, std::shared_ptr<MalObject>> &pair);

    bool equals(const MalObject &other) const override;


public:
    using value_type = typename decltype(hash_map)::value_type;

    using iterator = typename decltype(hash_map)::const_iterator;

    explicit MalHashMap() = default;

    void insert(const value_type &pair);

    iterator insert(iterator position, const value_type &val);

    std::string toString(bool readable) const override;

    iterator begin() const;

    iterator end() const;

    Type getType() const override;

};

class MalFuncBase : public MalObject {
public:

    std::string toString(bool readable) const override;

    Type getType() const override;

    virtual bool isCoreFunc() const = 0;

private:
    bool equals(const MalObject &other) const override;

};

class MalCoreFunc : public MalFuncBase {
public:
    using FuncType = std::function<std::shared_ptr<MalObject>(std::shared_ptr<MalList>)>;

    explicit MalCoreFunc(const FuncType &func);

    std::shared_ptr<MalObject> operator()(const std::shared_ptr<MalList> &vec) const;

    bool isCoreFunc() const;

private:
    const FuncType func;
};

class Environment;

struct MalFunc : public MalFuncBase {
public:
    const std::shared_ptr<MalObject> ast;
    const std::shared_ptr<MalList> params;
    const std::shared_ptr<Environment> env;
    const std::shared_ptr<MalCoreFunc> fn;

    MalFunc(const std::shared_ptr<MalObject> &ast, const std::shared_ptr<MalList> &params,
            const std::shared_ptr<Environment> &env, const std::shared_ptr<MalCoreFunc> &fn);


    bool isCoreFunc() const override;

};


#endif //MAL_TYPES_H
