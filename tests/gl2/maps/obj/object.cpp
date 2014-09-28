#include <libsqee/gl/gl.hpp>

#include "object.hpp"
#include "model.hpp"
#include "liquid.hpp"
#include "data.hpp"
#include "light.hpp"

namespace sqt {
namespace obj {

Ptr create(Json::Value& _json, MeshHolder* _meshH, SkinHolder* _skinH, sq::TexHolder* _texH,
           std::string& _mapPath, std::string& _uid) {
#ifdef SQEE_DEBUG
    if (!_json["type"].isString()) throw;
    std::string str = _json["type"].asString();
    if (str != "model" && str != "liquid" && str != "data" && str != "light") throw;
#endif

    Ptr ptr;

    std::string vType = _json["type"].asString();
    if      (vType == "model")  ptr = Ptr(new Model(_meshH, _skinH, _texH, _mapPath, _uid));
    else if (vType == "liquid") ptr = Ptr(new Liquid(_meshH, _skinH, _texH, _mapPath, _uid));
    else if (vType == "data")   ptr = Ptr(new Data(_meshH, _skinH, _texH, _mapPath, _uid));
    else if (vType == "light")  ptr = Ptr(new Light(_meshH, _skinH, _texH, _mapPath, _uid));

    for (std::string& key : _json.getMemberNames()) {
        Json::Value& val = _json[key];
        if      (val.isBool())   ptr->boolMap[key]   = val.asBool();
        else if (val.isInt())    ptr->intMap[key]    = val.asInt();
        else if (val.isDouble()) ptr->floatMap[key]  = val.asFloat();
        else if (val.isString()) ptr->stringMap[key] = val.asString();
    }

    if      (vType == "model")  std::static_pointer_cast<Model>(ptr)->create();
    else if (vType == "liquid") std::static_pointer_cast<Liquid>(ptr)->create();
    else if (vType == "data")   std::static_pointer_cast<Data>(ptr)->create();
    else if (vType == "light")  std::static_pointer_cast<Light>(ptr)->create();

    return ptr;
}

}
}

using namespace sqt::obj;

Object::Object(sq::ResHolder<sq::SIPair, std::shared_ptr<Mesh>>* _meshH,
               sq::ResHolder<sq::SIPair, std::shared_ptr<Skin>>* _skinH,
               sq::TexHolder* _texH, std::string _mapPath, std::string _uid) {
    meshH = _meshH; skinH = _skinH; texH = _texH;
    mapPath = _mapPath; uid = _uid;
}

void Object::tick(int) {}
