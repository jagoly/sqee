#pragma once

#include <string>
#include <vector>
#include <memory>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/misc/files.hpp>
#include <libsqee/resources/resholder.hpp>

namespace sqt {
namespace obj {

enum class Type {
    Model, Liquid, Data, Light
};

class Model; class Mesh; class Skin;
class Liquid;
class Data;
class Light;

class Object {
public:
    Object(sq::ResHolder<sq::SIPair, std::shared_ptr<Mesh>>* _meshH,
           sq::ResHolder<sq::SIPair, std::shared_ptr<Skin>>* _skinH,
           sq::TexHolder* _texH, std::string _mapPath, std::string _uid);

    Type type;
    sq::ResHolder<sq::SIPair, std::shared_ptr<Mesh>>* meshH;
    sq::ResHolder<sq::SIPair, std::shared_ptr<Skin>>* skinH;
    sq::TexHolder* texH;
    std::string mapPath;
    std::string uid;

    std::map<std::string, bool> boolMap;
    std::map<std::string, int> intMap;
    std::map<std::string, float> floatMap;
    std::map<std::string, std::string> stringMap;

    virtual void tick(int _tickRate);
};

typedef std::shared_ptr<Object> Ptr;

Ptr create(Json::Value& _json, sq::ResHolder<sq::SIPair, std::shared_ptr<Mesh>>* _meshH,
           sq::ResHolder<sq::SIPair, std::shared_ptr<Skin>>* _skinH, sq::TexHolder* _texH,
           std::string& _mapPath, std::string& _uid);

}
}

#include "model.hpp"
#include "liquid.hpp"
#include "data.hpp"
#include "light.hpp"
