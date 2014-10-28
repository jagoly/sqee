#include <iostream>

#include <sqee/gl/gl.hpp>

#include "object.hpp"
#include "model.hpp"
#include "liquid.hpp"
#include "data.hpp"
#include "light.hpp"

namespace sqt {
namespace obj {

Object* create(const Json::Value& _json,
               MeshHolder& _meshH, SkinHolder& _skinH, sq::TexHolder& _texH,
               const std::string& _mapPath, const std::string& _uid) {
#ifdef SQEE_DEBUG
    if (!_json["type"].isString()) throw;
    std::string str = _json["type"].asString();
    if (str != "model" && str != "liquid" && str != "data" && str != "light") throw;
#endif

    Object* ptr;

    std::string vType = _json["type"].asString();
    if      (vType == "model")  ptr = new Model(_meshH, _skinH, _texH, _mapPath, _uid);
    else if (vType == "liquid") ptr = new Liquid(_meshH, _skinH, _texH, _mapPath, _uid);
    else if (vType == "data")   ptr = new Data(_meshH, _skinH, _texH, _mapPath, _uid);
    else if (vType == "light")  ptr = new Light(_meshH, _skinH, _texH, _mapPath, _uid);

    for (const std::string& key : _json.getMemberNames()) {
        const Json::Value& val = _json[key];
        if      (val.isBool())   ptr->boolMap[key]   = val.asBool();
        else if (val.isInt())    ptr->intMap[key]    = val.asInt();
        else if (val.isDouble()) ptr->floatMap[key]  = val.asFloat();
        else if (val.isString()) ptr->stringMap[key] = val.asString();
    }

    if      (vType == "model")  static_cast<Model*>(ptr)->create();
    else if (vType == "liquid") static_cast<Liquid*>(ptr)->create();
    else if (vType == "data")   static_cast<Data*>(ptr)->create();
    else if (vType == "light")  static_cast<Light*>(ptr)->create();

    return ptr;
}

}
}
