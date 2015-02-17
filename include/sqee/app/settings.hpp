#pragma once
#include "forward.hpp"

#include <map>
#include <set>

namespace sq {

class SettingValue {
public:
    SettingValue() = delete;
    SettingValue(const bool& _val);
    SettingValue(const int& _val);
    SettingValue(const float& _val);
    SettingValue(const string& _val);
    SettingValue(const char* _val);

    bool b(); int i(); float f(); string s();

private:
    union { bool bVal; int iVal; float fVal; char sVal[20]; };
    enum class ValType : uchar { B, I, F, S, UNSET };
    ValType valType = ValType::UNSET;
};


class SettingMap {
public:
    void add(const string& _key, const SettingValue& _val);
    void modify(const string& _key, const SettingValue& _val);

    SettingValue crnt(const string& _key);
    SettingValue next(const string& _key);

    void apply();
    void revert();

    bool check_update(const string& _object);

private:
    std::map<const string, std::pair<SettingValue, SettingValue>> theMap;
    std::set<string> updateSet;
};

}
