#pragma once

#include <string>
#include <vector>
#include <memory>

#include <sqee/gl/gl.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/misc/files.hpp>
#include <sqee/misc/containers.hpp>

#define SQ_BOOLCHECK(key) if (!boolMap.count(key))\
    std::cout << "FATAL: Missing bool \"" << key << "\" in object \"" << uid << "\"" << std::endl;
#define SQ_INTCHECK(key) if (!intMap.count(key))\
    std::cout << "FATAL: Missing int \"" << key << "\" in object \"" << uid << "\"" << std::endl;
#define SQ_FLOATCHECK(key) if (!floatMap.count(key))\
    std::cout << "FATAL: Missing float \"" << key << "\" in object \"" << uid << "\"" << std::endl;
#define SQ_STRINGCHECK(key) if (!stringMap.count(key))\
    std::cout << "FATAL: Missing string \"" << key << "\" in object \"" << uid << "\"" << std::endl;

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
    Object(sq::ResHolder<std::string, Mesh>& _meshH, sq::ResHolder<std::string, Skin>& _skinH,
           sq::TexHolder& _texH, const std::string& _mapPath, const std::string& _uid)
        : meshH(_meshH), skinH(_skinH), texH(_texH), mapPath(_mapPath), uid(_uid) {}

    Type type;
    sq::ResHolder<std::string, Mesh>& meshH;
    sq::ResHolder<std::string, Skin>& skinH;
    sq::TexHolder& texH;
    std::string mapPath;
    std::string uid;

    std::map<std::string, bool> boolMap;
    std::map<std::string, int> intMap;
    std::map<std::string, float> floatMap;
    std::map<std::string, std::string> stringMap;

    virtual void tick() {}
};

Object* create(const Json::Value& _json, sq::ResHolder<std::string, Mesh>& _meshH,
               sq::ResHolder<std::string, Skin>& _skinH, sq::TexHolder& _texH,
               const std::string& _mapPath, const std::string& _uid);

}
}

#include "model.hpp"
#include "liquid.hpp"
#include "data.hpp"
#include "light.hpp"
