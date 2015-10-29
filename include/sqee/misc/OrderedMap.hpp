#pragma once

#include <deque>
#include <stdexcept>
#include <unordered_map>

#include <sqee/setup.hpp>

namespace sq {

/// Generic class for map with front and back
template <class TK, class TV>
class OrderedMap final {
public:
    using iterator = typename std::deque<TV>::iterator;
    iterator begin() { return theDeque.begin(); }
    iterator end()   { return theDeque.end(); }

    template<typename... TvArgs>
    void append(const TK& _key, TvArgs&&... _args) {
        if (has(_key)) throw std::out_of_range("");
        theDeque.emplace_back(_args...);
        theMap.emplace(_key, &theDeque.back());
    }

    template<typename... TvArgs>
    void prepend(const TK& _key, TvArgs&&... _args) {
        if (has(_key)) throw std::out_of_range("");
        theDeque.emplace_front(_args...);
        theMap.emplace(_key, &theDeque.front());
    }

    bool has(const TK& _key) const {
        return theMap.count(_key);
    }

    TV& get(const TK& _key) {
        return *theMap.at(_key);
    }

    TV& front() {
        return theDeque.front();
    }

    TV& back() {
        return theDeque.back();
    }

    void remove(const TK& _key) {
        const TV* ptr = theMap.at(_key);
        iterator iter = theDeque.begin();
        for (; iter != end(); ++iter) if (&*iter == ptr) break;
        theDeque.erase(iter); theMap.erase(_key);
    }

private:
    std::unordered_map<TK, TV*> theMap;
    std::deque<TV> theDeque;
};

}
