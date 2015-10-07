#include <glm/gtc/matrix_transform.hpp>
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
    ubo->reserve("slDirection", 4);
    ubo->reserve("slColour", 4);
    ubo->reserve("aColour", 4);
    ubo->allocate_storage();

    fvec3 slDirection = {0.f, 0.25f, -1.f};
    fvec3 slColour = {0.7f, 0.7f, 0.7f};
    fvec3 aColour = {0.3f, 0.3f, 0.3f};

    ubo->bind(1u);
    ubo->update("slDirection", &slDirection);
    ubo->update("slColour", &slColour);
    ubo->update("aColour", &aColour);

    pipeline.reset(new sq::Pipeline());
    app->preprocs.import_header("uniform_block");

    /// Create Shaders
    VERT.object.reset(new sq::Shader(gl::VERTEX_SHADER));
    FRAG.object.reset(new sq::Shader(gl::FRAGMENT_SHADER));

    /// Add Uniforms to Shaders
    VERT.object->add_uniform("modelMat"); // mat4
    VERT.object->add_uniform("normMat");  // mat4

    /// Load Shaders
    app->preprocs(*VERT.object, "object_vs");
    app->preprocs(*FRAG.object, "object_fs");

    /// Create and Load Meshes
    MESH.Ball = sq::res::mesh().add("Ball", "Ball");
    MESH.Floor = sq::res::mesh().add("Floor", "Floor");
    MESH.HoleA = sq::res::mesh().add("HoleA", "HoleA");
    MESH.HoleB = sq::res::mesh().add("HoleB", "HoleB");
    MESH.WallA = sq::res::mesh().add("WallA", "WallA");
    MESH.WallB = sq::res::mesh().add("WallB", "WallB");
    MESH.WallC = sq::res::mesh().add("WallC", "WallC");
    MESH.WallD = sq::res::mesh().add("WallD", "WallD");
    MESH.WallE = sq::res::mesh().add("WallE", "WallE");
    MESH.WallF = sq::res::mesh().add("WallF", "WallF");
    MESH.Player = sq::res::mesh().add("Player", "Player");

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


void GameScene::update() {
    const ivec2 offsets[4] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    rotCrnt = rotNext; posCrnt = posNext;
    bool skipTurnMove = false;

    if (pushing == true) {
        Ball* ball = level->get_Ball(position);
        ball->position += offsets[rotation%4];
        ball->pushed = true; pushing = false;
        skipTurnMove = true;
    }

    if (animate == true) {
        rotNext = glm::radians(90.f * rotation);
        posNext = position; animate = false;
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
            rotCrnt = rotNext = glm::radians(90.f * rotation);
        }

        if (turnNeg == true) {
            float rotPrev = glm::radians(90.f * rotation); animate = true;
            rotNext = glm::mix(rotPrev, glm::radians(90.f * --rotation), 0.5f);
        }

        if (turnPos == true) {
            float rotPrev = glm::radians(90.f * rotation); animate = true;
            rotNext = glm::mix(rotPrev, glm::radians(90.f * ++rotation), 0.5f);
        }

        if (movePos == true) {
            ivec2 gridNew = position + offsets[rotation%4];
            if (level->outside(gridNew) == false && level->get_Wall(gridNew) == nullptr && (
                level->get_Hole(gridNew) == nullptr || level->get_Hole(gridNew)->filled == true)) {
                pushing = level->get_Ball(gridNew) != nullptr;

                if (pushing == true) {
                    ivec2 pushedPos = gridNew + offsets[rotation%4];
                    if (level->outside(pushedPos) ||
                        level->get_Ball(pushedPos) ||
                        level->get_Wall(pushedPos)) {
                        pushing = false; gridNew = position;
                    } else {
                        posNext = glm::mix(fvec2(position), fvec2(gridNew), 0.5f);
                        position = gridNew; animate = true;
                    }
                } else {
                    posNext = glm::mix(fvec2(position), fvec2(gridNew), 0.5f);
                    position = gridNew; animate = true;
                }
            }
        }

        if (moveNeg == true) {
            ivec2 gridNew = position - offsets[rotation%4];
            if (level->outside(gridNew) == false && level->get_Wall(gridNew) == nullptr && (
                level->get_Hole(gridNew) == nullptr || level->get_Hole(gridNew)->filled == true)) {
                posNext = glm::mix(fvec2(position), fvec2(gridNew), 0.5f);
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
    pipeline->use_shader(*VERT.object);
    pipeline->use_shader(*FRAG.object);
    camera->ubo->bind(0u);

    for (int x = level->minPos.x; x < level->maxPos.x; x++) {
        for (int y = level->minPos.x; y < level->maxPos.y; y++) {
            if (!level->get_Hole({x, y}) && !level->get_Wall({x, y})) {
                fmat4 modelMat = glm::translate(fmat4(), fvec3(x, y, 0.f));
                fmat3 normMat = sq::make_normMat(camera->viewMat * modelMat);
                VERT.object->set_mat<fmat4>("modelMat", modelMat);
                VERT.object->set_mat<fmat3>("normMat", normMat);
                MESH.Floor->bind_vao(); MESH.Floor->draw_complete();
            }
        }
    }

    for (const Ball::Ptr& ball : level->ballList) {
        float zTrans = glm::mix(0.f, -float(ball->inhole), accum*tickRate);
        fvec2 translation = glm::mix(ball->posCrnt, ball->posNext, accum*tickRate);
        fmat4 modelMat = glm::translate(fmat4(), fvec3(translation, zTrans));
        fmat3 normMat = sq::make_normMat(camera->viewMat * modelMat);
        VERT.object->set_mat<fmat4>("modelMat", modelMat);
        VERT.object->set_mat<fmat3>("normMat", normMat);
        MESH.Ball->bind_vao(); MESH.Ball->draw_complete();
    }

    for (const Hole::Ptr& hole : level->holeList) {
        const sq::Mesh* mesh = hole->filled ? MESH.HoleB : MESH.HoleA;
        fmat4 modelMat = glm::translate(fmat4(), fvec3(hole->position, 0.f));
        fmat3 normMat = sq::make_normMat(camera->viewMat * modelMat);
        VERT.object->set_mat<fmat4>("modelMat", modelMat);
        VERT.object->set_mat<fmat3>("normMat", normMat);
        mesh->bind_vao(); mesh->draw_complete();
    }

    for (const Wall::Ptr& wall : level->wallList) {
        const sq::Mesh* mesh = nullptr;
        if (wall->connect == 0u) mesh = MESH.WallA;
        if (wall->connect == 1u) mesh = MESH.WallB;
        if (wall->connect == 2u) mesh = MESH.WallC;
        if (wall->connect == 3u) mesh = MESH.WallD;
        if (wall->connect == 4u) mesh = MESH.WallE;
        if (wall->connect == 5u) mesh = MESH.WallF;

        float rotation = 45.f;
        if (wall->rotate == 0u) rotation = glm::radians(0.f);
        if (wall->rotate == 1u) rotation = glm::radians(90.f);
        if (wall->rotate == 2u) rotation = glm::radians(180.f);
        if (wall->rotate == 3u) rotation = glm::radians(270.f);

        fmat4 modelMat = glm::translate(fmat4(), fvec3(wall->position, 0.f));
        modelMat = glm::rotate(modelMat, rotation, fvec3(0.f, 0.f, -1.f));
        fmat3 normMat = sq::make_normMat(camera->viewMat * modelMat);
        VERT.object->set_mat<fmat4>("modelMat", modelMat);
        VERT.object->set_mat<fmat3>("normMat", normMat);
        mesh->bind_vao(); mesh->draw_complete();
    }

    { // Player
        float rotation = glm::mix(rotCrnt, rotNext, accum*tickRate);
        fvec2 translation = glm::mix(posCrnt, posNext, accum*tickRate);
        fmat4 modelMat = glm::translate(fmat4(), fvec3(translation, 0.f));
        modelMat = glm::rotate(modelMat, rotation, fvec3(0.f, 0.f, -1.f));
        fmat3 normMat = sq::make_normMat(camera->viewMat * modelMat);
        VERT.object->set_mat<fmat4>("modelMat", modelMat);
        VERT.object->set_mat<fmat3>("normMat", normMat);
        MESH.Player->bind_vao(); MESH.Player->draw_complete();
    }


    gl::BindProgramPipeline(0u);
    gl::BindVertexArray(0u);
}


void GameScene::refresh() {
    camera->size = app->get_size();
}


bool GameHandler::handle(sf::Event _event) {
    GameScene& scene = app->get_scene<GameScene>("game");

    return false;
}
