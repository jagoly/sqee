#include <libsqee/gl/gl.hpp>
#include <libsqee/misc/files.hpp>

#include "models.hpp"

namespace sqt {

void Skin::load(std::string& _filePath, sq::TexHolderBasic* _texHolder) {
    Json::Value rootSkin = sq::get_json_from_file(_filePath);

    std::string str = rootSkin["ambi"].asString();
    std::cout << str << std::endl;
    if      (str == "none") ambi = 1;
    else if (str == "full") ambi = 2;
    else                    ambi = 0;
    alpha = rootSkin["alpha"].asBool();

    std::string name;

    // Normals
    name = "models/norm/" + rootSkin["textures"][0].asString() + ".png";
    if (_texHolder->has(name)) {
        texNorm = _texHolder->get(name);
    } else {
        texNorm = sq::tex2D_load_file("res/textures/" + name, gl::RGB16F);
        texNorm->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texNorm->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        _texHolder->set(name, texNorm);
    }

    // Diffuse
    name = "models/diff/" + rootSkin["textures"][1].asString() + ".png";
    if (_texHolder->has(name)) {
        texDiff = _texHolder->get(name);
    } else {
        texDiff = sq::tex2D_load_file("res/textures/" + name, gl::RGBA16F);
        texDiff->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texDiff->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        _texHolder->set(name, texDiff);
    }

    // Specular
    name = "models/spec/" + rootSkin["textures"][2].asString() + ".png";
    if (_texHolder->has(name)) {
        texSpec = _texHolder->get(name);
    } else {
        texSpec = sq::tex2D_load_file("res/textures/" + name, gl::RGB16F);
        if (texSpec == nullptr) texSpec = sq::tex2D_load_blank({1, 1}, gl::RGB16F);
        texSpec->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texSpec->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        _texHolder->set(name, texSpec);
    }
}

}
