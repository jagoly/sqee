#include "scenegame.hpp"

#include <iostream>
#include <random>

#include <libsqee/app/application.hpp>
#include <libsqee/gl/shaders.hpp>
#include <libsqee/gl/textures.hpp>

#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;

    level.load_map("res/maps/new");

    camera = sq::Camera({2.5f, -1.5f, 8.5f}, 0.375f, 0.f, 16, 9, 1.17f, 0.1f, 64.f);
    //camera = sq::Camera({2.5f, 2.5f, 8.5f}, 0.f, 0.f, 16, 9, 1.17f, 0.1f, 64.f);
    camera.update_projMat();
    camera.update_viewMat();
    camera.update_projViewMat();
}

void SceneGame::resize(glm::uvec2 _size) {
    updateFramebuffers = true;
}

void SceneGame::update() {
    int NS = 0;
    int EW = 0;
    bool update = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { EW += 1; update = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  { EW -= 1; update = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    { NS += 1; update = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  { NS -= 1; update = true; }

    if (update) {
        camera.pos.x += EW * 0.02f;
        camera.pos.y += NS * 0.02f;
        camera.update_projMat();
        camera.update_viewMat();
        camera.update_projViewMat();

        updateCamera = true;
    }
}

void SceneGame::render(float) {
    static bool first = true;

    static std::minstd_rand gen(std::time(NULL)); gen();

    static GLuint mainFb;
    static sq::Texture2D mainFbTexNorm, mainFbTexDiff, mainFbTexSpec;
    static sq::TextureDepth mainFbTexDepth;
    static GLenum mainFbDrawBuffers[] = {
        gl::COLOR_ATTACHMENT0, gl::COLOR_ATTACHMENT1,
        gl::COLOR_ATTACHMENT2
    };

    static GLuint ssaoFb;
    static sq::Texture2D ssaoFbTex;
    static GLenum ssaoFbDrawBuffers[] = {
        gl::COLOR_ATTACHMENT0
    };

    static GLfloat screenQuadPoints[] = {
        -1.0, -1.0,  1.0,  -1.0,  1.0, 1.0,
        1.0, 1.0,    -1.0, 1.0,   -1.0, -1.0
    };
    static GLfloat screenQuadTexcoords[] = {
        0.0, 0.0,  1.0, 0.0,  1.0, 1.0,
        1.0, 1.0,  0.0, 1.0,  0.0, 0.0
    };
    static GLuint screenQuadVao;

    static sq::Shader terrainProg;
    static sq::Shader mapModelProg;
    static sq::Shader ssaoProg;
    static sq::Shader constructProg;

    if (first) {
        gl::DepthFunc(gl::LEQUAL);
        gl::ClearColor(0.4f, 0.2f, 0.6f, 1.f);

        level.load_ground();
        level.load_models();

        // Main Framebuffer
        gl::GenFramebuffers(1, &mainFb);
        mainFbTexNorm.bind();
        mainFbTexNorm.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_NEAREST);
        mainFbTexNorm.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        mainFbTexDiff.bind();
        mainFbTexDiff.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_NEAREST);
        mainFbTexDiff.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        mainFbTexSpec.bind();
        mainFbTexSpec.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_NEAREST);
        mainFbTexSpec.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
        mainFbTexDepth.bind();
        mainFbTexDepth.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_NEAREST);
        mainFbTexDepth.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);

        // SSAO Framebuffer
        gl::GenFramebuffers(1, &ssaoFb);
        ssaoFbTex.bind();
        ssaoFbTex.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_NEAREST);
        ssaoFbTex.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);

        GLuint vboPoints, vboTexcoords;

        gl::GenBuffers(1, &vboPoints);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboPoints);
        gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(GLfloat), screenQuadPoints, gl::STATIC_DRAW);

        gl::GenBuffers(1, &vboTexcoords);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboTexcoords);
        gl::BufferData(gl::ARRAY_BUFFER, 12 * sizeof(GLfloat), screenQuadTexcoords, gl::STATIC_DRAW);

        gl::GenVertexArrays(1, &screenQuadVao);
        gl::BindVertexArray(screenQuadVao);
        gl::EnableVertexAttribArray(0);
        gl::EnableVertexAttribArray(1);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboPoints);
        gl::VertexAttribPointer(0, 2, gl::FLOAT, false, 0, NULL);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboTexcoords);
        gl::VertexAttribPointer(1, 2, gl::FLOAT, false, 0, NULL);

        terrainProg.load_from_file("res/shaders/terrain_vs.glsl", gl::VERTEX_SHADER);
        terrainProg.load_from_file("res/shaders/terrain_fs.glsl", gl::FRAGMENT_SHADER);
        terrainProg.build();
        mapModelProg.load_from_file("res/shaders/mapmodel_vs.glsl", gl::VERTEX_SHADER);
        mapModelProg.load_from_file("res/shaders/mapmodel_fs.glsl", gl::FRAGMENT_SHADER);
        mapModelProg.build();
        ssaoProg.load_from_file("res/shaders/ssao_vs.glsl", gl::VERTEX_SHADER);
        ssaoProg.load_from_file("res/shaders/ssao_fs.glsl", gl::FRAGMENT_SHADER);
        ssaoProg.build();
        constructProg.load_from_file("res/shaders/construct_vs.glsl", gl::VERTEX_SHADER);
        constructProg.load_from_file("res/shaders/construct_fs.glsl", gl::FRAGMENT_SHADER);
        constructProg.build();

        terrainProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        terrainProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        terrainProg.add_uniform("texNormArray", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texDiffArray", sq::U_TYPE::u_1i);
        terrainProg.add_uniform("texSpecArray", sq::U_TYPE::u_1i);

        mapModelProg.add_uniform("projMat", sq::U_TYPE::u_4m);
        mapModelProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        mapModelProg.add_uniform("modelMat", sq::U_TYPE::u_4m);
        mapModelProg.add_uniform("texNorm", sq::U_TYPE::u_1i);
        mapModelProg.add_uniform("texDiff", sq::U_TYPE::u_1i);
        mapModelProg.add_uniform("texSpec", sq::U_TYPE::u_1i);

        ssaoProg.add_uniform("invProjMat", sq::U_TYPE::u_4m);
        ssaoProg.add_uniform("texNorm", sq::U_TYPE::u_1i);
        ssaoProg.add_uniform("texDepth", sq::U_TYPE::u_1i);

        constructProg.add_uniform("invProjMat", sq::U_TYPE::u_4m);
        constructProg.add_uniform("viewMat", sq::U_TYPE::u_4m);
        constructProg.add_uniform("camPos", sq::U_TYPE::u_3f);
        constructProg.add_uniform("skyLightDir", sq::U_TYPE::u_3f);
        constructProg.add_uniform("skyLightDiff", sq::U_TYPE::u_3f);
        constructProg.add_uniform("skyLightAmbi", sq::U_TYPE::u_3f);
        constructProg.add_uniform("skyLightSpec", sq::U_TYPE::u_3f);
        constructProg.add_uniform("texNorm", sq::U_TYPE::u_1i);
        constructProg.add_uniform("texDiff", sq::U_TYPE::u_1i);
        constructProg.add_uniform("texAmbi", sq::U_TYPE::u_1i);
        constructProg.add_uniform("texSpec", sq::U_TYPE::u_1i);
        constructProg.add_uniform("texDepth", sq::U_TYPE::u_1i);

        terrainProg.use();
        terrainProg.set_uniform_i("texNormArray", 0);
        terrainProg.set_uniform_i("texDiffArray", 1);
        terrainProg.set_uniform_i("texSpecArray", 2);

        mapModelProg.use();
        mapModelProg.set_uniform_i("texNorm", 0);
        mapModelProg.set_uniform_i("texDiff", 1);
        mapModelProg.set_uniform_i("texSpec", 2);

        ssaoProg.use();
        ssaoProg.set_uniform_i("texNorm", 0);
        ssaoProg.set_uniform_i("texDepth", 1);

        constructProg.use();
        constructProg.set_uniform_i("texNorm", 0);
        constructProg.set_uniform_i("texDiff", 1);
        constructProg.set_uniform_i("texAmbi", 2);
        constructProg.set_uniform_i("texSpec", 3);
        constructProg.set_uniform_i("texDepth", 4);

        gl::BindBuffer(gl::ARRAY_BUFFER, 0);
        gl::UseProgram(0);

        first = false;
    }

    if (updateFramebuffers) {
        glm::uvec2 size = app->get_size();
        // Main Framebuffer
        {
        gl::BindFramebuffer(gl::FRAMEBUFFER, mainFb);

        mainFbTexNorm.load_blank(size, gl::RGB16F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0,
                                 gl::TEXTURE_2D, mainFbTexNorm.tex, 0);

        mainFbTexDiff.load_blank(size, gl::RGBA16F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT1,
                                 gl::TEXTURE_2D, mainFbTexDiff.tex, 0);

        mainFbTexSpec.load_blank(size, gl::RGB16F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT2,
                                 gl::TEXTURE_2D, mainFbTexSpec.tex, 0);

        mainFbTexDepth.load_blank(size, gl::DEPTH_COMPONENT32F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::DEPTH_ATTACHMENT,
                                 gl::TEXTURE_2D, mainFbTexDepth.tex, 0);

        if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE) {
            std::cout << "Primary FBO Error" << std::endl;
        }
        }

        // SSAO Framebuffer
        {
        gl::BindFramebuffer(gl::FRAMEBUFFER, ssaoFb);

        ssaoFbTex.load_blank(size, gl::RGB16F);
        gl::FramebufferTexture2D(gl::FRAMEBUFFER, gl::COLOR_ATTACHMENT0,
                               gl::TEXTURE_2D, ssaoFbTex.tex, 0);

        if (gl::CheckFramebufferStatus(gl::FRAMEBUFFER) != gl::FRAMEBUFFER_COMPLETE) {
            std::cout << "SSAO FBO Error" << std::endl;
        }
        }

        updateFramebuffers = false;
    }

    if (updateSkyLight) {
        constructProg.use();
        constructProg.set_uniform_fv("skyLightDir", glm::value_ptr(skyLight.dir));
        constructProg.set_uniform_fv("skyLightDiff", glm::value_ptr(skyLight.diff));
        constructProg.set_uniform_fv("skyLightAmbi", glm::value_ptr(skyLight.ambi));
        constructProg.set_uniform_fv("skyLightSpec", glm::value_ptr(skyLight.spec));

        updateSkyLight = false;
    }

    if (updateCamera) {
        terrainProg.use();
        terrainProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
        terrainProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));

        mapModelProg.use();
        mapModelProg.set_uniform_mv("projMat", false, glm::value_ptr(camera.projMat));
        mapModelProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));

        ssaoProg.use();
        ssaoProg.set_uniform_mv("invProjMat", false, glm::value_ptr(glm::inverse(camera.projMat)));

        constructProg.use();
        constructProg.set_uniform_mv("invProjMat", false, glm::value_ptr(glm::inverse(camera.projMat)));
        constructProg.set_uniform_mv("viewMat", false, glm::value_ptr(camera.viewMat));
        constructProg.set_uniform_fv("camPos", glm::value_ptr(camera.pos));

        updateCamera = false;
    }

    gl::BindFramebuffer(gl::FRAMEBUFFER, mainFb);
    gl::DrawBuffers(3, mainFbDrawBuffers);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);


    /// Map Models ///
    gl::Enable(gl::DEPTH_TEST);

    mapModelProg.use();
    for (int i = 0; i < level.modelVec.size(); i++) {
        level.modelVec[i].texNorm.bind(gl::TEXTURE0);
        level.modelVec[i].texDiff.bind(gl::TEXTURE1);
        level.modelVec[i].texSpec.bind(gl::TEXTURE2);
        gl::BindVertexArray(level.modelVec[i].vao);
        for (ModelInstance& m : level.modelInstVec) {
            if (m.index == i) {
                mapModelProg.set_uniform_mv("modelMat", false, glm::value_ptr(m.modelMat));
                gl::DrawArrays(gl::TRIANGLES, 0, level.modelVec[m.index].vCount);
            }
        }
    }


    /// Ground ///

    terrainProg.use();

    // Textures
    level.terrain.texNormArray.bind(gl::TEXTURE0);
    level.terrain.texDiffArray.bind(gl::TEXTURE1);
    level.terrain.texSpecArray.bind(gl::TEXTURE2);

    // Draw
    gl::BindVertexArray(level.terrain.vao);
    gl::DrawArrays(gl::TRIANGLES, 0, level.terrain.vCount);


    /// SSAO ///
    gl::Disable(gl::DEPTH_TEST);

    ssaoProg.use();
    gl::Viewport(0, 0, 1280, 720);
    gl::BindFramebuffer(gl::FRAMEBUFFER, ssaoFb);
    gl::DrawBuffers(1, ssaoFbDrawBuffers);
    gl::Clear(gl::COLOR_BUFFER_BIT);

    gl::BindVertexArray(screenQuadVao);
    mainFbTexNorm.bind(gl::TEXTURE0);
    mainFbTexDepth.bind(gl::TEXTURE1);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);


    /// Construct ///
    gl::Disable(gl::BLEND);

    constructProg.use();
    gl::Viewport(0, 0, 1280, 720);
    gl::BindFramebuffer(gl::FRAMEBUFFER, 0);
    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

    gl::BindVertexArray(screenQuadVao);
    mainFbTexNorm.bind(gl::TEXTURE0);
    mainFbTexDiff.bind(gl::TEXTURE1);
    ssaoFbTex.bind(gl::TEXTURE2);
    mainFbTexSpec.bind(gl::TEXTURE3);
    mainFbTexDepth.bind(gl::TEXTURE4);
    gl::DrawArrays(gl::TRIANGLES, 0, 6);
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

}
