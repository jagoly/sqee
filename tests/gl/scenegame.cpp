#include "scenegame.hpp"

#include "helpers.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;
}

void SceneGame::update() {
    int X = 0;
    int Y = 0;
    int Z = 0;
    int NS = 0;
    int EW = 0;
    int FB = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) X += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) X -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) Y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) Y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) Z += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) Z -= 1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) EW += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) EW -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) NS += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) NS -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp)) FB += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown)) FB -= 1;

    cube.rot[0] += X * 0.01f;
    cube.rot[1] += Y * 0.01f;
    cube.rot[2] += Z * 0.01f;

    cube.pos[0] += EW * 0.01f;
    cube.pos[1] += NS * 0.01f;
    cube.pos[2] += FB * 0.01f;

    cube.transformMatrix = glm::mat4();
    cube.transformMatrix = glm::translate(cube.transformMatrix, glm::vec3(cube.pos[0], cube.pos[1], cube.pos[2]));
    cube.transformMatrix = glm::rotate(cube.transformMatrix, cube.rot[0], glm::vec3(1, 0, 0));
    cube.transformMatrix = glm::rotate(cube.transformMatrix, cube.rot[1], glm::vec3(0, 1, 0));
    cube.transformMatrix = glm::rotate(cube.transformMatrix, cube.rot[2], glm::vec3(0, 0, 1));
}

void SceneGame::render(sf::RenderTarget& target, float) {
    static bool first = true;

    static GLfloat pointsBG[] = {
        -0.8f, -0.8f, 0.f,
        -0.8f, 0.8f, 0.f,
        0.8f, -0.8f, 0.f,
        -0.8f, 0.8f, 0.f,
        0.8f, -0.8f, 0.f,
        0.8f, 0.8f, 0.f
    };

    static GLfloat coloursBG[] = {
        0.f, 0.f, 0.f,
        0.7f, 0.f, 0.7f,
        0.7f, 0.f, 0.7f,
        0.7f, 0.f, 0.7f,
        0.7f, 0.f, 0.7f,
        1.f, 1.f, 1.f
    };

    static GLuint bgVboPoints = 0;
    static GLuint bgVboColours = 0;
    static GLuint bgVao = 0;

    static GLuint bgVert = glCreateShader(GL_VERTEX_SHADER);
    static GLuint bgFrag = glCreateShader(GL_FRAGMENT_SHADER);
    static GLuint bgProg = glCreateProgram();

    if (first) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);

        glGenBuffers(1, &bgVboPoints);
        glBindBuffer(GL_ARRAY_BUFFER, bgVboPoints);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pointsBG), pointsBG, GL_STATIC_DRAW);

        glGenBuffers(1, &bgVboColours);
        glBindBuffer(GL_ARRAY_BUFFER, bgVboColours);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coloursBG), coloursBG, GL_STATIC_DRAW);

        glGenVertexArrays(1, &bgVao);
        glBindVertexArray(bgVao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, bgVboPoints);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, bgVboColours);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        std::string bgVertStr = load_from_file("res/shaders/bg_vs.glsl");
        int bgVertSize = bgVertStr.size();
        const char* bgVertSrc = bgVertStr.c_str();
        glShaderSource(bgVert, 1, &bgVertSrc, &bgVertSize);
        glCompileShader(bgVert);
        log_shader_error(bgVert);

        std::string bgFragStr = load_from_file("res/shaders/bg_fs.glsl");
        const char* bgFragSrc = bgFragStr.c_str();
        int bgFragSize = bgFragStr.size();
        glShaderSource(bgFrag, 1, &bgFragSrc, &bgFragSize);
        glCompileShader(bgFrag);
        log_shader_error(bgFrag);

        glAttachShader(bgProg, bgVert);
        glAttachShader(bgProg, bgFrag);
        glLinkProgram(bgProg);
        log_shader_error(bgProg);

        first = false;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glUseProgram(bgProg);
    glBindVertexArray(bgVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glUseProgram(cube.prog);

    glUniformMatrix4fv(cube.u_transformMatrix, 1, GL_FALSE, glm::value_ptr(cube.transformMatrix));
    glBindVertexArray(cube.vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(0);
    glBindVertexArray(0);
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}

Cube::Cube() {
    vboPoints = 0;
    vboColours = 0;
    vao = 0;

    vert = glCreateShader(GL_VERTEX_SHADER);
    frag = glCreateShader(GL_FRAGMENT_SHADER);
    prog = glCreateProgram();

    glGenBuffers(1, &vboPoints);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsCube), pointsCube, GL_STATIC_DRAW);

    glGenBuffers(1, &vboColours);
    glBindBuffer(GL_ARRAY_BUFFER, vboColours);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coloursCube), coloursCube, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboColours);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    std::string vertStr = load_from_file("res/shaders/cube_vs.glsl");
    int vertSize = vertStr.size();
    const char* vertSrc = vertStr.c_str();
    glShaderSource(vert, 1, &vertSrc, &vertSize);
    glCompileShader(vert);
    log_shader_error(vert);

    std::string fragStr = load_from_file("res/shaders/cube_fs.glsl");
    const char* fragSrc = fragStr.c_str();
    int fragSize = fragStr.size();
    glShaderSource(frag, 1, &fragSrc, &fragSize);
    glCompileShader(frag);
    log_shader_error(frag);

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    log_shader_error(prog);

    u_transformMatrix = glGetUniformLocation(prog, "transformMatrix");
}

}
