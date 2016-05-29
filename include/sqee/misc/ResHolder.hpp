#pragma once

#include <unordered_set>
#include <unordered_map>

#include <sqee/builtins.hpp>

namespace sq {

/// Generic class for managing resources
template <class T> class ResHolder final {
private:
    struct Resource {
        std::unordered_set<string> userSet;
        unique_ptr<T> resourcePtr;
    };

    std::unordered_map<string, Resource> resMap;

public:
    using iterator = typename decltype(resMap)::iterator;

    iterator begin() { return resMap.begin(); }
    iterator end()   { return resMap.end(); }

    template<class... Args>
    T* add(const string& _key, Args&&... _args) {
        if (has(_key)) throw std::runtime_error("");
        resMap[_key].resourcePtr.reset(new T(_args...));
        return resMap.at(_key).resourcePtr.get();
    }

    T* get(const string& _key) {
        iterator iter = resMap.find(_key);
        if (iter == resMap.end()) return nullptr;
        return iter->second.resourcePtr.get();
    }

    void add_user(const string& _key, const string& _user) {
        resMap.at(_key).userSet.emplace(_user);
    }

    void remove_user(const string& _key, const string& _user) {
        resMap.at(_key).userSet.erase(_user);
    }

    bool has(const string& _key) const {
        return resMap.count(_key);
    }
};

}
