#include <stdexcept>

#include <chaiscript/dispatchkit/boxed_value.hpp>
#include <chaiscript/dispatchkit/boxed_cast.hpp>

#include <sqee/redist/tinyformat.hpp>
#include <sqee/app/Settings.hpp>

using namespace sq;


Settings::Settings() : settingMap(new std::unordered_map<string, chai::Boxed_Value>()) {}
Settings::~Settings() = default;


template<class T> void Settings::add(const string& _key, T _val) {
    if (settingMap->count(_key) == true) throw std::out_of_range(
        tfm::format("\"%s\" already in settings (type is \"%s\")",
                    _key, settingMap->at(_key).get_type_info().name()));

    settingMap->emplace(_key, chai::var(_val));
}


template<class T> void Settings::mod(const string& _key, T _val) {
    if (settingMap->count(_key) == false) throw std::out_of_range(
        tfm::format("\"%s\" not in settings", _key));

    if (settingMap->at(_key).get_type_info().bare_type_info() != &typeid(T))
        throw std::out_of_range(tfm::format("\"%s\" of wrong type (type is \"%s\")",
                                _key, settingMap->at(_key).get_type_info().name()));

    settingMap->at(_key).assign(chai::var(_val));
}


template<class T> T Settings::get(const string& _key) const {
    if (settingMap->count(_key) == false) throw std::out_of_range(
        tfm::format("\"%s\" not in settings", _key));

    if (settingMap->at(_key).get_type_info().bare_type_info() != &typeid(T))
        throw std::out_of_range(tfm::format("\"%s\" of wrong type (type is \"%s\")",
                                _key, settingMap->at(_key).get_type_info().name()));

    return chai::boxed_cast<T>(settingMap->at(_key));
}


void Settings::cs_mod(const string& _key, chai::Boxed_Value _val) {
    if (settingMap->count(_key) == false) throw std::out_of_range(
        tfm::format("\"%s\" not in settings", _key));

    if (settingMap->at(_key).get_type_info().bare_type_info() != _val.get_type_info().bare_type_info())
        throw std::out_of_range(tfm::format("\"%s\" of wrong type (type is \"%s\")",
                                _key, settingMap->at(_key).get_type_info().name()));

    settingMap->at(_key).assign(_val);
}


chai::Boxed_Value Settings::cs_get(const string& _key) const {
    if (settingMap->count(_key) == false) throw std::out_of_range(
        tfm::format("\"%s\" not in settings", _key));

    return settingMap->at(_key);
}


template void Settings::add<int>(const string&, int);
template void Settings::add<bool>(const string&, bool);
template void Settings::add<float>(const string&, float);
template void Settings::add<string>(const string&, string);

template void Settings::mod<int>(const string&, int);
template void Settings::mod<bool>(const string&, bool);
template void Settings::mod<float>(const string&, float);
template void Settings::mod<string>(const string&, string);

template int Settings::get<int>(const string&) const;
template bool Settings::get<bool>(const string&) const;
template float Settings::get<float>(const string&) const;
template string Settings::get<string>(const string&) const;
