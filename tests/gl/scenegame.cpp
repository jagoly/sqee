#include "scenegame.hpp"

#include "helpers.hpp"

namespace sqt {

SceneGame::SceneGame(sq::Application* _app) : sq::Scene(_app) {
    tickRate = 120;
    dt = 1/120.d;

    position = 0.f;
    nextPos = 0.01f;
}

void SceneGame::update() {
    static float speed = 0.01f;
    static bool direction = true;

    if (direction) {
        position += speed;
        nextPos = position + speed;
    } else {
        position -= speed;
        nextPos = position - speed;
    }

    if (position > 1.f | position < -1.f) {
        direction = !direction;
    }
}

void SceneGame::render(sf::RenderTarget& target, float) {
    static bool first = true;

    static GLfloat points[] = {
         0.0f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };

    static GLfloat colours[] = {
        1.f, 0.f, 0.f,
        0.f, 1.f, 0.f,
        0.f, 0.f, 1.f
    };

    static float matrix[] = {
        1.f,  0.f, 0.f, 0.f,
        0.f,  1.f, 0.f, 0.f,
        0.f,  0.f, 1.f, 0.f,
        0.5f, 0.f, 0.f, 1.f
    };

    static GLuint vboPoints = 0;
    static GLuint vboColours = 0;

    static GLuint vao = 0;

    static GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    static GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    static GLuint prog = glCreateProgram();

    static GLuint loc_matrix;

    if (first) {
        glGenBuffers(1, &vboPoints);
        glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

        glGenBuffers(1, &vboColours);
        glBindBuffer(GL_ARRAY_BUFFER, vboColours);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, vboColours);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        std::string vertStr = load_from_file("res/shaders/test_vs.glsl");
        int vertSize = vertStr.size();
        const char* vertSrc = vertStr.c_str();
        glShaderSource(vert, 1, &vertSrc, &vertSize);
        glCompileShader(vert);
        log_shader_error(vert);

        std::string fragStr = load_from_file("res/shaders/test_fs.glsl");
        const char* fragSrc = fragStr.c_str();
        int fragSize = fragStr.size();
        glShaderSource(frag, 1, &fragSrc, &fragSize);
        glCompileShader(frag);
        log_shader_error(frag);

        glAttachShader(prog, vert);
        glAttachShader(prog, frag);
        glLinkProgram(prog);
        log_shader_error(prog);

        glGetUniformLocation(prog, "matrix");

        first = false;
    }

    glUseProgram(prog);

    matrix[12] = interpolate(position, nextPos);
    glUniformMatrix4fv(loc_matrix, 1, GL_FALSE, matrix);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(0);
    glBindVertexArray(0);
}

bool HandlerGame::handle(sf::Event& event) {
    return false;
}

}
