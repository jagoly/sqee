#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/app/Settings.hpp>

#include "World.hpp"
#include "SkyLight.hpp"

using namespace sqt;
namespace maths = sq::maths;

SkyLight::SkyLight(const World& _world) :
    texA(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()),
    texB(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()),
    world(_world) {

    ubo.reserve("direction", 4u);
    ubo.reserve("colour", 3u);
    ubo.reserve("density", 1u);
    ubo.reserve("matArrA", 64u);
    ubo.reserve("matArrB", 32u);
    ubo.reserve("splits", 4u);
    ubo.create_and_allocate();
}

void SkyLight::configure() {
    if (PROP_enabled == false) {
        texA.delete_object();
        texB.delete_object();
        return;
    } else {
        uint adjSize = 1024u * (1u + world.settings.get<bool>("rpg_shadlarge"));
        texA.allocate_storage(Vec3U(adjSize, adjSize, 4u), false);
        texB.allocate_storage(Vec3U(adjSize, adjSize, 2u), false);
    }
}

void SkyLight::update() {
    if (PROP_enabled == false) return;

    Vec4F& sp = splits; array<Vec3F, 4> centres;
    const Vec3F tangent = sq::make_tangent(PROP_direction);
    const float rmin = world.camera.rmin, rmax = world.camera.rmax;
    sp.x = maths::mix(rmin+(rmax-rmin)*0.25f, rmin*std::pow(rmax/rmin, 0.25f), 0.6f);
    sp.y = maths::mix(rmin+(rmax-rmin)*0.50f, rmin*std::pow(rmax/rmin, 0.50f), 0.6f);
    sp.z = maths::mix(rmin+(rmax-rmin)*0.75f, rmin*std::pow(rmax/rmin, 0.75f), 0.6f);
    centres[0] = world.camera.pos + world.camera.dir * (rmin + sp.x) / 2.f;
    centres[1] = world.camera.pos + world.camera.dir * (sp.x + sp.y) / 2.f;
    centres[2] = world.camera.pos + world.camera.dir * (sp.y + sp.z) / 2.f;
    centres[3] = world.camera.pos + world.camera.dir * (sp.z + rmax) / 2.f;
    Mat4F viewMatA0 = maths::look_at(centres[0]-PROP_direction, centres[0], tangent);
    Mat4F viewMatA1 = maths::look_at(centres[1]-PROP_direction, centres[1], tangent);
    Mat4F viewMatA2 = maths::look_at(centres[2]-PROP_direction, centres[2], tangent);
    Mat4F viewMatA3 = maths::look_at(centres[3]-PROP_direction, centres[3], tangent);

    viewMatA0[3][0] -= std::fmod(viewMatA0[3][0], sp.x / 512.f);
    viewMatA0[3][1] -= std::fmod(viewMatA0[3][1], sp.x / 512.f);
    viewMatA0[3][2] -= std::fmod(viewMatA0[3][2], sp.x / 512.f);
    viewMatA1[3][0] -= std::fmod(viewMatA1[3][0], sp.y / 512.f);
    viewMatA1[3][1] -= std::fmod(viewMatA1[3][1], sp.y / 512.f);
    viewMatA1[3][2] -= std::fmod(viewMatA1[3][2], sp.y / 512.f);
    viewMatA2[3][0] -= std::fmod(viewMatA2[3][0], sp.z / 512.f);
    viewMatA2[3][1] -= std::fmod(viewMatA2[3][1], sp.z / 512.f);
    viewMatA2[3][2] -= std::fmod(viewMatA2[3][2], sp.z / 512.f);
    viewMatA3[3][0] -= std::fmod(viewMatA3[3][0], rmax / 512.f);
    viewMatA3[3][1] -= std::fmod(viewMatA3[3][1], rmax / 512.f);
    viewMatA3[3][2] -= std::fmod(viewMatA3[3][2], rmax / 512.f);
    matArrA[0] = maths::ortho(-sp.x, sp.x, -sp.x, sp.x, -sp.x, sp.x) * viewMatA0;
    matArrA[1] = maths::ortho(-sp.y, sp.y, -sp.y, sp.y, -sp.y, sp.y) * viewMatA1;
    matArrA[2] = maths::ortho(-sp.z, sp.z, -sp.z, sp.z, -sp.z, sp.z) * viewMatA2;
    matArrA[3] = maths::ortho(-rmax, rmax, -rmax, rmax, -rmax, rmax) * viewMatA3;

    Mat4F viewMatB = maths::look_at(world.camera.pos-PROP_direction, world.camera.pos, tangent);
    sp.w = rmax * 0.4f; Mat4F viewMatB0 = viewMatB, viewMatB1 = viewMatB;
    viewMatB0[3][0] -= std::fmod(viewMatB0[3][0], sp.w / 512.f);
    viewMatB0[3][1] -= std::fmod(viewMatB0[3][1], sp.w / 512.f);
    viewMatB0[3][2] -= std::fmod(viewMatB0[3][2], sp.w / 512.f);
    viewMatB1[3][0] -= std::fmod(viewMatB1[3][0], rmax / 512.f);
    viewMatB1[3][1] -= std::fmod(viewMatB1[3][1], rmax / 512.f);
    viewMatB1[3][2] -= std::fmod(viewMatB1[3][2], rmax / 512.f);
    matArrB[0] = maths::ortho(-sp.w, sp.w, -sp.w, sp.w, -sp.w, sp.w) * viewMatB0;
    matArrB[1] = maths::ortho(-rmax, rmax, -rmax, rmax, -rmax, rmax) * viewMatB1;

    orthArrA[0] = sq::make_OrthoFrus(matArrA[0], centres[0]);
    orthArrA[1] = sq::make_OrthoFrus(matArrA[1], centres[1]);
    orthArrA[2] = sq::make_OrthoFrus(matArrA[2], centres[2]);
    orthArrA[3] = sq::make_OrthoFrus(matArrA[3], centres[3]);
    orthArrB[0] = sq::make_OrthoFrus(matArrB[0], world.camera.pos);
    orthArrB[1] = sq::make_OrthoFrus(matArrB[1], world.camera.pos);

    ubo.update("direction", &PROP_direction);
    ubo.update("colour", &PROP_colour);
    ubo.update("density", &PROP_density);
    ubo.update("matArrA", matArrA.data());
    ubo.update("matArrB", matArrB.data());
    ubo.update("splits", &splits);
}
