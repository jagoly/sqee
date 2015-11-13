#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

namespace sq {

/// Generic class for managing resources
template <class T>
class ResHolder final {
public:
    using iterator = typename std::unordered_map<string, unique_ptr<T>>::iterator;

    iterator begin() { return theMap.begin(); }
    iterator end()   { return theMap.end(); }

    template<class... Args>
    T* add(const string& _key, Args&&... _args) {
        if (has(_key)) throw std::runtime_error("");
        theMap[_key].reset(new T(_args...));
        return theMap.at(_key).get();
    }

    T* get(const string& _key) {
        iterator it = theMap.find(_key);
        return it != theMap.end() ? it->second.get() : nullptr;
    }

    bool has(const string& _key) const {
        return theMap.count(_key);
    }

    void erase(const string& _key) {
        theMap.erase(_key);
    }

private:
    std::unordered_map<string, unique_ptr<T>> theMap;
};

}
