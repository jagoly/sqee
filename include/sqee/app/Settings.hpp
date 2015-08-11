#pragma once
#include <sqee/forward.hpp>

namespace sq {

class Settings {
public:
    template <class T> void add(const string& _key, const T& _val);
    template <class T> void mod(const string& _key, const T& _val);
    template <class T> T get(const string& _key) const;
    template <class T> bool check(const string& _key) const;
    void clear_changed();

protected:
    unordered_map<string, int> intMap;
    unordered_map<string, bool> boolMap;
    unordered_map<string, float> floatMap;
    unordered_map<string, string> stringMap;
    unordered_set<string> intChanges;
    unordered_set<string> boolChanges;
    unordered_set<string> floatChanges;
    unordered_set<string> stringChanges;
};

}
