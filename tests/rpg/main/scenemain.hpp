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
        unique_ptr<sq::Shader> modl_static;
        unique_ptr<sq::Shader> modl_skelly;
        unique_ptr<sq::Shader> shad_static;
        unique_ptr<sq::Shader> shad_skelly;
        unique_ptr<sq::Shader> gnrc_quad;
        unique_ptr<sq::Shader> gnrc_lines;
    } VS;

    struct {
        unique_ptr<sq::Shader> modl_write;
        unique_ptr<sq::Shader> shds_ambient;
        unique_ptr<sq::Shader> shds_skylight;
        unique_ptr<sq::Shader> shds_spotlight;
        unique_ptr<sq::Shader> shds_pointlight;
        unique_ptr<sq::Shader> shds_skylight_shad;
        unique_ptr<sq::Shader> shds_spotlight_shad;
        unique_ptr<sq::Shader> shds_pointlight_shad;
        unique_ptr<sq::Shader> shad_punch;
        unique_ptr<sq::Shader> gnrc_passthru;
        unique_ptr<sq::Shader> gnrc_passthru_layer;
        unique_ptr<sq::Shader> gnrc_lumalpha;
        unique_ptr<sq::Shader> gnrc_tonemap;
        unique_ptr<sq::Shader> prty_fxaa_fxaa_low;
        unique_ptr<sq::Shader> prty_fxaa_fxaa_high;
        unique_ptr<sq::Shader> prty_ssao_ssao_low;
        unique_ptr<sq::Shader> prty_ssao_ssao_high;
        unique_ptr<sq::Shader> prty_ssao_blur;
        unique_ptr<sq::Shader> prty_vignette;
    } FS;

    struct {
        unique_ptr<sq::Framebuffer> defr;
        unique_ptr<sq::Framebuffer> ssaoA;
        unique_ptr<sq::Framebuffer> ssaoB;
        unique_ptr<sq::Framebuffer> hdr;
        unique_ptr<sq::Framebuffer> basic;
    } FB;

    struct {
        unique_ptr<sq::Texture2D> defrDiff;
        unique_ptr<sq::Texture2D> defrNorm;
        unique_ptr<sq::Texture2D> defrSurf;
        unique_ptr<sq::Texture2D> defrSpec;
        unique_ptr<sq::Texture2D> defrDepth;
        unique_ptr<sq::Texture2D> ssaoGreyA;
        unique_ptr<sq::Texture2D> ssaoGreyB;
        unique_ptr<sq::Texture2D> hdrColour;
        unique_ptr<sq::Texture2D> basicColour;
        unique_ptr<sq::Texture2D> basicDepth;
    } TX;

    struct {
        float viewDistance;
        float farDistance;
        int shadQuality;
        int shadFilter;
        int ssaoQuality;
        int fxaaQuality;
        bool mouseFocus;
        bool vignetting;
        uvec2 fullSize;
        uvec2 halfSize;
    } INFO;

    void update_settings();
};

}
