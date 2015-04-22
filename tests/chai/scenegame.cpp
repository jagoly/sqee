#include <glm/geometric.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/shaders.hpp>
#include <sqee/misc/files.hpp>
#include <sqee/app/application.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/text/text.hpp>

#include "scenegame.hpp"

using namespace sqt;

SceneGame::SceneGame(sq::Application& _app) : sq::Scene(_app) {
    tickRate = 60;

    camera.pos = {0, 0, 0};
    camera.dir = {0, 0, -1};
    camera.range = {0.1f, 100.f};
    camera.size = {4, 3};
    camera.fov = 1.f;
    camera.update();
}

void SceneGame::update() {
    static glm::vec3 prevdir, tdir, dir;
    static glm::vec3 target;

    static bool state = false;
    static int side = 0;
    static uint cnt = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) != state) {
        state = !state;
        if (state) { side++;
            if (side == 4) side = 0;
            if (side == 0) target = {0.f, 1.f, -4.f};
            if (side == 1) target = {1.f, 0.f, -4.f};
            if (side == 2) target = {0.f, -1.f, -4.f};
            if (side == 3) target = {-1.f, 0.f, -4.f};

            prevdir = dir;
            cnt = 0;
        }
    }

    if (glm::distance(target, translation) > 0.025f) {
        tdir = glm::normalize(target - translation);
        dir = cnt < 30 ? glm::mix(prevdir, tdir, float(cnt++) / 30.f) : tdir;
        translation += dir * 0.05f;
    } else {
        translation = target;
        dir = {0, 0, 0};
    }
}

void SceneGame::render(float) {   
    static sq::Mesh mesh;

    static sq::Shader vert(gl::VERTEX_SHADER);
    static sq::Shader frag(gl::FRAGMENT_SHADER);
    static sq::Pipeline pipe;

    static bool first = true;
    if (first) { first = false;
        gl::Enable(gl::DEPTH_TEST);
        gl::ClearColor(0.3f, 0.3f, 0.5f, 1.f);

        vert.load(app.preproc("vert1_vs"));
        frag.load(app.preproc("frag1_fs"));

        vert.add_uniform("viewMat"); // mat4
        vert.add_uniform("projMat"); // mat4
        vert.add_uniform("translation"); // vec3

        pipe.use_shader(vert);
        pipe.use_shader(frag);

        vert.set_mat<mat4>("viewMat", camera.viewMat);
        vert.set_mat<mat4>("projMat", camera.projMat);

        mesh.create("Dice");
    }

    gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);

    pipe.bind();
    vert.set_vec("translation", translation);

    mesh.bind_vao();
    mesh.draw_ibo(0);
}

bool HandlerGame::handle(const sf::Event& _event) {
    return false;
}
