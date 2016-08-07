#include <sqee/assert.hpp>

#include "StaticCell.hpp"

using namespace sqt::world;

StaticBody* StaticCell::add_Body(const string& _name) {
    SQASSERT(!bodyMap.count(_name), "cell already has body");
    return &bodyMap[_name];
}

StaticModel* StaticCell::add_Model(const string& _name) {
    SQASSERT(!modelMap.count(_name), "cell already has model");
    return &modelMap[_name];
}

StaticDecal* StaticCell::add_Decal(const string& _name) {
    SQASSERT(!decalMap.count(_name), "cell already has decal");
    return &decalMap[_name];
}


const StaticBody& StaticCell::get_Body(const string& _name) const {
    SQASSERT(bodyMap.count(_name), "cell doesn't have body");
    return bodyMap.at(_name);
}

const StaticModel& StaticCell::get_Model(const string& _name) const {
    SQASSERT(modelMap.count(_name), "cell doesn't have model");
    return modelMap.at(_name);
}

const StaticDecal& StaticCell::get_Decal(const string& _name) const {
    SQASSERT(decalMap.count(_name), "cell doesn't have decal");
    return decalMap.at(_name);
}
