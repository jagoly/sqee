#pragma once

#include <sqee/app/Scene.hpp>
#include <sqee/app/InputDevices.hpp>

#include <sqee/app/Event.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Program.hpp>

#include <sqee/objects/Mesh.hpp>

#include "Level.hpp"

namespace sqt {

class GameScene final : public sq::Scene
{
public: //====================================================//

    GameScene(sq::InputDevices& input);

    ~GameScene();

    //--------------------------------------------------------//

    void refresh_options() override;

    void handle_event(sq::Event event) override;

private: //===================================================//

    sq::InputDevices& mInput;

    //--------------------------------------------------------//

    void update() override;

    void render(double elapsed) override;

    //--------------------------------------------------------//

    sq::PreProcessor processor;

    sq::UniformBuffer ubo;

    std::unique_ptr<Level> level;

    sq::Program PROG_Object;

    sq::Mesh MESH_Ball;
    sq::Mesh MESH_Floor;
    sq::Mesh MESH_HoleA;
    sq::Mesh MESH_HoleB;
    sq::Mesh MESH_Player;
    sq::Mesh MESH_Wall;

    //bool moving = false;

    int rotation = 0;
    Vec2I position = { 0, 0 };
    Vec2F posCrnt, posNext;
//    bool animate = false;
//    bool pushing = false;

    Ball* pushedBall = nullptr;

    uint moveProgress = 10u;
    Vec2I moveOffset = { 0, 0 };

    Mat4F viewMat;
    Mat4F projMat;
};

}
