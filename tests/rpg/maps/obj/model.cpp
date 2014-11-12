#include <iostream>
#include <fstream>

#include "model.hpp"

using namespace sqt::wld;

void Model::create(ObjectSpec& _spec) {
    alpha = _spec.flagSet.count("alpha");
    shad  = _spec.flagSet.count("shad");
    refl  = _spec.flagSet.count("refl");
    refr  = _spec.flagSet.count("refr");

    const string& mName = _spec.stringMap["mesh"][0];
    if (!(mesh = resBank().meshH.get(mName))) {
        mesh = resBank().meshH.add(mName);
        mesh->load(mName);
    }

    if (_spec.stringMap.count("norm")) {
        const string& name = "models/norm/" + _spec.stringMap["norm"][0];
        if (!(texNorm = resBank().texH.get(name))) {
            texNorm = resBank().texH.add(name);
            texNorm->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, name, sq::TexPreset::L_C);
        }
    } else texNorm = &resBank().tex2D_8080FF;

    if (_spec.stringMap.count("diff")) {
        const string& name = "models/diff/" + _spec.stringMap["diff"][0];
        if (!(texDiff = resBank().texH.get(name))) {
            texDiff = resBank().texH.add(name);
            texDiff->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, name, sq::TexPreset::L_C);
        }
    } else texDiff = &resBank().tex2D_FFFFFF;

    if (_spec.stringMap.count("spec")) {
        const string& name = "models/spec/" + _spec.stringMap["spec"][0];
        if (!(texSpec = resBank().texH.get(name))) {
            texSpec = resBank().texH.add(name);
            texSpec->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, name, sq::TexPreset::L_C);
        }
    } else texSpec = &resBank().tex2D_000000;

    if (_spec.stringMap.count("ambi")) {
        const string& name = "models/ambi/" + _spec.stringMap["ambi"][0];
        if (!(texAmbi = resBank().texH.get(name))) {
            texAmbi = resBank().texH.add(name);
            texAmbi->create(gl::TEXTURE_2D, gl::RED, gl::R8, name, sq::TexPreset::L_C);
        }
    } else texAmbi = &resBank().tex2D_FFFFFF;

    glm::vec3 pos = {0, 0, 0};
    glm::vec3 rot = {0, 0, 0};
    glm::vec3 sca = {1, 1, 1};
    if (_spec.floatMap.count("pos"))
        pos = glm::make_vec3(_spec.floatMap["pos"].data());
    if (_spec.floatMap.count("rot"))
        rot = glm::make_vec3(_spec.floatMap["rot"].data());
    if (_spec.floatMap.count("sca"))
        sca = glm::make_vec3(_spec.floatMap["sca"].data());

    modelMat = sq::iMat4;
    modelMat = glm::translate(sq::iMat4, pos);
    modelMat = glm::rotate(modelMat, rot.x, {1, 0, 0});
    modelMat = glm::rotate(modelMat, rot.y, {0, 1, 0});
    modelMat = glm::rotate(modelMat, rot.z, {0, 0, 1});
    modelMat = glm::scale(modelMat, sca);
}


void Model::bind_textures() {
    texNorm->bind(gl::TEXTURE0);
    texDiff->bind(gl::TEXTURE1);
    texSpec->bind(gl::TEXTURE2);
    texAmbi->bind(gl::TEXTURE3);
}
