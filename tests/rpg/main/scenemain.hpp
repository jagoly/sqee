#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/scene.hpp>

namespace sqt {

namespace wcoe { class World; }
namespace rndr { class Graph; }

class MainCamera;

class SceneMain : public sq::Scene {
public:
    SceneMain(sq::Application* _app);

    void update();
    void render(float _ft);
    void update_settings();

private:
    unique_ptr<sq::Pipeline> pipeline;
    unique_ptr<MainCamera> camera;
    unique_ptr<wcoe::World> world;
    unique_ptr<rndr::Graph> graph;

    sq::Direction moveDir;
    vec3 posCrnt, posNext;
    float rotX, rotZ;

    struct {
        unique_ptr<sq::Shader> gnrc_screen;
        unique_ptr<sq::Shader> modl_static;
        unique_ptr<sq::Shader> modl_skelly;
        unique_ptr<sq::Shader> modl_stencil;
        unique_ptr<sq::Shader> modl_decal;
        unique_ptr<sq::Shader> modl_static_refl;
        unique_ptr<sq::Shader> modl_skelly_refl;
        unique_ptr<sq::Shader> modl_stencil_refl;
        unique_ptr<sq::Shader> shds_skybox;
        unique_ptr<sq::Shader> shds_skybox_refl;
        unique_ptr<sq::Shader> shad_static;
        unique_ptr<sq::Shader> shad_skelly;
        unique_ptr<sq::Shader> shds_reflector;
    } VS;

    struct {
        unique_ptr<sq::Shader> gnrc_fillwith;
        unique_ptr<sq::Shader> gnrc_passthru;
        unique_ptr<sq::Shader> gnrc_lumalpha;
        unique_ptr<sq::Shader> modl_write;
        unique_ptr<sq::Shader> modl_decal;
        unique_ptr<sq::Shader> modl_write_refl;
        unique_ptr<sq::Shader> shds_skybox;
        unique_ptr<sq::Shader> shds_ambient;
        unique_ptr<sq::Shader> shds_skylight;
        unique_ptr<sq::Shader> shds_spot_none;
        unique_ptr<sq::Shader> shds_spot_shad;
        unique_ptr<sq::Shader> shds_spot_spec;
        unique_ptr<sq::Shader> shds_spot_both;
        unique_ptr<sq::Shader> shds_point_none;
        unique_ptr<sq::Shader> shds_point_shad;
        unique_ptr<sq::Shader> shds_point_spec;
        unique_ptr<sq::Shader> shds_point_both;
        unique_ptr<sq::Shader> shds_ambient_refl;
        unique_ptr<sq::Shader> shds_skylight_refl;
        unique_ptr<sq::Shader> shds_spot_none_refl;
        unique_ptr<sq::Shader> shds_spot_shad_refl;
        unique_ptr<sq::Shader> shds_point_none_refl;
        unique_ptr<sq::Shader> shds_point_shad_refl;
        unique_ptr<sq::Shader> shds_reflector;
        unique_ptr<sq::Shader> shad_punch;
        unique_ptr<sq::Shader> prty_ssao_ssao;
        unique_ptr<sq::Shader> prty_ssao_blur;
        unique_ptr<sq::Shader> prty_hdr_highs;
        unique_ptr<sq::Shader> prty_hdr_blurh;
        unique_ptr<sq::Shader> prty_hdr_blurv;
        unique_ptr<sq::Shader> prty_hdr_tones;
        unique_ptr<sq::Shader> prty_fxaa_fxaa;
        unique_ptr<sq::Shader> prty_vignette;
    } FS;

    struct {
        unique_ptr<sq::Texture2D> depth;
        unique_ptr<sq::Texture2D> diff;
        unique_ptr<sq::Texture2D> surf;
        unique_ptr<sq::Texture2D> norm;
        unique_ptr<sq::Texture2D> spec;
        unique_ptr<sq::Texture2D> hdr;
        unique_ptr<sq::Texture2D> basic;
        unique_ptr<sq::Texture2D> ssaoA;
        unique_ptr<sq::Texture2D> ssaoB;
        unique_ptr<sq::Texture2D> bloomA;
        unique_ptr<sq::Texture2D> bloomB;
    } TX;

    struct {
        unique_ptr<sq::Framebuffer> defr;
        unique_ptr<sq::Framebuffer> hdr;
        unique_ptr<sq::Framebuffer> basic;
        unique_ptr<sq::Framebuffer> ssaoA;
        unique_ptr<sq::Framebuffer> ssaoB;
        unique_ptr<sq::Framebuffer> bloomA;
        unique_ptr<sq::Framebuffer> bloomB;
    } FB;

    struct {
        bool ssaoEnable;
        bool hdrbEnable;
        bool fxaaEnable;
        bool vgntEnable;
        bool shadFltr;
        uint shadMult;
        float viewDist;
        uvec2 fullSize;
        uvec2 halfSize;
        uvec2 qterSize;
        vec2 fPixSize;
        vec2 hPixSize;
        vec2 qPixSize;
    } INFO;

    void reload_shaders();
};

}
