#pragma once

#include <gl/gl.hpp>
#include <gl/textures.hpp>

namespace sq {

class Framebuffer {
public:
    void create(vector<GLenum> _drawBuffers,
                vector<GLenum> _cFormats, vector<GLenum> _iCFormats);
    void create(GLenum _dsFormat, GLenum _idsFormat);
    void create(vector<GLenum> _drawBuffers,
                vector<GLenum> _cFormats, vector<GLenum> _icFormats,
                GLenum _dsFormat, GLenum _idsFormat);

    void resize(glm::uvec2 _size);
    void clear();
    void clear(glm::vec4 _colour);
    void use();
    void useVP();

    GLuint fbo;

    vector<Texture> cTexVec;
    Texture dsTex;
private:
    vector<GLenum> drawBuffers;
    glm::uvec2 size;
};

}
