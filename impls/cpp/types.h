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

struct MalType {
    enum class Type {
        NIL,
        Iterable,
        Map,
        Integer,
        String,
        Symbol,
        Function,
    };

    virtual std::string toString() const = 0;

    virtual Type getType() const = 0;

    virtual ~MalType() = default;
};


struct MalAtom : public MalType {
};

class MalInt : public MalAtom {
public:
    const int value;


    explicit MalInt(int value);

    std::string toString() const override;

    Type getType() const override;

};

struct MalString : public MalAtom {

    const std::string_view value;

    explicit MalString(const std::string_view &value);

    std::string toString() const override;

    Type getType() const override;

};

struct MalSymbol : public MalAtom {

    const std::string value;

    explicit MalSymbol(const std::string &value);

    explicit MalSymbol(const std::string_view &value);

    std::string toString() const override;

    Type getType() const override;

};

class MalList : public MalType {
private:
    std::vector<std::shared_ptr<MalType>> list;

public:
    using value_type = typename decltype(list)::value_type;

    using iterator = typename decltype(list)::const_iterator;

    const bool is_list;

    explicit MalList(bool isList);

    MalList(const std::string &macro, const std::shared_ptr<MalType> &type);

    iterator begin() const;

    iterator end() const;

    std::string toString() const override;

    void push_back(const std::shared_ptr<MalType> &type);

    const value_type &at(size_t n) const;

    size_t size() const;

    bool empty() const;

    Type getType() const override;
};

class MalHashMap : public MalType {
private:
    std::map<std::shared_ptr<MalType>, std::shared_ptr<MalType>> hash_map;

    static std::string pairToString(const std::pair<std::shared_ptr<MalType>, std::shared_ptr<MalType>> &pair) ;

public:
    using value_type = typename decltype(hash_map)::value_type;

    using iterator = typename decltype(hash_map)::const_iterator;

    explicit MalHashMap() = default;

    void insert(const value_type &pair);

    iterator insert (iterator position, const value_type& val);

    std::string toString() const override;

    iterator begin() const;

    iterator end() const;

    Type getType() const override;
};

template<typename T>
class MalFunc;

template<typename R, typename ...Args>
class MalFunc<std::function<R(Args...)>> : public MalType {
private:
    std::function<R(Args...)> func;

public:
    explicit MalFunc(const std::function<R(Args...)> &func) : func(func) {}

    std::string toString() const override;

    Type getType() const override;

    R operator()(Args...);
};

template<typename R, typename... Args>
std::string MalFunc<std::function<R(Args...)>>::toString() const {
    return "MalFunc";
}

template<typename R, typename... Args>
MalType::Type MalFunc<std::function<R(Args...)>>::getType() const {
    return MalType::Type::Function;
}

template<typename R, typename... Args>
R MalFunc<std::function<R(Args...)>>::operator()(Args... args) {
    return func(args...);
}

#endif //MAL_TYPES_H
