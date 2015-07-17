#pragma once
#include <sqee/forward.hpp>

#include <sqee/scenes/Scene.hpp>
#include <sqee/handlers/Handler.hpp>

namespace sqt {

class Level;

class GameScene : public sq::Scene {
public:
    GameScene(sq::Application* _app);

    void update();
    void render(float _ft);
    void update_settings();

private:
    unique_ptr<sq::Pipeline> pipeline;
    unique_ptr<sq::UniformBuffer> ubo;
    unique_ptr<sq::Camera> camera;
    unique_ptr<Level> level;

    struct {
        unique_ptr<sq::Shader> object;
    } VERT;

    struct {
        unique_ptr<sq::Shader> object;
    } FRAG;

    struct {
        sq::Mesh* Ball = nullptr;
        sq::Mesh* Floor = nullptr;
        sq::Mesh* HoleA = nullptr;
        sq::Mesh* HoleB = nullptr;
        sq::Mesh* WallA = nullptr;
        sq::Mesh* WallB = nullptr;
        sq::Mesh* WallC = nullptr;
        sq::Mesh* WallD = nullptr;
        sq::Mesh* WallE = nullptr;
        sq::Mesh* WallF = nullptr;
        sq::Mesh* Player = nullptr;
    } MESH;

    int rotation = 0;
    ivec2 position = {0, 0};
    float rotCrnt, rotNext;
    fvec2 posCrnt, posNext;
    bool animate = false;
    bool pushing = false;
};


class GameHandler: public sq::Handler {
public:
    using sq::Handler::Handler;
    bool handle(const sf::Event& event);
};

}
