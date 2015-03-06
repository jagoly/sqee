#include <cstring>

#include "app/settings.hpp"

using namespace sq;

#define SETTING_TEMPLATES(Type, Map) \
template<> void SettingMap::add<Type>(const string& _key, const Type& _val) { \
    Map.emplace(_key, std::pair<Type, Type>(_val, _val)); \
} \
template<> void SettingMap::mod<Type>(const string& _key, const Type& _val) { \
    Map.at(_key).second = _val; \
} \
template<> Type SettingMap::crnt<Type>(const string& _key) { \
    return Map.at(_key).first; \
} \
template<> Type SettingMap::next<Type>(const string& _key) { \
    return Map.at(_key).second; \
}
namespace sq {
SETTING_TEMPLATES(int, iMap)
SETTING_TEMPLATES(bool, bMap)
SETTING_TEMPLATES(float, fMap)
SETTING_TEMPLATES(string, sMap)
}
#undef SETTING_TEMPLATES

using namespace sq;

void SettingMap::apply() {
    for (auto& item : iMap) item.second.first = item.second.second;
    for (auto& item : bMap) item.second.first = item.second.second;
    for (auto& item : fMap) item.second.first = item.second.second;
    for (auto& item : sMap) item.second.first = item.second.second;
    updateSet.clear();
}

void SettingMap::revert() {
    for (auto& item : iMap) item.second.second = item.second.first;
    for (auto& item : bMap) item.second.second = item.second.first;
    for (auto& item : fMap) item.second.second = item.second.first;
    for (auto& item : sMap) item.second.second = item.second.first;
}

bool SettingMap::check_update(const string& _object) {
    if (updateSet.count(_object)) return false;
    else { updateSet.emplace(_object); return true; }
}
