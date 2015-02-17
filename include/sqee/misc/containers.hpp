#pragma once
#include "forward.hpp"

#include <algorithm>
#include <list>
#include <memory>
#include <unordered_map>

namespace sq {

template <class T>
class ResHolder {
public:
    typedef typename std::unordered_map<string, T>::iterator iterator;

    iterator begin() { return theMap.begin(); }
    iterator end() { return theMap.end(); }

    T* add(const string& _key) {
        if (has(_key)) theMap.erase(_key);
        theMap[_key] = std::unique_ptr<T>(new T());
        return theMap.at(_key).get();
    }

    T* get(const string& _key) {
        return has(_key) ? theMap.at(_key).get() : nullptr;
    }

    bool has(const string& _key) {
        return theMap.count(_key);
    }

    void del(const string& _key) {
        theMap.erase(_key);
    }

protected:
    std::unordered_map<string, std::unique_ptr<T>> theMap;
};

template <class Tk, class Tv>
class IndexedMap {
public:
    typedef typename std::list<Tv>::iterator iterator;
    typedef typename std::list<Tv>::reverse_iterator reverse_iterator;
    iterator begin() { return theList.begin(); }
    iterator end() { return theList.end(); }
    reverse_iterator rbegin() { return theList.rbegin(); }
    reverse_iterator rend() { return theList.rend(); }

    template<typename... TvArgs> Tv& insert(uint _ind, const Tk& _key, TvArgs&&... _args) {
        iterator iter = theList.before_begin();
        for (uint i = 0; i < _ind; i++) iter++;
        iter = theList.emplace_after(iter, _args...);
        theMap.emplace({_key, *iter});
        return *iter;
    }

    template<typename... TvArgs> Tv& prepend(const Tk& _key, TvArgs&&... _args) {
        theList.emplace_front(_args...);
        theMap.emplace(_key, theList.front());
        return theList.front();
    }

    template<typename... TvArgs> Tv& append(const Tk& _key, TvArgs&&... _args) {
        theList.emplace_back(_args...);
        theMap.emplace(_key, theList.back());
        return theList.back();
    }

    void del(const Tk& _key) {
        Tv& val = theMap.at(_key);
        theList.remove(val);
        theMap.erase(_key);
    }

    Tv& get(const Tk& _key) {
        return theMap.at(_key);
    }

protected:
    std::list<Tv> theList;
    std::unordered_map<Tk, Tv&> theMap;
};

template <typename T>
bool val_in(const T& _val, const std::initializer_list<T>& _in) {
    return std::find(_in.begin(), _in.end(), _val) != _in.end();
}

}
