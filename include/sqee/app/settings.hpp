#pragma once
#include <sqee/forward.hpp>

#include <map>
#include <set>

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
    std::map<const string, std::pair<int, int>> iMap;
    std::map<const string, std::pair<bool, bool>> bMap;
    std::map<const string, std::pair<float, float>> fMap;
    std::map<const string, std::pair<string, string>> sMap;

    std::set<string> updateSet;
};

}
