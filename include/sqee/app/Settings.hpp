#pragma once
#include <sqee/forward.hpp>

namespace sq {

/// The SQEE Settings manager
class Settings final : NonCopyable {
public:
    template <class T> void add(const string& _key, const T& _val);
    template <class T> void mod(const string& _key, const T& _val);
    template <class T> bool check(const string& _key) const;
    template <class T> T get(const string& _key) const;
    void clear_changed();

private:
    std::unordered_map<string, int> intMap;
    std::unordered_map<string, bool> boolMap;
    std::unordered_map<string, float> floatMap;
    std::unordered_map<string, string> stringMap;
    std::unordered_set<string> intChanges;
    std::unordered_set<string> boolChanges;
    std::unordered_set<string> floatChanges;
    std::unordered_set<string> stringChanges;
};

}
