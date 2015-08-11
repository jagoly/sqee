#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_4_1.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/objects/ModelStatic.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Emitter.hpp"
#include "../wcoe/objects/Liquid.hpp"
#include "../wcoe/objects/Decal.hpp"
#include "Graph.hpp"

using namespace sqt::rndr;

IrrVolTree::IrrVolTree() {
    texDiff.reset(new sq::TextureCubeArray());
    texSurf.reset(new sq::TextureCubeArray());
    texDepth.reset(new sq::TextureCubeArray());
    texHdr.reset(new sq::TextureCubeArray());
    texFinal.reset(new sq::TextureCubeArray());

    texDiff->create(gl::RGB, gl::RGB8, 3, false);
    texSurf->create(gl::RGB, gl::RGB8, 3, false);
    texDepth->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1, false);
    texHdr->create(gl::RGB, gl::R11F_G11F_B10F, 3, false);
    texFinal->create(gl::RGB, gl::RGB8, 3, false);

    texDiff->set_preset(sq::Texture::LinearClamp());
    texSurf->set_preset(sq::Texture::LinearClamp());
    texDepth->set_preset(sq::Texture::LinearClamp());
    texHdr->set_preset(sq::Texture::LinearClamp());
    texFinal->set_preset(sq::Texture::LinearClamp());

    texDiff->allocate_storage(uvec2(32u, 16u*9u));
    texSurf->allocate_storage(uvec2(32u, 16u*9u));
    texDepth->allocate_storage(uvec2(32u, 16u*9u));
    texFinal->allocate_storage(uvec2(32u, 16u*9u));
}

void Graph::refresh_IrrVolTree() {
}

//////////////////////////

Graph::Graph(sq::Camera* _camera, sq::Settings* _settings)
    : camera(_camera), settings(_settings) {

    // Particles /////
    gl::GenVertexArrays(1, &partVAO);
    gl::GenBuffers(1, &partVBO);
    gl::GenBuffers(1, &partIBO);

    gl::BindVertexArray(partVAO);
    gl::BindBuffer(gl::ARRAY_BUFFER, partVBO);
    gl::VertexAttribPointer(0, 4, gl::FLOAT, false, 32, (void*)(0));
    gl::VertexAttribPointer(1, 4, gl::FLOAT, false, 32, (void*)(16));
    gl::EnableVertexAttribArray(0); gl::EnableVertexAttribArray(1);
}

void Graph::update() {
    modelStaticList.remove_if(wptr_expired<wcoe::ModelStatic>);
    modelSkellyList.remove_if(wptr_expired<wcoe::ModelSkelly>);
    pointLightList.remove_if(wptr_expired<wcoe::PointLight>);
    spotLightList.remove_if(wptr_expired<wcoe::SpotLight>);
    reflectorList.remove_if(wptr_expired<wcoe::Reflector>);
    emitterList.remove_if(wptr_expired<wcoe::Emitter>);
    liquidList.remove_if(wptr_expired<wcoe::Liquid>);
    decalList.remove_if(wptr_expired<wcoe::Decal>);

    modelStaticList.sort([this] (ModelStaticList::value_type& a, ModelStaticList::value_type& b) {
       return glm::distance(camera->pos, fvec3(a.lock()->bbox.sphere.origin))
            < glm::distance(camera->pos, fvec3(b.lock()->bbox.sphere.origin));
    });

    modelSkellyList.sort([this] (ModelSkellyList::value_type& a, ModelSkellyList::value_type& b) {
       return glm::distance(camera->pos, fvec3(a.lock()->sphere.origin))
            < glm::distance(camera->pos, fvec3(b.lock()->sphere.origin));
    });

    pointLightList.sort([this] (PointLightList::value_type& a, PointLightList::value_type& b) {
       return glm::distance(camera->pos, fvec3(a.lock()->sphere.origin))
            < glm::distance(camera->pos, fvec3(b.lock()->sphere.origin));
    });

    spotLightList.sort([this] (SpotLightList::value_type& a, SpotLightList::value_type& b) {
       return glm::distance(camera->pos, fvec3(a.lock()->frus.sphere.origin))
            < glm::distance(camera->pos, fvec3(b.lock()->frus.sphere.origin));
    });

    reflectorList.sort([this] (ReflectorList::value_type& a, ReflectorList::value_type& b) {
       return glm::distance(camera->pos, fvec3(a.lock()->bbox.sphere.origin))
            < glm::distance(camera->pos, fvec3(b.lock()->bbox.sphere.origin));
    });

    emitterList.sort([this] (EmitterList::value_type& a, EmitterList::value_type& b) {
       return glm::distance(camera->pos, fvec3(a.lock()->position))
            < glm::distance(camera->pos, fvec3(b.lock()->position));
    });

//    liquidList.sort([this] (LiquidList::value_type& a, LiquidList::value_type& b) {
//       return glm::distance(camera->pos, vec3(a.lock()->bbox.sphere.origin))
//            < glm::distance(camera->pos, vec3(b.lock()->bbox.sphere.origin));
//    });

    decalList.sort([this] (DecalList::value_type& a, DecalList::value_type& b) {
       return glm::distance(camera->pos, fvec3(a.lock()->bbox.sphere.origin))
            < glm::distance(camera->pos, fvec3(b.lock()->bbox.sphere.origin));
    });
}


void Graph::reload_lists() {
    modelStaticList.clear();
    modelSkellyList.clear();
    pointLightList.clear();
    spotLightList.clear();
    reflectorList.clear();
    emitterList.clear();
    liquidList.clear();
    decalList.clear();

    for (const auto& model : world->filtered<wcoe::ModelStatic>())
        modelStaticList.emplace_back(model);
    for (const auto& model : world->filtered<wcoe::ModelSkelly>())
        modelSkellyList.emplace_back(model);
    for (const auto& light : world->filtered<wcoe::PointLight>())
        pointLightList.emplace_back(light);
    for (const auto& light : world->filtered<wcoe::SpotLight>())
        spotLightList.emplace_back(light);
    for (const auto& rflct : world->filtered<wcoe::Reflector>())
        reflectorList.emplace_back(rflct);
    for (const auto& emitr : world->filtered<wcoe::Emitter>())
        emitterList.emplace_back(emitr);
    for (const auto& liqid : world->filtered<wcoe::Liquid>())
        liquidList.emplace_back(liqid);
    for (const auto& decal : world->filtered<wcoe::Decal>())
        decalList.emplace_back(decal);

    update_settings();
}


void Graph::update_settings() {
}
