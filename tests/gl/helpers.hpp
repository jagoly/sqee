#pragma once

#include <string>
#include <fstream>
#include <sstream>

#define LOG_FILE "log_file.txt"

namespace sqt {

std::string load_from_file(std::string filePath) {
    std::cout << "Loading shader \"" << filePath << "\"" << std::endl;
    std::ifstream src;
    src.open(filePath);
    std::stringstream sstr;
    sstr << src.rdbuf();
    src.close();
    return sstr.str();
}

void write_to_log(const std::string& text) {
    std::ofstream log_file(LOG_FILE, std::ios_base::out | std::ios_base::app);
    log_file << text << std::endl;
}

void log_shader_error(const GLuint& shader) {
    int max_length = 2048;
    int actual_length = 0;
    char log[2048];
    glGetShaderInfoLog(shader, max_length, &actual_length, log);
    write_to_log(log);
}

}
