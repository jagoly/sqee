#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sqee/app/logging.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/misc/files.hpp>
#include <sqee/maths/general.hpp>

#include "../main/camera.hpp"
#include "world.hpp"

using namespace sqt::wcoe;

SkyBox::SkyBox(MainCamera* _camera) : camera(_camera) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("colour", 4);
    ubo->create();
}

void SkyBox::refresh() {
    if (DAT_enabled == false) return;

    string texPath = "skybox/" + DAT_texPath;
    if (!(tex = sq::res::texCube().get(texPath)))
        tex = sq::res::texCube().add(texPath),
        tex->create(gl::RGB, gl::RGB8, 3),
        tex->set_preset(sq::TextureCube::L_C()),
        tex->buffer_full(texPath, 1024u);

    ubo->bind(1);
    ubo->update("colour", &DAT_colour);
}

void SkyBox::tick() {
    if (DAT_enabled == false) return;
}

void SkyBox::calc(double _accum) {
    if (DAT_enabled == false) return;
}


Ambient::Ambient(MainCamera* _camera) : camera(_camera) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("colour", 4);
    ubo->create();
}

void Ambient::refresh() {
    if (DAT_enabled == false) return;

    ubo->bind(1);
    ubo->update("colour", &DAT_colour);
}

void Ambient::tick() {
    if (DAT_enabled == false) return;
}

void Ambient::calc(double _accum) {
    if (DAT_enabled == false) return;
}


SkyLight::SkyLight(MainCamera* _camera) : camera(_camera) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("colour", 4);
    ubo->reserve("normal", 4);
    ubo->reserve("splitsA", 4);
    ubo->reserve("splitsB", 4);
    ubo->reserve("matArrA", 4*16);
    ubo->reserve("matArrB", 2*16);
    ubo->create();

    texA.reset(new sq::Texture2DArray());
    texB.reset(new sq::Texture2DArray());
    texA->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
    texB->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
    texA->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    texB->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    fboArrA[0].reset(new sq::Framebuffer()); fboArrA[0]->attach(gl::DEPTH_ATTACHMENT, *texA, 0);
    fboArrA[1].reset(new sq::Framebuffer()); fboArrA[1]->attach(gl::DEPTH_ATTACHMENT, *texA, 1);
    fboArrA[2].reset(new sq::Framebuffer()); fboArrA[2]->attach(gl::DEPTH_ATTACHMENT, *texA, 2);
    fboArrA[3].reset(new sq::Framebuffer()); fboArrA[3]->attach(gl::DEPTH_ATTACHMENT, *texA, 3);
    fboArrB[0].reset(new sq::Framebuffer()); fboArrB[0]->attach(gl::DEPTH_ATTACHMENT, *texB, 0);
    fboArrB[1].reset(new sq::Framebuffer()); fboArrB[1]->attach(gl::DEPTH_ATTACHMENT, *texB, 1);
}

void SkyLight::refresh() {
    if (DAT_enabled == false) return;

    ubo->bind(1);
    ubo->update("colour", &DAT_colour);
    ubo->update("normal", &DAT_normal);
}

void SkyLight::tick() {
    if (DAT_enabled == false) return;

    ubo->bind(1);

    array<vec3, 4> centres;
    const float weight = 0.6f;
    float prevSplit = camera->range.x;
    for (int i = 0; i < 4; i++) {
        float f = float(i+1) / 4.f;
        float splitUni = camera->range.x + (camera->range.y - camera->range.x) * f;
        float splitLog = camera->range.x * glm::pow(camera->range.y / camera->range.x, f);
        float splitMix = glm::mix(splitUni, splitLog, weight);
        centres[i] = camera->pos + camera->dir * (prevSplit + splitMix) / 2.f;
        splitArrA[i] = splitMix; prevSplit = splitMix;
    } ubo->update("splitsA", splitArrA.data());

    vec3 tangent = sq::make_tangent(DAT_normal);
    for (int i = 0; i < 4; i++) {
        const auto& centre = centres[i]; const auto& split = splitArrA[i];
        mat4 viewMat = glm::lookAt(centre-DAT_normal, centre, tangent);
        viewMat[3][0] -= glm::mod(viewMat[3][0], split / 512.f);
        viewMat[3][1] -= glm::mod(viewMat[3][1], split / 512.f);
        viewMat[3][2] -= glm::mod(viewMat[3][2], split / 512.f);
        mat4 projMat = glm::ortho(-split, split, -split, split, -split, split);
        matArrA[i] = projMat * viewMat;
    } ubo->update("matArrA", matArrA.data());

    mat4 viewMat = glm::lookAt(camera->pos-DAT_normal, camera->pos, tangent);
    mat4 viewMat0 = viewMat, viewMat1 = viewMat;
    float f0 = camera->range.y*0.4f, f1 = camera->range.y;
    viewMat0[3][0] -= glm::mod(viewMat0[3][0], f0 / 512.f);
    viewMat0[3][1] -= glm::mod(viewMat0[3][1], f0 / 512.f);
    viewMat0[3][2] -= glm::mod(viewMat0[3][2], f0 / 512.f);
    viewMat1[3][0] -= glm::mod(viewMat1[3][0], f1 / 512.f);
    viewMat1[3][1] -= glm::mod(viewMat1[3][1], f1 / 512.f);
    viewMat1[3][2] -= glm::mod(viewMat1[3][2], f1 / 512.f);
    matArrB[0] = glm::ortho(-f0, f0, -f0, f0, -f0, f0) * viewMat0;
    matArrB[1] = glm::ortho(-f1, f1, -f1, f1, -f1, f1) * viewMat1;
    ubo->update("splitsB", splitArrB.data());
    ubo->update("matArrB", matArrB.data());
}

void SkyLight::calc(double _accum) {
    if (DAT_enabled == false) return;
}


World::World(MainCamera* _camera, sq::SettingsMaps* _settings)
    : skybox(_camera), ambient(_camera), skylight(_camera),
      camera(_camera), settings(_settings) {}

Cell* World::add_cell(const string& _name) {
    Cell* ptr = new Cell(_name, this);
    cellMap.emplace(_name, shared_ptr<Cell>(ptr));
    return cellMap.at(_name).get();
}

Cell* World::get_cell(const string& _name) {
    return cellMap.at(_name).get();
}

void World::reload_list() {
    objectList.clear();
    for (auto& sc : cellMap)
        if (sc.second->DAT_enabled == true)
            for (auto& so : sc.second->objectMap)
                objectList.emplace_front(so.second);
}

void World::refresh() {
    skybox.refresh();
    ambient.refresh();
    skylight.refresh();
    for (auto& sc : cellMap)
        if (sc.second->DAT_enabled)
            sc.second->refresh();
    reload_list();
}

void World::tick() {
    skybox.tick();
    ambient.tick();
    skylight.tick();
    for (auto& sc : cellMap)
        if (sc.second->DAT_enabled)
            sc.second->tick();
}

void World::calc(double _accum) {
    skybox.calc(_accum);
    ambient.calc(_accum);
    skylight.calc(_accum);
    for (auto& sc : cellMap)
        if (sc.second->DAT_enabled)
            sc.second->calc(_accum);
}
