#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sqee/app/logging.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/render/camera.hpp>
#include <sqee/misc/files.hpp>
#include <sqee/maths/general.hpp>

#include "world.hpp"

using namespace sqt::wcoe;

SkyBox::SkyBox(sq::Camera* _camera) : camera(_camera) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("colour", 4);
    ubo->create();
}

void SkyBox::refresh() {
    if (PROP_enabled == false) return;

    string texPath = "skybox/" + PROP_texPath;
    if (!(tex = sq::res::texCube().get(texPath)))
        tex = sq::res::texCube().add(texPath),
        tex->create(gl::RGB, gl::RGB8, 3),
        tex->set_preset(sq::TextureCube::L_C()),
        tex->buffer_full(texPath, 1024u);

    ubo->bind(1);
    ubo->update("colour", &PROP_colour);
}

void SkyBox::tick() {
    if (PROP_enabled == false) return;
}

void SkyBox::calc(double _accum) {
    if (PROP_enabled == false) return;
}


Ambient::Ambient(sq::Camera* _camera) : camera(_camera) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("colour", 4);
    ubo->create();
}

void Ambient::refresh() {
    if (PROP_enabled == false) return;
    animate();
}

void Ambient::tick() {
    if (PROP_enabled == false) return; bool doFinish = false;
    if (ANIM_colour.active()) if (ANIM_colour.tick()) doFinish = true;
    if (doFinish == true) animate();
}

void Ambient::calc(double _accum) {
    if (PROP_enabled == false) return; bool doAnim = false;
    if (ANIM_colour.active()) { ANIM_colour.calc(_accum); doAnim = true; }
    if (doAnim == true) animate();
}

void Ambient::animate() {
    ubo->bind(1);
    ubo->update("colour", &PROP_colour);
}

SkyLight::SkyLight(sq::Camera* _camera) : camera(_camera) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("direction", 4);
    ubo->reserve("colour", 4);
    ubo->reserve("matArrA", 4*16);
    ubo->reserve("matArrB", 2*16);
    ubo->reserve("splits", 4);
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
    if (PROP_enabled == false) return;
    animate();
}

void SkyLight::tick() {
    if (PROP_enabled == false) return; bool doFinish = false;
    if (ANIM_direction.active()) if (ANIM_direction.tick()) doFinish = true;
    if (ANIM_colour.active())    if (ANIM_colour.tick())    doFinish = true;
    if (doFinish == true) animate();
}

void SkyLight::calc(double _accum) {
    if (PROP_enabled == false) return; bool doAnim = false;
    if (ANIM_direction.active()) { ANIM_direction.calc(_accum); doAnim = true; }
    if (ANIM_colour.active())    { ANIM_colour.calc(_accum);    doAnim = true; }
    if (doAnim == true) animate(); else ubo->bind(1);

    fvec4& sp = splits; array<fvec3, 4> centres;
    const float rmin = camera->rmin, rmax = camera->rmax;
    const fvec3 tangent = sq::make_tangent(PROP_direction);
    sp.x = glm::mix(rmin+(rmax-rmin)*0.25f, rmin*glm::pow(rmax/rmin, 0.25f), 0.6f);
    sp.y = glm::mix(rmin+(rmax-rmin)*0.50f, rmin*glm::pow(rmax/rmin, 0.50f), 0.6f);
    sp.z = glm::mix(rmin+(rmax-rmin)*0.75f, rmin*glm::pow(rmax/rmin, 0.75f), 0.6f);
    centres[0] = camera->pos + camera->dir * (rmin + sp.x) / 2.f;
    centres[1] = camera->pos + camera->dir * (sp.x + sp.y) / 2.f;
    centres[2] = camera->pos + camera->dir * (sp.y + sp.z) / 2.f;
    centres[3] = camera->pos + camera->dir * (sp.z + rmax) / 2.f;
    fmat4 viewMatA0 = glm::lookAt(centres[0]-PROP_direction, centres[0], tangent);
    fmat4 viewMatA1 = glm::lookAt(centres[1]-PROP_direction, centres[1], tangent);
    fmat4 viewMatA2 = glm::lookAt(centres[2]-PROP_direction, centres[2], tangent);
    fmat4 viewMatA3 = glm::lookAt(centres[3]-PROP_direction, centres[3], tangent);
    viewMatA0[3][0] -= glm::mod(viewMatA0[3][0], sp.x / 512.f);
    viewMatA0[3][1] -= glm::mod(viewMatA0[3][1], sp.x / 512.f);
    viewMatA0[3][2] -= glm::mod(viewMatA0[3][2], sp.x / 512.f);
    viewMatA1[3][0] -= glm::mod(viewMatA1[3][0], sp.y / 512.f);
    viewMatA1[3][1] -= glm::mod(viewMatA1[3][1], sp.y / 512.f);
    viewMatA1[3][2] -= glm::mod(viewMatA1[3][2], sp.y / 512.f);
    viewMatA2[3][0] -= glm::mod(viewMatA2[3][0], sp.z / 512.f);
    viewMatA2[3][1] -= glm::mod(viewMatA2[3][1], sp.z / 512.f);
    viewMatA2[3][2] -= glm::mod(viewMatA2[3][2], sp.z / 512.f);
    viewMatA3[3][0] -= glm::mod(viewMatA3[3][0], rmax / 512.f);
    viewMatA3[3][1] -= glm::mod(viewMatA3[3][1], rmax / 512.f);
    viewMatA3[3][2] -= glm::mod(viewMatA3[3][2], rmax / 512.f);
    matArrA[0] = glm::ortho(-sp.x, sp.x, -sp.x, sp.x, -sp.x, sp.x) * viewMatA0;
    matArrA[1] = glm::ortho(-sp.y, sp.y, -sp.y, sp.y, -sp.y, sp.y) * viewMatA1;
    matArrA[2] = glm::ortho(-sp.z, sp.z, -sp.z, sp.z, -sp.z, sp.z) * viewMatA2;
    matArrA[3] = glm::ortho(-rmax, rmax, -rmax, rmax, -rmax, rmax) * viewMatA3;

    fmat4 viewMatB = glm::lookAt(camera->pos-PROP_direction, camera->pos, tangent);
    sp.w = rmax * 0.4f; fmat4 viewMatB0 = viewMatB, viewMatB1 = viewMatB;
    viewMatB0[3][0] -= glm::mod(viewMatB0[3][0], sp.w / 512.f);
    viewMatB0[3][1] -= glm::mod(viewMatB0[3][1], sp.w / 512.f);
    viewMatB0[3][2] -= glm::mod(viewMatB0[3][2], sp.w / 512.f);
    viewMatB1[3][0] -= glm::mod(viewMatB1[3][0], rmax / 512.f);
    viewMatB1[3][1] -= glm::mod(viewMatB1[3][1], rmax / 512.f);
    viewMatB1[3][2] -= glm::mod(viewMatB1[3][2], rmax / 512.f);
    matArrB[0] = glm::ortho(-sp.w, sp.w, -sp.w, sp.w, -sp.w, sp.w) * viewMatB0;
    matArrB[1] = glm::ortho(-rmax, rmax, -rmax, rmax, -rmax, rmax) * viewMatB1;

    ubo->update("matArrA", matArrA.data());
    ubo->update("matArrB", matArrB.data());
    ubo->update("splits", &splits);
}

void SkyLight::animate() {
    ubo->bind(1);
    ubo->update("direction", &PROP_direction);
    ubo->update("colour", &PROP_colour);
}


World::World(sq::Camera* _camera, sq::SettingsMaps* _settings)
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
