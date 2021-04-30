#include <sqee/maths/Functions.hpp>

#include <sqee/debug/Logging.hpp>

#include <sqee/gl/Context.hpp>
#include <sqee/gl/Textures.hpp>

#include <sqee/core/Utilities.hpp>

#include "SokoApp.hpp"
#include "Level.hpp"
#include "Game.hpp"

// note from september 2020, after more than five years
// looking at this old sokoban stuff and marvelling at how horrible it is
// looked at git, and even in july 2015 I said: "Code is mega ugly, avert your eyes."
// REALLY should get around to fixing it, if for no other reason to have a simple
// demo application for sqee, making animations actually work would be great too

using namespace sqt;
namespace maths = sq::maths;

GameScene::GameScene(sq::InputDevices& input)
    : Scene(1.0 / 20.0), mInput(input)
{
    ubo.allocate_dynamic(176u);

    ubo.update ( 0u, sq::Structure (
        Mat4F(), Mat4F(),
        Vec3F { 0.f, 0.25f, -1.f }, 0,
        Vec3F { 0.7f, 0.7f, 0.7f }, 0,
        Vec3F { 0.3f, 0.3f, 0.3f }, 0
    ) );

    // Load Shaders
    processor.load_vertex(PROG_Object, "shaders/object_vs.glsl", {});
    processor.load_fragment(PROG_Object, "shaders/object_fs.glsl", {});
    PROG_Object.link_program_stages();

    // Load Meshes
    MESH_Ball.load_from_file("assets/meshes/Ball.sqm");
    MESH_Floor.load_from_file("assets/meshes/Floor.sqm");
    MESH_HoleA.load_from_file("assets/meshes/HoleA.sqm");
    MESH_HoleB.load_from_file("assets/meshes/HoleB.sqm");
    MESH_Player.load_from_file("assets/meshes/Player.sqm");
    MESH_Wall.load_from_file("assets/meshes/Wall.sqm");

    // Setup Level
    Level::Specification spec;

    spec.wallSet.emplace(1u, 1u); spec.wallSet.emplace(2u, 1u);
    spec.wallSet.emplace(3u, 1u); spec.wallSet.emplace(4u, 1u);
    spec.wallSet.emplace(5u, 1u); spec.wallSet.emplace(5u, 2u);
    spec.wallSet.emplace(5u, 3u); spec.wallSet.emplace(5u, 4u);
    spec.wallSet.emplace(5u, 5u); spec.wallSet.emplace(4u, 5u);
    spec.wallSet.emplace(3u, 5u); spec.wallSet.emplace(2u, 5u);
    spec.wallSet.emplace(1u, 5u); spec.wallSet.emplace(1u, 4u);
    spec.wallSet.emplace(1u, 3u); spec.wallSet.emplace(1u, 2u);

    spec.wallSet.emplace(3u, 3u); spec.wallSet.emplace(3u, 4u);
    spec.wallSet.emplace(4u, 3u); spec.wallSet.emplace(3u, 2u);
    spec.wallSet.emplace(2u, 3u);

    spec.ballSet.emplace(1u, 0u); spec.ballSet.emplace(0u, 5u);
    spec.ballSet.emplace(5u, 6u); spec.ballSet.emplace(6u, 1u);
    spec.ballSet.emplace(6u, 4u);

    spec.holeSet.emplace(0u, 0u); spec.holeSet.emplace(0u, 6u);
    spec.holeSet.emplace(6u, 6u); spec.holeSet.emplace(6u, 0u);

    spec.minPos = {-2, -2}; spec.maxPos = {9, 9};
    level = std::make_unique<Level>(spec);

    level->update();
}

GameScene::~GameScene() = default;

//============================================================================//

void GameScene::refresh_options_create()
{
    const Vec2F size = Vec2F(sq::Context::get().get_ViewPort());

    constexpr Vec3F pos = { 3.f, -2.f, 10.f };
    constexpr Vec3F dir = { 0.f, 0.4f, -1.f };

    viewMat = maths::look_at_RH(pos, pos + dir, Vec3F(0.f, 0.f, 1.f));
    projMat = maths::perspective_RH(1.f, size.x / size.y, 0.5f, 25.f);

    ubo.update(0u, viewMat);
    ubo.update(64u, projMat);
}

//============================================================================//

void GameScene::update()
{
    const Vec2I offsets[4] = {{-1, 0}, {+1, 0}, {0, -1}, {0, +1}};

    //--------------------------------------------------------//

    posCrnt = posNext;

    if (pushedBall != nullptr)
    {
        pushedBall->posCrnt = pushedBall->posNext;
    }

    //--------------------------------------------------------//

    if (moveProgress < 10u)
    {
        const float progress = float(moveProgress) / 10.f;

        posNext = maths::mix(Vec2F(position), Vec2F(position + moveOffset), progress);

        if (pushedBall != nullptr)
        {
            pushedBall->posNext = maths::mix ( Vec2F(pushedBall->position),
                                               Vec2F(pushedBall->position + moveOffset),
                                               progress );
        }

        moveProgress += 1u;
    }

    //--------------------------------------------------------//

    if (moveProgress == 10u)
    {
        position += moveOffset;

        if (pushedBall != nullptr)
        {
            pushedBall->position += moveOffset;
            pushedBall = nullptr;
        }

        moveOffset = { 0, 0 };

        //----------------------------------------------------//

        bool keyLeft = mInput.is_pressed(sq::Keyboard_Key::Arrow_Left);
        bool keyRight = mInput.is_pressed(sq::Keyboard_Key::Arrow_Right);
        bool keyDown = mInput.is_pressed(sq::Keyboard_Key::Arrow_Down);
        bool keyUp = mInput.is_pressed(sq::Keyboard_Key::Arrow_Up);

        bool moveLeft = keyLeft && !keyRight && !keyDown && !keyUp;
        bool moveRight = !keyLeft && keyRight && !keyDown && !keyUp;
        bool moveDown = !keyLeft && !keyRight && keyDown && !keyUp;
        bool moveUp = !keyLeft && !keyRight && !keyDown && keyUp;

        if (moveLeft) rotation = 0;
        if (moveRight) rotation = 1;
        if (moveDown) rotation = 2;
        if (moveUp) rotation = 3;

        if (moveLeft || moveRight || moveDown || moveUp)
        {
            Vec2I targetPos = position + offsets[rotation];

            if ( level->outside(targetPos) == false && level->get_wall(targetPos) == nullptr &&
                 ( level->get_hole(targetPos) == nullptr || level->get_hole(targetPos)->complete == true ) )
            {
                moveProgress = 0u;

                moveOffset = offsets[rotation];

                pushedBall = level->get_ball(position + moveOffset);

                if (pushedBall != nullptr)
                {
                    Vec2I pushedPos = targetPos + offsets[rotation];

                    if (level->outside(pushedPos) || level->get_ball(pushedPos) || level->get_wall(pushedPos))
                    {
                        moveProgress = 10u;
                        moveOffset = {};
                        pushedBall = nullptr;
                    }
                }
            }
        }
    }

    level->update();
}


void GameScene::render(double)
{
    auto& context = sq::Context::get();

    context.set_state(sq::CullFace::Back);
    context.set_state(sq::DepthTest::Replace);
    context.set_state(sq::StencilTest::Disable);
    context.set_state(sq::BlendMode::Disable);

    context.clear_depth_stencil_colour(1.0, 0x00, 0xFF, {0.15f, 0.1f, 0.2f, 0.f});

    context.bind_program(PROG_Object);
    context.bind_buffer(ubo, sq::BufTarget::Uniform, 0u);

    float tickPercent = float(mAccumulation) * 20.f;

    for (int x = level->minPos.x; x < level->maxPos.x; x++)
    {
        for (int y = level->minPos.x; y < level->maxPos.y; y++)
        {
            if (!level->get_hole({x, y}) && !level->get_wall({x, y}))
            {
                Mat4F modelMat = maths::translate(Mat4F(), Vec3F(x, y, 0.f));
                Mat3F normMat = maths::normal_matrix(viewMat * modelMat);
                PROG_Object.update(0, modelMat);
                PROG_Object.update(1, normMat);
                MESH_Floor.apply_to_context(context);
                MESH_Floor.draw_complete(context);
            }
        }
    }

    for (const Ball& ball : level->ballList)
    {
        float zTrans = -float(ball.complete) * tickPercent;
        Vec2F translation = maths::mix(ball.posCrnt, ball.posNext, tickPercent);
        Mat4F modelMat = maths::translate(Mat4F(), Vec3F(translation, zTrans));
        Mat3F normMat = maths::normal_matrix(viewMat * modelMat);
        PROG_Object.update(0, modelMat);
        PROG_Object.update(1, normMat);
        MESH_Ball.apply_to_context(context);
        MESH_Ball.draw_complete(context);
    }

    for (const Hole& hole : level->holeList)
    {
        const auto& mesh = hole.complete ? MESH_HoleB : MESH_HoleA;
        Mat4F modelMat = maths::translate(Mat4F(), Vec3F(Vec2F(hole.position), 0.f));
        Mat3F normMat = maths::normal_matrix(viewMat * modelMat);
        PROG_Object.update(0, modelMat);
        PROG_Object.update(1, normMat);
        mesh.apply_to_context(context);
        mesh.draw_complete(context);
    }

    for (const Wall& wall : level->wallList)
    {
        Mat4F modelMat = maths::translate(Mat4F(), Vec3F(Vec2F(wall.position), 0.f));
        Mat3F normMat = maths::normal_matrix(viewMat * modelMat);
        PROG_Object.update(0, modelMat);
        PROG_Object.update(1, normMat);
        MESH_Wall.apply_to_context(context);
        MESH_Wall.draw_complete(context);
    }

    { // Player
        float rotation = 0.f;
        Vec2F translation = maths::mix(posCrnt, posNext, tickPercent);
        Mat4F modelMat = maths::transform(Vec3F(translation, 0.f), QuatF(0.f, 0.f, rotation), Vec3F(1.f));
        Mat3F normMat = maths::normal_matrix(viewMat * modelMat);
        PROG_Object.update(0, modelMat);
        PROG_Object.update(1, normMat);
        MESH_Player.apply_to_context(context);
        MESH_Player.draw_complete(context);
    }
}


void GameScene::handle_event(sq::Event /*event*/)
{
}
