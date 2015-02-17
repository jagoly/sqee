#include <cstring>

#include "app/settings.hpp"

using namespace sq;

SettingValue::SettingValue(const bool& _val) : bVal(_val) { valType = ValType::B; }
SettingValue::SettingValue(const int& _val) : iVal(_val) { valType = ValType::I; }
SettingValue::SettingValue(const float& _val) : fVal(_val) { valType = ValType::F; }
SettingValue::SettingValue(const char* _val) { std::strcpy(sVal, _val); valType = ValType::S; }
SettingValue::SettingValue(const string& _val) : SettingValue(_val.c_str()) {}

bool SettingValue::b() { if (valType == ValType::B) return bVal; else throw; }
int SettingValue::i() { if (valType == ValType::I) return iVal; else throw; }
float SettingValue::f() { if (valType == ValType::F) return fVal; else throw; }
string SettingValue::s() { if (valType == ValType::S) return sVal; else throw; }


void SettingMap::add(const string& _key, const SettingValue& _val) {
    theMap.emplace(_key, std::pair<SettingValue, SettingValue>(_val, _val));
}

void SettingMap::modify(const string& _key, const SettingValue& _val) {
    theMap.at(_key).second = _val;
}

SettingValue SettingMap::crnt(const string& _key) {
    return theMap.at(_key).first;
}

SettingValue SettingMap::next(const string& _key) {
    return theMap.at(_key).second;
}

void SettingMap::apply() {
    for (auto& item : theMap)
        item.second.first = item.second.second;
    updateSet.clear();
}

void SettingMap::revert() {
    for (auto& item : theMap)
        item.second.second = item.second.first;
}

bool SettingMap::check_update(const string& _object) {
    if (updateSet.count(_object)) return false;
    else { updateSet.emplace(_object); return true; }
}
