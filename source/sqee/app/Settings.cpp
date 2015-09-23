#include <stdexcept>

#include <sqee/app/Settings.hpp>

using std::out_of_range;

using namespace sq;

#define SETTING_TEMPLATES(Type, Map, Changes) \
template<> void Settings::add<Type>(const string& _key, const Type& _val) { \
    if (Map.count(_key)) throw out_of_range("\""+_key+"\" already in "#Type" settings"); \
    Changes.emplace(_key); Map.emplace(_key, _val); \
} \
template<> void Settings::mod<Type>(const string& _key, const Type& _val) { \
    if (!Map.count(_key)) throw out_of_range("\""+_key+"\" not in "#Type" settings"); \
    Changes.emplace(_key); Map.at(_key) = _val; \
} \
template<> bool Settings::check<Type>(const string& _key) const { \
    if (!Map.count(_key)) throw out_of_range("\""+_key+"\" not in "#Type" settings"); \
    return Changes.count(_key); \
} \
template<> Type Settings::get<Type>(const string& _key) const { \
    if (!Map.count(_key)) throw out_of_range("\""+_key+"\" not in "#Type" settings"); \
    return Map.at(_key); \
}

namespace sq {
SETTING_TEMPLATES(int, intMap, intChanges)
SETTING_TEMPLATES(bool, boolMap, boolChanges)
SETTING_TEMPLATES(float, floatMap, floatChanges)
SETTING_TEMPLATES(string, stringMap, stringChanges)
}
#undef SETTING_TEMPLATES

void Settings::clear_changed() {
    intChanges.clear();
    boolChanges.clear();
    floatChanges.clear();
    stringChanges.clear();
}
