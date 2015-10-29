#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

// Forward Declarations /////
namespace chaiscript { class Boxed_Value; }

namespace sq {

/// The SQEE Settings manager
class Settings final : NonCopyable {
public:
    Settings(); ~Settings();

    template<class T> void add(const string& _key, T _val);
    template<class T> void mod(const string& _key, T _val);
    template<class T> T get(const string& _key) const;

    void cs_mod(const string& _key, chai::Boxed_Value _val);
    chai::Boxed_Value cs_get(const string& _key) const;

private:
    unique_ptr<std::unordered_map<string, chai::Boxed_Value>> settingMap;
};

}
