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

SkyBox::SkyBox() {
}

void SkyBox::set_colour(const vec3& _colour) {
    colour = _colour;
}

void SkyBox::set_texture(const string& _path) {
    string texPath = "skybox/" + _path;
    if (!(tex = sq::res::texCube().get(texPath)))
        tex = sq::res::texCube().add(texPath),
        tex->create(gl::RGB, gl::RGB8, 3),
        tex->set_preset(sq::TextureCube::L_C()),
        tex->buffer_full(texPath, 1024u);
}

void SkyBox::tick() {}
void SkyBox::calc(double _accum) {}


Ambient::Ambient() {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("colour", 4);
    ubo->create();
}

void Ambient::set_colour(const vec3& _colour) {
    colour = _colour; ubo->bind(1);
    ubo->update("colour", &colour);
}

void Ambient::tick() {}
void Ambient::calc(double _accum) {}


SkyLight::SkyLight() {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("colour", 4);
    ubo->reserve("direction", 4);
    ubo->reserve("splits", 4);
    ubo->reserve("matArr", 4*16);
    ubo->create();

    tex.reset(new sq::Texture2DArray());
    tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
    tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    fboArr[0].reset(new sq::Framebuffer()); fboArr[0]->attach(gl::DEPTH_ATTACHMENT, *tex, 0);
    fboArr[1].reset(new sq::Framebuffer()); fboArr[1]->attach(gl::DEPTH_ATTACHMENT, *tex, 1);
    fboArr[2].reset(new sq::Framebuffer()); fboArr[2]->attach(gl::DEPTH_ATTACHMENT, *tex, 2);
    fboArr[3].reset(new sq::Framebuffer()); fboArr[3]->attach(gl::DEPTH_ATTACHMENT, *tex, 3);
}

void SkyLight::set_colour(const vec3& _colour) {
    colour = _colour; ubo->bind(1);
    ubo->update("colour", &colour);
}

void SkyLight::set_direction(const vec3& _direction) {
    direction = _direction; ubo->bind(1);
    ubo->update("direction", &direction);
}

void SkyLight::tick() {
    ubo->bind(1);

    const float weight = 0.666f;
    float prevSplit = camera->range.x;
    for (uint sliceInd = 0; sliceInd < 4; sliceInd++) {
        float f = float(sliceInd+1) / 4.f;
        float splitUni = camera->range.x + (camera->range.y - camera->range.x) * f;
        float splitLog = camera->range.x * glm::pow(camera->range.y / camera->range.x, f);
        float splitMix = glm::mix(splitUni, splitLog, weight);
        mat4 proj = glm::perspective(camera->fov, camera->size.x/camera->size.y, prevSplit, splitMix);
        frusArr[sliceInd] = sq::make_Frustum(proj*camera->viewMat, camera->pos, camera->dir, camera->range);
        splitArr[sliceInd] = splitMix; prevSplit = splitMix;
    } ubo->update("splits", splitArr.data());

    vec3 tangent = sq::make_tangent(direction);
    for (int i = 0; i < 4; i++) {
        const auto& split = splitArr[i]; const auto& frus = frusArr[i];
        vec3 frusCentre = sq::calc_frusCentre(frus);
        mat4 viewMat = glm::lookAt(frusCentre-direction, frusCentre, tangent);
        viewMat[3][0] -= glm::mod(viewMat[3][0], split / 512.f);
        viewMat[3][1] -= glm::mod(viewMat[3][1], split / 512.f);
        viewMat[3][2] -= glm::mod(viewMat[3][2], split / 512.f);
        mat4 projMat = glm::ortho(-split, split, -split, split, -split, split);
        matArr[i] = projMat * viewMat;
    } ubo->update("matArr", matArr.data());
}

void SkyLight::calc(double _accum) {}


void World::add_cell(const string& _name, vec3 _position) {
    Cell* ptr = new Cell(_name, _position, this);
    cellMap.emplace(_name, shared_ptr<Cell>(ptr));
}

Cell& World::get_cell(const string& _name) {
    return *cellMap.at(_name);
}

void World::enable_cell(const string& _cell) {
    cellMap.at(_cell)->enabled = true;
    for (const auto& so : cellMap.at(_cell)->objectMap) {
        objectList.remove_if([so](const weak_ptr<Object>& val) {
            return val.lock().get() == so.second.get(); });
        objectList.emplace_front(so.second);
    }
}

void World::disable_cell(const string& _cell) {
    cellMap.at(_cell)->enabled = false;
    for (const auto& so : cellMap.at(_cell)->objectMap) {
        objectList.remove_if([so](const weak_ptr<Object>& val) {
            return val.lock().get() == so.second.get(); });
    }
}

void World::tick() {
    skybox.tick();
    ambient.tick();
    skylight.tick();
    for (auto& sc : cellMap) sc.second->tick();
}

void World::calc(double _accum) {
    skybox.calc(_accum);
    ambient.calc(_accum);
    skylight.calc(_accum);
    for (auto& sc : cellMap) sc.second->calc(_accum);
}
