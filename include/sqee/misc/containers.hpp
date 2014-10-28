#pragma once

#include <memory>
#include <map>
#include <list>

namespace sq {

template <class Tk, class Tv>
class ResHolder {
public:
    typedef typename std::map<Tk, Tv>::iterator iterator;
    typedef std::pair<const Tk, Tv> Pair;

    iterator begin() { return map.begin(); }
    iterator end() { return map.end(); }

    Tv* add(const Tk& _key) {
        if (has(_key)) map.erase(_key);
        map[_key] = std::unique_ptr<Tv>(new Tv());
        return map.at(_key).get();
    }

    Tv* get(const Tk& _key) {
        if (has(_key))
            return map.at(_key).get();
        return nullptr;
    }

    bool has(const Tk& _key) {
        return map.count(_key);
    }

    void del(const Tk& _key) {
        map.erase(_key);
    }

protected:
    std::map<Tk, std::unique_ptr<Tv>> map;
};

template <class Tk, class Tv>
class IndexedMap {
public:
    typedef typename std::list<Tv>::iterator iterator;
    iterator begin() { return list.begin(); }
    iterator end() { return list.end(); }
    typedef typename std::list<Tv>::reverse_iterator reverse_iterator;
    reverse_iterator rbegin() { return list.rbegin(); }
    reverse_iterator rend() { return list.rend(); }

    template<typename... TvArgs> Tv& insert(uint _ind, const Tk& _key, TvArgs&&... _args) {
        iterator iter = list.before_begin();
        for (uint i = 0; i < _ind; i++) iter++;
        iter = list.emplace_after(iter, _args...);
        map.emplace({_key, *iter});
        return *iter;
    }

    template<typename... TvArgs> Tv& prepend(const Tk& _key, TvArgs&&... _args) {
        list.emplace_front(_args...);
        map.emplace({_key, list.front()});
        return list.front();
    }

    template<typename... TvArgs> Tv& append(const Tk& _key, TvArgs&&... _args) {
        list.emplace_back(_args...);
        map.emplace(_key, list.back());
        return list.back();
    }

    void del(const Tk& _key) {
        Tv& val = map.at(_key);
        list.remove(val);
        map.erase(_key);
    }

    Tv& get(const Tk& _key) {
        return map.at(_key);
    }

protected:
    std::list<Tv> list;
    std::map<Tk, Tv&> map;
};

}
