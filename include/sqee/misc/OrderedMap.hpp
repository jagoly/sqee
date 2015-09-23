#pragma once
#include <sqee/forward.hpp>

namespace sq {

/// Generic class for map with front and back
template <class TK, class TV>
class OrderedMap final {
public:
    using iterator = typename deque<TV>::iterator;
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
        const auto ptr  = theMap.at(_key);
        const auto func = [ptr](TV& dptr) { return &dptr == ptr; };
        const auto iter = std::find_if(begin(), end(), func);
        theDeque.erase(iter); theMap.erase(_key);
    }

private:
    deque<TV> theDeque;
    unordered_map<TK, TV*> theMap;
};

namespace res {
string& path();
}

}
