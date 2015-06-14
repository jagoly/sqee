#include <glm/matrix.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/app/settings.hpp>
#include <sqee/gl/drawing.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/camera.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/render/skeleton.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../wcoe/world.hpp"
#include "../wcoe/obj/modelstatic.hpp"
#include "../wcoe/obj/modelskelly.hpp"
#include "../wcoe/obj/pointlight.hpp"
#include "../wcoe/obj/spotlight.hpp"
#include "../wcoe/obj/reflector.hpp"
#include "../wcoe/obj/emitter.hpp"
#include "../wcoe/obj/liquid.hpp"
#include "../wcoe/obj/decal.hpp"
#include "graph.hpp"

using namespace sqt;
using namespace rndr;

Graph::Graph(sq::Camera* _camera, sq::SettingsMaps* _settings)
    : camera(_camera), settings(_settings) {}

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
       return glm::distance(camera->pos, fvec3(a.lock()->bbox.sphere.origin))
            < glm::distance(camera->pos, fvec3(b.lock()->bbox.sphere.origin));
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
    uint adjSize = 1024u * INFO.shadMult;
    world->skylight.texA->resize(uvec3(adjSize, adjSize, 4u));
    world->skylight.texB->resize(uvec3(adjSize, adjSize, 2u));
    if (INFO.shadFltr) world->skylight.texA->set_preset(sq::Texture2DArray::L_C()),
                       world->skylight.texB->set_preset(sq::Texture2DArray::L_C());
    else world->skylight.texA->set_preset(sq::Texture2DArray::N_C()),
         world->skylight.texB->set_preset(sq::Texture2DArray::N_C());

    for (const auto& lptr : spotLightList) {
        const wcoe::SpotLight& light = *lptr.lock();
        if (light.PROP_shadow == true) {
            adjSize = light.PROP_texsize * INFO.shadMult;
            light.tex->resize(uvec2(adjSize, adjSize));
            if (INFO.shadFltr) light.tex->set_preset(sq::Texture2D::L_C());
            else light.tex->set_preset(sq::Texture2D::N_C());
        }
    }
    for (const auto& lptr : pointLightList) {
        const wcoe::PointLight& light = *lptr.lock();
        if (light.PROP_shadow == true) {
            adjSize = light.PROP_texsize * INFO.shadMult;
            light.tex->resize(adjSize);
            if (INFO.shadFltr) light.tex->set_preset(sq::TextureCube::L_C());
            else light.tex->set_preset(sq::TextureCube::N_C());
        }
    }
}
