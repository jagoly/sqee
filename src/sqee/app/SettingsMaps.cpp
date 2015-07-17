#include <sqee/app/SettingsMaps.hpp>

using namespace sq;

#define SETTING_TEMPLATES(Type, Map) \
template<> void SettingsMaps::add<Type>(const string& _key, const Type& _val) { \
    Map.emplace(_key, std::pair<Type, Type>(_val, _val)); \
} \
template<> void SettingsMaps::mod<Type>(const string& _key, const Type& _val) { \
    Map.at(_key).second = _val; \
} \
template<> Type SettingsMaps::crnt<Type>(const string& _key) { \
    return Map.at(_key).first; \
} \
template<> Type SettingsMaps::next<Type>(const string& _key) { \
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

void SettingsMaps::apply() {
    for (auto& item : iMap) item.second.first = item.second.second;
    for (auto& item : bMap) item.second.first = item.second.second;
    for (auto& item : fMap) item.second.first = item.second.second;
    for (auto& item : sMap) item.second.first = item.second.second;
}

void SettingsMaps::revert() {
    for (auto& item : iMap) item.second.second = item.second.first;
    for (auto& item : bMap) item.second.second = item.second.first;
    for (auto& item : fMap) item.second.second = item.second.first;
    for (auto& item : sMap) item.second.second = item.second.first;
}
