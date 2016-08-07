#pragma once

#include <unordered_map>

#include <sqee/builtins.hpp>

#include "StaticStuff.hpp"

namespace sqt {
namespace world {

class StaticCell final : sq::NonCopyable {
public:
    using BodyMap = std::unordered_map<string, StaticBody>;
    using ModelMap = std::unordered_map<string, StaticModel>;
    using DecalMap = std::unordered_map<string, StaticDecal>;

    StaticBody* add_Body(const string& _name);
    StaticModel* add_Model(const string& _name);
    StaticDecal* add_Decal(const string& _name);

    const StaticBody& get_Body(const string& _name) const;
    const StaticModel& get_Model(const string& _name) const;
    const StaticDecal& get_Decal(const string& _name) const;

    const BodyMap& get_BodyMap() const { return bodyMap; }
    const ModelMap& get_ModelMap() const { return modelMap; }
    const DecalMap& get_DecalMap() const { return decalMap; }

private:
    std::unordered_map<string, StaticBody> bodyMap;
    std::unordered_map<string, StaticModel> modelMap;
    std::unordered_map<string, StaticDecal> decalMap;
};

using StaticCellMap = std::unordered_map<string, StaticCell>;

}}
