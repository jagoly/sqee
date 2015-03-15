#pragma once
#include "forward.hpp"

#include <list>
#include <unordered_map>

namespace sq {

template <class Tk, class Tv>
class IndexedMap {
public:
    using iterator         = typename std::list<Tv>::iterator;
    using reverse_iterator = typename std::list<Tv>::reverse_iterator;
    iterator begin()          { return theList.begin();  }
    iterator end()            { return theList.end();    }
    reverse_iterator rbegin() { return theList.rbegin(); }
    reverse_iterator rend()   { return theList.rend();   }

    template<typename... TvArgs>
    Tv& insert(uint _ind, const Tk& _key, TvArgs&&... _args) {
        auto iter = theList.before_begin();
        std::advance(iter, _ind);
        iter = theList.emplace_after(iter, _args...);
        theMap.emplace({_key, *iter});
        return *iter;
    }

    template<typename... TvArgs>
    Tv& prepend(const Tk& _key, TvArgs&&... _args) {
        theList.emplace_front(_args...);
        theMap.emplace(_key, theList.front());
        return theList.front();
    }

    template<typename... TvArgs>
    Tv& append(const Tk& _key, TvArgs&&... _args) {
        theList.emplace_back(_args...);
        theMap.emplace(_key, theList.back());
        return theList.back();
    }

    Tv& get(const Tk& _key) {
        return theMap.at(_key);
    }

    void del(const Tk& _key) {
        Tv& val = theMap.at(_key);
        theList.remove(val);
        theMap.erase(_key);
    }

protected:
    std::list<Tv> theList;
    std::unordered_map<Tk, Tv&> theMap;
};

}
