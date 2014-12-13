#pragma once

#include <memory>
#include <map>
#include <unordered_map>
#include <list>
#include <string>

namespace sq {

template <class Tk, class Tv>
class ResHolder {
public:
    typedef typename std::unordered_map<Tk, Tv>::iterator iterator;

    iterator begin() { return theMap.begin(); }
    iterator end() { return theMap.end(); }

    Tv* add(const Tk& _key) {
        if (has(_key)) theMap.erase(_key);
        theMap[_key] = std::unique_ptr<Tv>(new Tv());
        return theMap.at(_key).get();
    }

    Tv* get(const Tk& _key) {
        return has(_key) ? theMap.at(_key).get() : nullptr;
    }

    bool has(const Tk& _key) {
        return theMap.count(_key);
    }

    void del(const Tk& _key) {
        theMap.erase(_key);
    }

protected:
    std::unordered_map<Tk, std::unique_ptr<Tv>> theMap;
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
        theMap.emplace({_key, theList.front()});
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
    std::map<Tk, Tv&> theMap;
};

template<class T> class SettingMap {
public:
    void add_setting(const std::string& _key, const T& _val) {
        theMap[_key] = {_val, _val};
    }

    void modify(const std::string& _key, const T& _val) {
        theMap.at(_key).second = _val;
    }

    T crnt(const std::string& _key) { return theMap.at(_key).first; }
    T next(const std::string& _key) { return theMap.at(_key).second; }

    void apply() {
        for (std::pair<const std::string, std::pair<T, T>>& item : theMap)
            item.second.first = item.second.second;
    }

    void revert() {
        for (std::pair<const std::string, std::pair<T, T>>& item : theMap)
            item.second.second = item.second.first;
    }

protected:
    std::map<const std::string, std::pair<T, T>> theMap;
};

}
