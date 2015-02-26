#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/models/mesh.hpp>

#include "liquid.hpp"

using namespace sqt;

Liquid::Liquid(const ObjectSpec& _spec) : Object(_spec) {
    ubo.reserve("reflMat", 16);
    ubo.reserve("wSmooth", 1);
    ubo.reserve("wScale", 1);
    ubo.reserve("flowOffset", 2);
    ubo.reserve("translation", 3); // needs to go
    ubo.reserve("normProg", 1);
    ubo.reserve("colour", 3);
    ubo.reserve("thickness", 1);
    ubo.reserve("normA", 1);
    ubo.reserve("normB", 1);

    ubo.create();

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);

    glm::vec3 pos = glm::make_vec3(_spec.fMap.at("pos").data());
    string texDir = _spec.sMap.at("texDir")[0];

    const string& mPath = _spec.sMap.at("mesh")[0];
    if (!(mesh = sq::res::mesh().get(mPath))) {
        mesh = sq::res::mesh().add(mPath);
        mesh->create(mPath);
    }

    wScale = _spec.fMap.at("scale")[0];
    wSmooth = _spec.fMap.at("smooth")[0];
    flow = glm::make_vec2(_spec.fMap.at("flow").data());
    colour = glm::make_vec3(_spec.fMap.at("colour").data());
    thickness = _spec.fMap.at("thickness")[0];

    glm::mat4 reflMat {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 2.f*pos.z, 1
    };

    if (!(normArray = sq::res::texture().get("waternorms_"+texDir))) {
        normArray = sq::res::texture().add("waternorms_"+texDir);
        normArray->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::Texture::Preset::L_R);
        normArray->resize({1024, 1024, 4});
        normArray->buffer_file("static/water/"+texDir+"/0", 0);
        normArray->buffer_file("static/water/"+texDir+"/1", 1);
        normArray->buffer_file("static/water/"+texDir+"/2", 2);
        normArray->buffer_file("static/water/"+texDir+"/3", 3);
    }

    update_ubo();
    ubo.update("reflMat", &reflMat);
    ubo.update("translation", &pos);
}

void Liquid::update_ubo() {
    ubo.bind(2);
    ubo.update("wSmooth", &wSmooth);
    ubo.update("wScale", &wScale);
    ubo.update("thickness", &thickness);
    ubo.update("colour", &colour);
}

void Liquid::bind_ubo() {
    ubo.bind(2);
    ubo.update("flowOffset", &flowOffs);
    ubo.update("normA", &normInfo.x);
    ubo.update("normB", &normInfo.y);
    ubo.update("normProg", &normInfo.z);
}

void Liquid::draw() {
    normArray->bind(gl::TEXTURE3);
    mesh->bind_vao();
    mesh->draw_ibo();
}

void Liquid::tick() {
    flowOffsA = flowOffsB;
    flowOffsB.x += flow.x / wScale;
    flowOffsB.y += flow.y / wScale;

    static int ticker = 191;
    if (++ticker == 192) ticker = 0;
    int ind = ticker / 48;
    if      (ind == 0) { normInfo.x = 0; normInfo.y = 1; }
    else if (ind == 1) { normInfo.x = 1; normInfo.y = 2; }
    else if (ind == 2) { normInfo.x = 2; normInfo.y = 3; }
    else if (ind == 3) { normInfo.x = 3; normInfo.y = 0; }

    normOffsA = normOffsB;
    uchar rem = ticker % 48;
    if (!rem) { normOffsA = 0.f; normOffsB = 1.f / 48.f; }
    else normOffsB += 1.f / 48.f;
}

void Liquid::calc(double _accum) {
    const double dt = 1.d / 24.d;
    flowOffs = glm::mix(flowOffsA, flowOffsB, _accum / dt);
    normInfo.z = glm::mix(normOffsA, normOffsB, _accum / dt);

}
