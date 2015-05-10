#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/scene.hpp>

namespace sqt {

namespace wcoe { class World; }
namespace rndr { class Graph; }

class MainCamera;

class SceneMain : public sq::Scene {
public:
    SceneMain(sq::Application& _app);

    void update();
    void resize(uvec2 _size);
    void render(float _ft);

private:
    shared_ptr<MainCamera> camera;
    shared_ptr<wcoe::World> world;
    shared_ptr<rndr::Graph> graph;

    sq::Direction moveDir;
    vec3 posCrnt, posNext;
    float rotX, rotZ;

    unique_ptr<sq::Pipeline> pipeline;

    struct {
        unique_ptr<sq::Shader> gnrc_screen;
        unique_ptr<sq::Shader> modl_static;
        unique_ptr<sq::Shader> modl_skelly;
        unique_ptr<sq::Shader> shds_skybox;
        unique_ptr<sq::Shader> shad_static;
        unique_ptr<sq::Shader> shad_skelly;
        unique_ptr<sq::Shader> shds_spotstncl;
        unique_ptr<sq::Shader> shds_pointstncl;
    } VS;

    struct {
        unique_ptr<sq::Shader> gnrc_fillwith;
        unique_ptr<sq::Shader> gnrc_passthru;
        unique_ptr<sq::Shader> gnrc_lumalpha;
        unique_ptr<sq::Shader> modl_write;
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
        unique_ptr<sq::Shader> shad_punch;
        unique_ptr<sq::Shader> prty_ssao_ssao;
        unique_ptr<sq::Shader> prty_ssao_blur;
        unique_ptr<sq::Shader> prty_hdr_blmhigh;
        unique_ptr<sq::Shader> prty_hdr_blmblur;
        unique_ptr<sq::Shader> prty_hdr_tonemap;
        unique_ptr<sq::Shader> prty_fxaa_fxaa;
        unique_ptr<sq::Shader> prty_vignette;
    } FS;

    struct {
        unique_ptr<sq::Framebuffer> defr;
        unique_ptr<sq::Framebuffer> ssaoA;
        unique_ptr<sq::Framebuffer> ssaoB;
        unique_ptr<sq::Framebuffer> bloomA;
        unique_ptr<sq::Framebuffer> bloomB;
        unique_ptr<sq::Framebuffer> hdr;
        unique_ptr<sq::Framebuffer> basic;
    } FB;

    struct {
        unique_ptr<sq::Texture2D> defrDiff;
        unique_ptr<sq::Texture2D> defrNorm;
        unique_ptr<sq::Texture2D> defrSurf;
        unique_ptr<sq::Texture2D> defrSpec;
        unique_ptr<sq::Texture2D> defrDepth;
        unique_ptr<sq::Texture2D> ssaoA;
        unique_ptr<sq::Texture2D> ssaoB;
        unique_ptr<sq::Texture2D> bloomA;
        unique_ptr<sq::Texture2D> bloomB;
        unique_ptr<sq::Texture2D> hdrColour;
        unique_ptr<sq::Texture2D> basicColour;
        unique_ptr<sq::Texture2D> basicDepth;
    } TX;

    struct {
        float viewDist;
        int shadQlty;
        int shadFltr;
        int ssaoQlty;
        int hdrbQlty;
        int fxaaQlty;
        bool vignette;
        bool mouseFocus;
        uvec2 fullSize;
        uvec2 halfSize;
        uvec2 qterSize;
        vec2 fPixSize;
        vec2 hPixSize;
        vec2 qPixSize;
    } INFO;

    void update_settings();
    void reload_shaders();
};

}
