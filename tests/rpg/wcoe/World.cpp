#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <rp3d/collision/shapes/BoxShape.hpp>
#include <rp3d/engine/DynamicsWorld.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/misc/Files.hpp>

#include "World.hpp"

using namespace sqt::wcoe;

SkyBox::SkyBox(sq::Camera* _camera) : camera(_camera) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("saturation", 1);
    ubo->reserve("brightness", 1);
    ubo->reserve("contrast", 1);
    ubo->reserve("alpha", 1);
    ubo->create();
}

void SkyBox::refresh() {
    if (PROP_enabled == false) return;

    string texPath = "skybox/" + PROP_texture;
    if (!(tex = sq::res::texCube().get(texPath)))
        tex = sq::res::texCube().add(texPath),
        tex->create(gl::RGB, gl::RGB8, 3u, false),
        tex->set_preset(sq::Texture::LinearClamp()),
        tex->allocate_storage(1024u),
        tex->buffer_full(texPath);

    animate();
}

void SkyBox::tick() {
    if (PROP_enabled == false) return; bool doFinish = false;
    if (ANIM_saturation.active()) if (ANIM_saturation.tick()) doFinish = true;
    if (ANIM_brightness.active()) if (ANIM_brightness.tick()) doFinish = true;
    if (ANIM_contrast.active())   if (ANIM_contrast.tick())   doFinish = true;
    if (ANIM_alpha.active())      if (ANIM_alpha.tick())      doFinish = true;
    if (doFinish == true) animate();
}

void SkyBox::calc(double _accum) {
    if (PROP_enabled == false) return; bool doAnim = false;
    if (ANIM_saturation.active()) { ANIM_saturation.calc(_accum); doAnim = true; }
    if (ANIM_brightness.active()) { ANIM_brightness.calc(_accum); doAnim = true; }
    if (ANIM_contrast.active())   { ANIM_contrast.calc(_accum);   doAnim = true; }
    if (ANIM_alpha.active())      { ANIM_alpha.calc(_accum);      doAnim = true; }
    if (doAnim == true) animate();
}

void SkyBox::animate() {
    ubo->bind(1);
    ubo->update("saturation", &PROP_saturation);
    ubo->update("brightness", &PROP_brightness);
    ubo->update("contrast", &PROP_contrast);
    ubo->update("alpha", &PROP_alpha);
}

Ambient::Ambient(sq::Camera* _camera) : camera(_camera) {
    ubo.reset(new sq::UniformBuffer());
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

SkyLight::SkyLight(const sq::Settings& _settings, sq::Camera* _camera)
    : settings(_settings), camera(_camera) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("direction", 4);
    ubo->reserve("colour", 3);
    ubo->reserve("density", 1);
    ubo->reserve("matArrA", 4*16);
    ubo->reserve("matArrB", 2*16);
    ubo->reserve("splits", 4);
    ubo->create();

    texDepthA.reset(new sq::TextureMut2DArray());
    texDepthB.reset(new sq::TextureMut2DArray());
    texDepthA->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1u);
    texDepthB->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1u);
    texDepthA->set_preset(sq::Texture::ShadowMap());
    texDepthB->set_preset(sq::Texture::ShadowMap());

    fboArrA[0].reset(new sq::FrameBuffer());
    fboArrA[1].reset(new sq::FrameBuffer());
    fboArrA[2].reset(new sq::FrameBuffer());
    fboArrA[3].reset(new sq::FrameBuffer());
    fboArrB[0].reset(new sq::FrameBuffer());
    fboArrB[1].reset(new sq::FrameBuffer());
    fboArrA[0]->attach(gl::DEPTH_ATTACHMENT, *texDepthA, 0u);
    fboArrA[1]->attach(gl::DEPTH_ATTACHMENT, *texDepthA, 1u);
    fboArrA[2]->attach(gl::DEPTH_ATTACHMENT, *texDepthA, 2u);
    fboArrA[3]->attach(gl::DEPTH_ATTACHMENT, *texDepthA, 3u);
    fboArrB[0]->attach(gl::DEPTH_ATTACHMENT, *texDepthB, 0u);
    fboArrB[1]->attach(gl::DEPTH_ATTACHMENT, *texDepthB, 1u);
}

void SkyLight::refresh() {
    if (settings.check<bool>("rpg_shadlarge")) {
        uint adjSize = 1024u * (1u + settings.get<bool>("rpg_shadlarge"));
        texDepthA->resize(uvec3(adjSize, adjSize, 4u));
        texDepthB->resize(uvec3(adjSize, adjSize, 2u));
    }

    if (PROP_enabled == false) return;
    animate();
}

void SkyLight::tick() {
    if (PROP_enabled == false) return; bool doFinish = false;
    if (ANIM_direction.active()) if (ANIM_direction.tick()) doFinish = true;
    if (ANIM_colour.active())    if (ANIM_colour.tick())    doFinish = true;
    if (ANIM_density.active())   if (ANIM_density.tick())   doFinish = true;
    if (doFinish == true) animate();
}

void SkyLight::calc(double _accum) {
    if (PROP_enabled == false) return; bool doAnim = false;
    if (ANIM_direction.active()) { ANIM_direction.calc(_accum); doAnim = true; }
    if (ANIM_colour.active())    { ANIM_colour.calc(_accum);    doAnim = true; }
    if (ANIM_density.active())   { ANIM_density.calc(_accum);   doAnim = true; }
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

    orthArrA[0] = sq::make_OrthoFrus(matArrA[0], centres[0]);
    orthArrA[1] = sq::make_OrthoFrus(matArrA[1], centres[1]);
    orthArrA[2] = sq::make_OrthoFrus(matArrA[2], centres[2]);
    orthArrA[3] = sq::make_OrthoFrus(matArrA[3], centres[3]);
    orthArrB[0] = sq::make_OrthoFrus(matArrB[0], camera->pos);
    orthArrB[1] = sq::make_OrthoFrus(matArrB[1], camera->pos);

    ubo->update("matArrA", matArrA.data());
    ubo->update("matArrB", matArrB.data());
    ubo->update("splits", &splits);
}

void SkyLight::animate() {
    ubo->bind(1);
    ubo->update("direction", &PROP_direction);
    ubo->update("colour", &PROP_colour);
    ubo->update("density", &PROP_density);
}


World::World(const sq::Settings& _settings, sq::Camera* _camera)
    : skybox(_camera), ambient(_camera), skylight(_settings, _camera),
      settings(_settings), camera(_camera) {
    physWorld.reset(new rp3d::DynamicsWorld({0.f, 0.f, -1.f}));
    physWorld->setNbIterationsVelocitySolver(18u);
    physWorld->setNbIterationsPositionSolver(10u);
}


World::~World() {}


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
    physWorld->update(1.f / 24.f);

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

void World::invalidate() {
    for (auto& sc : cellMap)
        if (sc.second->DAT_enabled)
            sc.second->invalidate();
}
