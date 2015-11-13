#pragma once

#include <list>
#include <stdexcept>
#include <unordered_map>

#include <sqee/setup.hpp>

namespace sq {

/// Generic class for map with front and back
template <class TK, class TV>
class OrderedMap final {
public:
    using iterator = typename std::list<TV>::iterator;

    iterator begin() { return theList.begin(); }
    iterator end()   { return theList.end(); }

    template<class... Args>
    void prepend(const TK& _key, Args&&... _args) {
        if (has(_key)) throw std::out_of_range("");
        theList.emplace_front(_args...);
        theMap.emplace(_key, &theList.front());
    }

    template<class... Args>
    void append(const TK& _key, Args&&... _args) {
        if (has(_key)) throw std::out_of_range("");
        theList.emplace_back(_args...);
        theMap.emplace(_key, &theList.back());
    }

    void remove(const TK& _key) {
        const TV* ptr = theMap.at(_key);
        iterator iter = theList.begin();
        while (&*iter != ptr && ++iter != end()){}
        theList.erase(iter); theMap.erase(_key);
    }

    template<class... Args>
    void insert_before(const TK& _pos, const TK& _key, Args&&... _args) {
        const TV* ptr = theMap.at(_pos);
        iterator iter = theList.begin();
        while (&*iter != ptr && ++iter != end()){}
        theList.emplace(iter, _args...);
        theMap.emplace(_key, &*(--iter));
    }

    template<class... Args>
    void insert_after(const TK& _pos, const TK& _key, Args&&... _args) {
        const TV* ptr = theMap.at(_pos);
        iterator iter = theList.begin();
        while (&*iter != ptr && ++iter != end()){}
        theList.emplace(++iter, _args...);
        theMap.emplace(_key, &*(--iter));
    }

    bool has(const TK& _key) const { return theMap.count(_key); }

    TV& get(const TK& _key) { return *theMap.at(_key); }
    const TV& get(const TK& _key) const { return *theMap.at(_key); }

    TV& front() { return theList.front(); }
    const TV& front() const { return theList.front(); }
    TV& back() { return theList.back(); }
    const TV& back() const { return theList.back(); }

private:
    std::unordered_map<TK, TV*> theMap;
    std::list<TV> theList;
};

}
