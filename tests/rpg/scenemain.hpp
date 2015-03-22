#pragma once
#include <sqee/forward.hpp>

#include <vector>

#include <sqee/events/handler.hpp>
#include <sqee/scenes/scene.hpp>

namespace sqt {

class MainCamera;
class Player;
class World;
class SkyLight;
class SpotLight;

class SceneMain : public sq::Scene {
public:
    SceneMain(sq::Application& _app);

    void render(float _ft);
    void resize(uvec2 _size);
    void update();

    unique_ptr<MainCamera> camera;
    unique_ptr<Player> player;
    unique_ptr<World> world;
    unique_ptr<sq::Pipeline> pipeLine;

private:
    struct {
        unique_ptr<sq::Shader> modl_static;
        unique_ptr<sq::Shader> modl_skelly;
        unique_ptr<sq::Shader> shad_static;
        unique_ptr<sq::Shader> shad_skelly;
        unique_ptr<sq::Shader> shad_static_punch;
        unique_ptr<sq::Shader> shad_skelly_punch;
        unique_ptr<sq::Shader> liqd_surface;
        unique_ptr<sq::Shader> gnrc_quad;
        unique_ptr<sq::Shader> gnrc_lines;
    } VS;

    struct {
        unique_ptr<sq::Shader> modl_write;
        unique_ptr<sq::Shader> shds_ambient;
        unique_ptr<sq::Shader> shds_skylight;
        unique_ptr<sq::Shader> shds_spotlight;
        unique_ptr<sq::Shader> shad_punch;
        unique_ptr<sq::Shader> liqd_surface;
        unique_ptr<sq::Shader> gnrc_passthru;
        unique_ptr<sq::Shader> gnrc_passthru_layer;
        unique_ptr<sq::Shader> gnrc_lumalpha;
        unique_ptr<sq::Shader> gnrc_tonemap;
        unique_ptr<sq::Shader> prty_fxaa_fxaa_low;
        unique_ptr<sq::Shader> prty_fxaa_fxaa_high;
        unique_ptr<sq::Shader> prty_ssao_ssao_low;
        unique_ptr<sq::Shader> prty_ssao_ssao_high;
        unique_ptr<sq::Shader> prty_ssao_blur;
    } FS;

    struct {
        unique_ptr<sq::Framebuffer> defr;
        unique_ptr<sq::Framebuffer> ssaoA;
        unique_ptr<sq::Framebuffer> ssaoB;
        unique_ptr<sq::Framebuffer> hdr;
        unique_ptr<sq::Framebuffer> basic;
        array<unique_ptr<sq::Framebuffer>, 4> skyArr;
        std::vector<unique_ptr<sq::Framebuffer>> spotVec;
    } FB;

    struct {
        unique_ptr<sq::Texture> defrNorm;
        unique_ptr<sq::Texture> defrSurf;
        unique_ptr<sq::Texture> defrDiff;
        unique_ptr<sq::Texture> defrSpec;
        unique_ptr<sq::Texture> defrDepth;
        unique_ptr<sq::Texture> ssaoGreyA;
        unique_ptr<sq::Texture> ssaoGreyB;
        unique_ptr<sq::Texture> hdrColour;
        unique_ptr<sq::Texture> basicColour;
        unique_ptr<sq::Texture> basicDepth;
        unique_ptr<sq::Texture> skyDepth;
        std::vector<unique_ptr<sq::Texture>> spotDepthVec;
    } TX;

    struct {
        float viewDistance;
        float farDistance;
        int shadQuality;
        int shadFilter;
        int ssaoQuality;
        int fxaaQuality;
        bool mouseFocus;
        uvec2 fullSize;
        uvec2 halfSize;
    } INFO;

    unique_ptr<SkyLight> skylight;
    std::vector<unique_ptr<SpotLight>> spotlightVec;

    void update_settings();
    void draw_skylight();
    void draw_spotlight();
};

class HandlerMain : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& event);
};

}
