#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/cameras.hpp>
#include <sqee/scenes/scene.hpp>
#include <sqee/events/handler.hpp>

#include "player.hpp"
#include "maps/world.hpp"

namespace sqt {

class SceneGame : public sq::Scene {
public:
    SceneGame(sq::Application& _app);

    void render(float _ft);
    void resize(glm::uvec2 _size);
    void update();

    sq::Camera camera;
    World world;
    Player player;

private:
    bool updateFramebuffers = true;

    struct {
        unique_ptr<sq::Shader> modl_static;
        unique_ptr<sq::Shader> modl_skelly;
        unique_ptr<sq::Shader> shad_static;
        unique_ptr<sq::Shader> shad_skelly;
        unique_ptr<sq::Shader> shad_static_punch;
        unique_ptr<sq::Shader> shad_skelly_punch;
        unique_ptr<sq::Shader> liqd_surface;
        unique_ptr<sq::Shader> gnrc_quad;
        unique_ptr<sq::Shader> dbug_lines;
    } VS;

    struct {
        unique_ptr<sq::Shader> modl_frag;
        unique_ptr<sq::Shader> shad_punch;
        unique_ptr<sq::Shader> liqd_surface;
        unique_ptr<sq::Shader> gnrc_luma;
        unique_ptr<sq::Shader> gnrc_passthru;
        unique_ptr<sq::Shader> gnrc_passthru_layer;
        unique_ptr<sq::Shader> post_fxaa_low;
        unique_ptr<sq::Shader> post_fxaa_high;
    } FS;

    struct {
        unique_ptr<sq::Framebuffer> pri;
        unique_ptr<sq::Framebuffer> sec;
        unique_ptr<sq::Framebuffer> ter;
        array<unique_ptr<sq::Framebuffer>, 6> skyl;
        array<array<unique_ptr<sq::Framebuffer>, 4>, 8> spotArr;
    } FB;

    struct {
        unique_ptr<sq::Texture> cPri;
        unique_ptr<sq::Texture> dPri;
        unique_ptr<sq::Texture> cSec;
        unique_ptr<sq::Texture> dSec;
        unique_ptr<sq::Texture> cTer;
        unique_ptr<sq::Texture> dTer;
        unique_ptr<sq::Texture> dSkyl;
        array<unique_ptr<sq::Texture>, 8> dSpotArr;
    } TX;

};

class HandlerGame : public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& event);
};

}
