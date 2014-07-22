#pragma once

#include <string>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

namespace sqe {

std::string get_shader_error(const GLuint& shader);

bool create_shader(std::string vertPath, std::string fragPath, GLuint& prog);

bool load_mesh(std::string filePath, GLuint& vao, int& pointCount);

}
