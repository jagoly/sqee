#include "scenegame.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <libsqee/gl/gl.hpp>
#include <libsqee/extra/helpers.hpp>

#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;

    camera = sq::Camera({0.f, 0.f, 2.f}, 0.f, 0.f, 4, 3, 1.17f, 0.1f, 100.f);
    camera.update_projMatrix();
    camera.update_viewMatrix();
}

void SceneGame::update() {
    int xR = 0;
    int yR = 0;
    int NS = 0;
    int EW = 0;
    int UD = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) xR -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) xR += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) yR -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) yR += 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) EW += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) EW -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) NS += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) NS -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp)) UD += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown)) UD -= 1;

    camera.pos.x += EW * 0.01f;
    camera.pos.y += NS * 0.01f;
    camera.pos.z += UD * 0.01f;

    camera.xRot += xR * 0.01f;
    camera.yRot += yR * 0.01f;

    camera.update_viewMatrix();
    camera.update_projMatrix();
}

void SceneGame::render(sf::RenderTarget& target, float) {
    target.popGLStates();

    static bool first = true;

    static GLuint prog = glCreateProgram();

    static GLuint u_projMatrix, u_viewMatrix,
                  u_w_lightPos, u_lightSpec, u_lightDiff, u_lightAmbi,
                  u_basicTex;

    static GLuint texture = 0;

    if (first) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //glEnable (GL_CULL_FACE); // cull face
        //glCullFace (GL_BACK); // cull back face
        //glFrontFace (GL_CCW); // set counter-clock-wise vertex order to mean the front

        sf::Image image;
        image.loadFromFile("res/dice.png");
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getSize().x, image.getSize().y, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, image.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        sq::create_shader("res/shaders/cube_vs.glsl", "res/shaders/cube_fs.glsl", prog);

        u_projMatrix = glGetUniformLocation(prog, "projMatrix");
        u_viewMatrix = glGetUniformLocation(prog, "viewMatrix");
        u_w_lightPos = glGetUniformLocation(prog, "w_lightPos");
        u_lightSpec = glGetUniformLocation(prog, "lightSpec");
        u_lightDiff = glGetUniformLocation(prog, "lightDiff");
        u_lightAmbi = glGetUniformLocation(prog, "lightAmbi");
        u_basicTex = glGetUniformLocation(prog, "basicTex");

        first = false;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glClearColor(0.3f, 0.3f, 0.3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(prog);

    glUniformMatrix4fv(u_projMatrix, 1, GL_FALSE, glm::value_ptr(camera.projMat));
    glUniformMatrix4fv(u_viewMatrix, 1, GL_FALSE, glm::value_ptr(camera.viewMat));

    glUniform3fv(u_lightSpec, 1, glm::value_ptr(light.lightSpec));
    glUniform3fv(u_lightDiff, 1, glm::value_ptr(light.lightDiff));
    glUniform3fv(u_lightAmbi, 1, glm::value_ptr(light.lightAmbi));
    glUniform3fv(u_w_lightPos, 1, glm::value_ptr(light.worldPos));

    glUniform1i(u_basicTex, 0);

    glBindVertexArray(cube.vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(0);
    glBindVertexArray(0);

    target.pushGLStates();
}

bool HandlerGame::handle(sf::Event&) {
    return false;
}

Cube::Cube() {
    vboPoints = 0;
    vboTexcoords = 0;
    vboNormals = 0;
    vao = 0;

    glGenBuffers(1, &vboPoints);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    glGenBuffers(1, &vboTexcoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

}
