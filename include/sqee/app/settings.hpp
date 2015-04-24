#pragma once
#include <sqee/forward.hpp>

namespace sq {

class SettingMap {
public:
    template <class T>
    void add(const string& _key, const T& _val);
    template <class T>
    void mod(const string& _key, const T& _val);

    template <class T>
    T crnt(const string& _key);
    template <class T>
    T next(const string& _key);

    void apply();
    void revert();

    bool check_update(const string& _object);

private:
    unordered_map<string, pair<int, int>> iMap;
    unordered_map<string, pair<bool, bool>> bMap;
    unordered_map<string, pair<float, float>> fMap;
    unordered_map<string, pair<string, string>> sMap;
    unordered_set<string> updateSet;
};

}
