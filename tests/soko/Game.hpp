#pragma once

#include <sqee/forward.hpp>
#include <sqee/builtins.hpp>
#include <sqee/scenes/Scene.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

class Level;

class GameScene : public sq::Scene {
public:
    GameScene(sq::Application* _app);

    void configure();
    void tick(), render();

    bool handle(sf::Event event);

private:
    unique_ptr<sq::Pipeline> pipeline;
    unique_ptr<sq::UniformBuffer> ubo;
    unique_ptr<sq::Camera> camera;
    unique_ptr<Level> level;

    unique_ptr<sq::Shader> VS_object;
    unique_ptr<sq::Shader> FS_object;

    unique_ptr<sq::Mesh> MESH_Ball;
    unique_ptr<sq::Mesh> MESH_Floor;
    unique_ptr<sq::Mesh> MESH_HoleA;
    unique_ptr<sq::Mesh> MESH_HoleB;
    unique_ptr<sq::Mesh> MESH_WallA;
    unique_ptr<sq::Mesh> MESH_WallB;
    unique_ptr<sq::Mesh> MESH_WallC;
    unique_ptr<sq::Mesh> MESH_WallD;
    unique_ptr<sq::Mesh> MESH_WallE;
    unique_ptr<sq::Mesh> MESH_WallF;
    unique_ptr<sq::Mesh> MESH_Player;

    int rotation = 0;
    Vec2I position = {0, 0};
    float rotCrnt, rotNext;
    Vec2F posCrnt, posNext;
    bool animate = false;
    bool pushing = false;
};

}
