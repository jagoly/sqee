#include <sqee/gl/gl.hpp>
#include <sqee/models/mesh.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/misc/files.hpp>

#include "scenegame.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 24;

    camera.init({0, 0, 0}, {0, 0, -1}, {0.1f, 100.f}, {4, 3}, 1.f);
    camera.update_projMat();
    camera.update_viewMat();
}

void SceneGame::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        translation.y += 0.02f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        translation.x += 0.02f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        translation.y -= 0.02f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        translation.x -= 0.02f;
    }

    translation.z = -2.f;
}

void SceneGame::render(float) {
    static bool first = true;

    static sq::Mesh mesh;

    static sq::Shader vs1, fs1, fs2;
    static sq::Pipeline pl1;
    static sq::Texture texDiff;
    static sq::Texture texNorm;



    if (first) {
        gl::Enable(gl::DEPTH_TEST);
        gl::ClearColor(0.3f, 0.3f, 0.5f, 1.f);

        vs1.load("vert1_vs", gl::VERTEX_SHADER);
        fs1.load("frag1_fs", gl::FRAGMENT_SHADER);
        fs2.load("frag2_fs", gl::FRAGMENT_SHADER);

        vs1.add_uniform("viewMat"); // mat4
        vs1.add_uniform("projMat"); // mat4
        vs1.add_uniform("normMat"); // mat3
        vs1.add_uniform("translation"); // vec3

        fs1.add_uniform("viewMat"); // mat4
        fs1.add_uniform("lightDir"); // vec3
        fs1.add_uniform("lightColour"); // vec3

        fs2.add_uniform("viewMat"); // mat4
        fs2.add_uniform("lightDir"); // vec3
        fs2.add_uniform("lightColour"); // vec3

        pl1.use_shader(vs1);
        pl1.use_shader(fs1);

        vs1.set_glmmat("viewMat", camera.viewMat);
        vs1.set_glmmat("projMat", camera.projMat);
        fs1.set_glmmat("viewMat", camera.viewMat);
        fs1.set_glmvec("lightDir", glm::vec3(1.f, 0.f, 0.f));
        fs1.set_glmvec("lightColour", glm::vec3(1.f, 1.f, 1.f));
        fs2.set_glmvec("lightDir", glm::vec3(1.f, 0.f, 0.f));
        fs2.set_glmvec("lightColour", glm::vec3(1.f, 1.f, 1.f));

        mesh.load("Dice");
        texDiff.create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, "diff", sq::Texture::Preset::L_C);
        texNorm.create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, "norm", sq::Texture::Preset::L_C);
        first = false;
    }

    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

    pl1.bind();
    vs1.set_glmvec("translation", translation);

    texDiff.bind(gl::TEXTURE0);
    texNorm.bind(gl::TEXTURE1);
    mesh.bind_vao();
    mesh.draw_ibo();
}

bool HandlerGame::handle(const sf::Event&) {
    return false;
}
