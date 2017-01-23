#pragma once

#include <sqee/builtins.hpp>
#include <sqee/app/Scene.hpp>
#include <sqee/maths/Vectors.hpp>

#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Shaders.hpp>

#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>

namespace sqt {

class SokoApp; class Level;

class GameScene : public sq::Scene {
public:
    GameScene(SokoApp& _app);

    void update_options();
    void tick(); void render();
    bool handle(sf::Event event);

private:
    unique_ptr<sq::PreProcessor> preprocs;
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

    SokoApp& app;
};

}
