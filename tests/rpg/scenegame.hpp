#pragma once
#include <sqee/forward.hpp>

#include <sqee/events/handler.hpp>
#include <sqee/gl/cameras.hpp>
#include <sqee/scenes/scene.hpp>

#include "maps/world.hpp"
#include "player.hpp"
#include "render/camera.hpp"
#include "render/light.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application& _app);

    void render(float _ft);
    void resize(uvec2 _size);
    void update();

    MainCamera camera;
    World world;
    Player player;

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
        unique_ptr<sq::Framebuffer> hdr;
        unique_ptr<sq::Framebuffer> pri;
        unique_ptr<sq::Framebuffer> sec;
        unique_ptr<sq::Framebuffer> ssao;
        array<unique_ptr<sq::Framebuffer>, 4> skylightArr;
        std::vector<unique_ptr<sq::Framebuffer>> spotlightVec;
    } FB;

    struct {
        unique_ptr<sq::Texture> defrNorm;
        unique_ptr<sq::Texture> defrSurf;
        unique_ptr<sq::Texture> defrDiff;
        unique_ptr<sq::Texture> defrSpec;
        unique_ptr<sq::Texture> defrDep;
        unique_ptr<sq::Texture> hdrFloat;
        unique_ptr<sq::Texture> priCol;
        unique_ptr<sq::Texture> priDep;
        unique_ptr<sq::Texture> secCol;
        unique_ptr<sq::Texture> secDep;
        unique_ptr<sq::Texture> ssao;
        unique_ptr<sq::Texture> skylight;
        std::vector<unique_ptr<sq::Texture>> spotlightVec;
    } TX;

    unique_ptr<sq::Pipeline> pipeLine;

    SkyLight skylight;
    array<SpotLight, 1> spotlightVec;

    void update_settings();
    void draw_skylight();
    void draw_spotlight();
    void draw_pointlight();
};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& event);
};

}
