#pragma once
#include "forward.hpp"

#include <unordered_map>

namespace sq {

template <class T>
class ResHolder {
public:
    using iterator = typename std::unordered_map<string, T>::iterator;
    iterator begin() { return theMap.begin(); }
    iterator end()   { return theMap.end(); }

    template<typename... TvArgs>
    T* add(const string& _key, TvArgs&&... _args) {
        if (has(_key)) theMap.erase(_key);
        theMap[_key] = unique_ptr<T>(new T(_args...));
        return theMap.at(_key).get();
    }

    T* get(const string& _key) {
        if (has(_key)) return theMap.at(_key).get();
        else return nullptr;
    }

    bool has(const string& _key) {
        return theMap.count(_key);
    }

    void del(const string& _key) {
        theMap.erase(_key);
    }

protected:
    std::unordered_map<string, unique_ptr<T>> theMap;
};

}
