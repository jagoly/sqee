#include <SFML/Window/Event.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/app/Settings.hpp>
#include <sqee/app/PreProcessor.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/render/Camera.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/maths/General.hpp>

#include "Level.hpp"
#include "Game.hpp"

using namespace sqt;
namespace maths = sq::maths;

GameScene::GameScene(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 10u;

    camera.reset(new sq::Camera());
    camera->pos = {3.f, -2.f, 10.f};
    camera->dir = {0.f, 0.4f, -1.f};
    camera->rmin = 0.5f;
    camera->rmax = 25.f;
    camera->size = {8.f, 6.f};
    camera->fov = 1.f;
    camera->update();

    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("slDirection", 4u);
    ubo->reserve("slColour", 4u);
    ubo->reserve("aColour", 4u);
    ubo->create_and_allocate();

    Vec3F slDirection = {0.f, 0.25f, -1.f};
    Vec3F slColour = {0.7f, 0.7f, 0.7f};
    Vec3F aColour = {0.3f, 0.3f, 0.3f};

    ubo->update("slDirection", &slDirection);
    ubo->update("slColour", &slColour);
    ubo->update("aColour", &aColour);

    pipeline.reset(new sq::Pipeline());
    app->preprocs.import_header("uniform_block");

    /// Create Shaders
    VS_object.reset(new sq::Shader(gl::VERTEX_SHADER));
    FS_object.reset(new sq::Shader(gl::FRAGMENT_SHADER));

    /// Add Uniforms to Shaders
    VS_object->add_uniform("modelMat"); // mat4
    VS_object->add_uniform("normMat");  // mat4

    /// Load Shaders
    app->preprocs.load(*VS_object, "object_vs");
    app->preprocs.load(*FS_object, "object_fs");

    /// Create and Load Meshes
    MESH_Ball.reset(new sq::Mesh("Ball"));
    MESH_Floor.reset(new sq::Mesh("Floor"));
    MESH_HoleA.reset(new sq::Mesh("HoleA"));
    MESH_HoleB.reset(new sq::Mesh("HoleB"));
    MESH_WallA.reset(new sq::Mesh("WallA"));
    MESH_WallB.reset(new sq::Mesh("WallB"));
    MESH_WallC.reset(new sq::Mesh("WallC"));
    MESH_WallD.reset(new sq::Mesh("WallD"));
    MESH_WallE.reset(new sq::Mesh("WallE"));
    MESH_WallF.reset(new sq::Mesh("WallF"));
    MESH_Player.reset(new sq::Mesh("Player"));

    /// Setup Level
    Level::Spec spec;

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
    level.reset(new Level(spec));

    level->update();
}


void GameScene::configure() {
    camera->size = Vec2F(app->get_size());
}


void GameScene::tick() {
    const Vec2I offsets[4] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    rotCrnt = rotNext; posCrnt = posNext;
    bool skipTurnMove = false;

    if (pushing == true) {
        Ball* ball = level->get_Ball(position);
        ball->position += offsets[rotation%4];
        ball->pushed = true; pushing = false;
        skipTurnMove = true;
    }

    if (animate == true) {
        rotNext = maths::radians(0.25f * rotation);
        posNext = Vec2F(position); animate = false;
        skipTurnMove = true;
    }

    if (skipTurnMove == false) {
        bool keyUp = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
        bool keyDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
        bool keyLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
        bool keyRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
        bool movePos = keyUp && !(keyDown || keyLeft || keyRight);
        bool moveNeg = keyDown && !(keyUp || keyLeft || keyRight);
        bool turnNeg = keyLeft && !keyRight;
        bool turnPos = keyRight && !keyLeft;

        if (!turnNeg && !turnPos) {
            rotation = rotation % 4;
            if (rotation < 0) rotation = 4 - rotation * -1;
            rotCrnt = rotNext = maths::radians(0.25f * rotation);
        }

        if (turnNeg == true) {
            float rotPrev = maths::radians(0.25f * rotation); animate = true;
            rotNext = maths::mix(rotPrev, maths::radians(0.25f * --rotation), 0.5f);
        }

        if (turnPos == true) {
            float rotPrev = maths::radians(0.25f * rotation); animate = true;
            rotNext = maths::mix(rotPrev, maths::radians(0.25f * ++rotation), 0.5f);
        }

        if (movePos == true) {
            Vec2I gridNew = position + offsets[rotation%4];
            if (level->outside(gridNew) == false && level->get_Wall(gridNew) == nullptr && (
                level->get_Hole(gridNew) == nullptr || level->get_Hole(gridNew)->filled == true)) {
                pushing = level->get_Ball(gridNew) != nullptr;

                if (pushing == true) {
                    Vec2I pushedPos = gridNew + offsets[rotation%4];
                    if (level->outside(pushedPos) ||
                        level->get_Ball(pushedPos) ||
                        level->get_Wall(pushedPos)) {
                        pushing = false; gridNew = position;
                    } else {
                        posNext = maths::mix(Vec2F(position), Vec2F(gridNew), 0.5f);
                        position = gridNew; animate = true;
                    }
                } else {
                    posNext = maths::mix(Vec2F(position), Vec2F(gridNew), 0.5f);
                    position = gridNew; animate = true;
                }
            }
        }

        if (moveNeg == true) {
            Vec2I gridNew = position - offsets[rotation%4];
            if (level->outside(gridNew) == false && level->get_Wall(gridNew) == nullptr && (
                level->get_Hole(gridNew) == nullptr || level->get_Hole(gridNew)->filled == true)) {
                posNext = maths::mix(Vec2F(position), Vec2F(gridNew), 0.5f);
                position = gridNew; animate = true;
            }
        }
    }

    camera->update();
    level->update();
}


void GameScene::render() {
    sq::VIEWPORT(app->get_size());
    gl::ClearColor(0.15f, 0.1f, 0.2f, 0.f);
    sq::CULLFACE_ON(); sq::DEPTH_ON();
    sq::CLEAR_COLOR_DEPTH_STENC();
    sq::BLEND_OFF();

    pipeline->bind();
    pipeline->use_shader(*VS_object);
    pipeline->use_shader(*FS_object);
    camera->ubo.bind(0u);
    ubo->bind(1u);

    for (int x = level->minPos.x; x < level->maxPos.x; x++) {
        for (int y = level->minPos.x; y < level->maxPos.y; y++) {
            if (!level->get_Hole({x, y}) && !level->get_Wall({x, y})) {
                Mat4F modelMat = maths::translate(Mat4F(), Vec3F(x, y, 0.f));
                Mat3F normMat = sq::make_normMat(camera->viewMat * modelMat);
                VS_object->set_mat<Mat4F>("modelMat", modelMat);
                VS_object->set_mat<Mat3F>("normMat", normMat);
                MESH_Floor->bind_vao(); MESH_Floor->draw_complete();
            }
        }
    }

    for (const Ball::Ptr& ball : level->ballList) {
        float zTrans = -float(ball->inhole) * float(accum*tickRate);
        Vec2F translation = maths::mix(ball->posCrnt, ball->posNext, float(accum*tickRate));
        Mat4F modelMat = maths::translate(Mat4F(), Vec3F(translation, zTrans));
        Mat3F normMat = sq::make_normMat(camera->viewMat * modelMat);
        VS_object->set_mat<Mat4F>("modelMat", modelMat);
        VS_object->set_mat<Mat3F>("normMat", normMat);
        MESH_Ball->bind_vao(); MESH_Ball->draw_complete();
    }

    for (const Hole::Ptr& hole : level->holeList) {
        const auto& mesh = hole->filled ? MESH_HoleB : MESH_HoleA;
        Mat4F modelMat = maths::translate(Mat4F(), Vec3F(Vec2F(hole->position), 0.f));
        Mat3F normMat = sq::make_normMat(camera->viewMat * modelMat);
        VS_object->set_mat<Mat4F>("modelMat", modelMat);
        VS_object->set_mat<Mat3F>("normMat", normMat);
        mesh->bind_vao(); mesh->draw_complete();
    }

    for (const Wall::Ptr& wall : level->wallList) {
        const sq::Mesh* mesh = nullptr;
        if (wall->connect == 0u) mesh = MESH_WallA.get();
        if (wall->connect == 1u) mesh = MESH_WallB.get();
        if (wall->connect == 2u) mesh = MESH_WallC.get();
        if (wall->connect == 3u) mesh = MESH_WallD.get();
        if (wall->connect == 4u) mesh = MESH_WallE.get();
        if (wall->connect == 5u) mesh = MESH_WallF.get();

        float rotation = 45.f;
        if (wall->rotate == 0u) rotation = maths::radians(0.f);
        if (wall->rotate == 1u) rotation = maths::radians(0.25f);
        if (wall->rotate == 2u) rotation = maths::radians(0.5f);
        if (wall->rotate == 3u) rotation = maths::radians(0.75f);

        Mat4F modelMat = maths::translate(Mat4F(), Vec3F(Vec2F(wall->position), 0.f));
        modelMat = maths::rotate(modelMat, Vec3F(0.f, 0.f, -1.f), rotation);
        Mat3F normMat = sq::make_normMat(camera->viewMat * modelMat);
        VS_object->set_mat<Mat4F>("modelMat", modelMat);
        VS_object->set_mat<Mat3F>("normMat", normMat);
        mesh->bind_vao(); mesh->draw_complete();
    }

    { // Player
        float rotation = maths::mix(rotCrnt, rotNext, float(accum*tickRate));
        Vec2F translation = maths::mix(posCrnt, posNext, float(accum*tickRate));
        Mat4F modelMat = maths::translate(Mat4F(), Vec3F(translation, 0.f));
        modelMat = maths::rotate(modelMat, Vec3F(0.f, 0.f, -1.f), rotation);
        Mat3F normMat = sq::make_normMat(camera->viewMat * modelMat);
        VS_object->set_mat<Mat4F>("modelMat", modelMat);
        VS_object->set_mat<Mat3F>("normMat", normMat);
        MESH_Player->bind_vao(); MESH_Player->draw_complete();
    }


    gl::BindProgramPipeline(0u);
    gl::BindVertexArray(0u);
}


bool GameScene::handle(sf::Event _event) {
    return false;
}
