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
#include <deque>

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
        ATOM,
        EXCEPTION,
    };

    virtual Type getType() const = 0;

    virtual std::string toString(bool readable = true) const = 0;

    virtual bool isTrue() const;

    virtual ~MalObject() = default;

    bool operator==(const MalObject &other) const;

    bool operator!=(const MalObject &other) const;

    friend std::ostream &operator<<(std::ostream &os, const MalObject &object);

private:
    static bool areSameTypes(const MalObject &first, const MalObject &second);

    virtual bool isListLike() const;

    virtual bool equals(const MalObject &other) const;

};

using MalObjectPtr = std::shared_ptr<MalObject>;

class MalAtom : public MalObject {
public:
    MalObjectPtr object;

    explicit MalAtom(const MalObjectPtr &object);

    Type getType() const override;

    std::string toString(bool readable) const override;
};

struct MalNil : public MalObject {
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

using MalNilPtr = std::shared_ptr<MalNil>;

class MalInt : public MalObject {
public:
    const int value;


    explicit MalInt(int value);

    std::string toString(bool readable) const override;

    Type getType() const override;

private:
    bool equals(const MalObject &other) const override;

};

struct MalString : public MalObject {

    const std::string value;

    explicit MalString(const std::string &value);

    explicit MalString(const std::string_view &value);

    std::string toString(bool readable) const override;

    Type getType() const override;

private:

    static std::string transformString(const std::string_view &str);

    bool equals(const MalObject &other) const override;
};

struct MalSymbol : public MalObject {

    const std::string value;

    explicit MalSymbol(const std::string &value);

    explicit MalSymbol(const std::string_view &value);

    std::string toString(bool readable) const override;

    Type getType() const override;

private:

    bool equals(const MalObject &other) const override;

};

struct MalKeyword : public MalSymbol {
    explicit MalKeyword(const std::string &value);

    explicit MalKeyword(const std::string_view &value);

    Type getType() const override;
};

struct MalBool : public MalObject {
    Type getType() const override;

    static std::shared_ptr<MalBool> getInstance(bool value);

private:

    bool equals(const MalObject &other) const override;
};

using MalBoolPtr = std::shared_ptr<MalBool>;

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
    std::deque<MalObjectPtr> list;

    bool equals(const MalObject &other) const override;

    virtual bool isListLike() const override;

    explicit MalList(const std::deque<MalObjectPtr> &list, bool as_list);

public:
    using value_type = typename decltype(list)::value_type;

    using iterator = typename decltype(list)::const_iterator;

    const bool is_list;

    explicit MalList(bool isList = true);

    MalList(const std::string &macro, const MalObjectPtr &object);

    iterator begin() const;

    iterator end() const;

    void insert(iterator position, iterator first, iterator last);

    std::string toString(bool readable) const override;

    void push_back(const MalObjectPtr &object);

    void push_front(const MalObjectPtr &object);

    void pop_back();

    const value_type &at(size_t n) const;

    size_t size() const;

    bool empty() const;

    Type getType() const override;

    std::string joinElements(bool include_first, bool readable = true, const std::string &delimiter = " ") const;

    std::shared_ptr<MalList> cloneAsList() const;

    std::shared_ptr<MalList> cloneAsVector() const;

};

using MalListPtr = std::shared_ptr<MalList>;

class MalHashMap : public MalObject {
private:
    struct EQPredicate {
        bool operator()(const MalObjectPtr &a, const MalObjectPtr &b) const;
    };

    struct Hash {
        unsigned int operator()(const MalObjectPtr &k) const;
    };

    std::unordered_map<MalObjectPtr, MalObjectPtr, Hash, EQPredicate> hash_map;

    static std::string pairToString(const std::pair<MalObjectPtr, MalObjectPtr> &pair, bool readable);

    bool equals(const MalObject &other) const override;

public:
    using value_type = typename decltype(hash_map)::value_type;

    using iterator = typename decltype(hash_map)::const_iterator;

    explicit MalHashMap() = default;

    MalObjectPtr& operator[] (const MalObjectPtr& k);

    iterator insert(iterator position, const value_type &val);

    std::string toString(bool readable) const override;

    iterator begin() const;

    iterator end() const;

    Type getType() const override;

    void erase(const MalObjectPtr &key);

    const MalObjectPtr &at(const MalObjectPtr &key) const;

    bool contains(const MalObjectPtr &key) const;
};

using MalHashMapPtr = std::shared_ptr<MalHashMap>;

class MalFuncBase : public MalObject {
public:

    std::string toString(bool readable) const override;

    Type getType() const override;

    virtual bool isCoreFunc() const = 0;

    virtual MalObjectPtr operator()(const MalListPtr &vec) const = 0;
};

class MalCoreFunc : public MalFuncBase {
public:
    using FuncType = std::function<MalObjectPtr(std::shared_ptr<MalList>)>;

    explicit MalCoreFunc(const FuncType &func);

    MalObjectPtr operator()(const MalListPtr &vec) const override;

    bool isCoreFunc() const;

private:
    const FuncType func;
};

using MalCoreFuncPtr = std::shared_ptr<MalCoreFunc>;

class Environment;

struct MalFunc : public MalFuncBase {
public:
    const MalObjectPtr ast;
    const MalListPtr params;
    const std::shared_ptr<Environment> env;

    bool is_macro = false;

    MalFunc(const MalObjectPtr &ast, const MalListPtr &params,
            const std::shared_ptr<Environment> &env, const MalCoreFuncPtr &fn);


    bool isCoreFunc() const override;

    MalObjectPtr operator()(const MalListPtr &vec) const override;

private:
    const std::shared_ptr<MalCoreFunc> fn;

};

using MalFuncPtr = std::shared_ptr<MalFunc>;

class MalException : public MalObject {
public:
    MalObjectPtr object;

    explicit MalException(const MalObjectPtr &object);

    Type getType() const override;

    std::string toString(bool readable) const override;
};

#endif //MAL_TYPES_H
