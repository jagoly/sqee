#pragma once

#include "object.hpp"

namespace sqt {
namespace obj {

class Mesh {
public:
    ~Mesh();

    struct Vertex {
        Vertex(float* _array) {
            x=_array[0]; y=_array[1]; z=_array[2];
            nx=_array[3]; ny=_array[4]; nz=_array[5];
            u=_array[6]; v=_array[7];
        }
        float x, y, z, nx, ny, nz, u, v;
    };

    void load(const std::string& _filePath);

    GLuint vboP, vboN, vboTc;
    GLuint vao;
    GLuint ibo;
    uint iCount;

    std::vector<Vertex> vertVec;
    std::vector<glm::uvec3> faceVec;

    void bind_buffers();
};
typedef sq::ResHolder<std::string, Mesh> MeshHolder;

struct Skin {
    void load(std::string& _filePath, sq::TexHolder* _texH);

    sq::Texture* texNorm;
    sq::Texture* texDiff;
    sq::Texture* texSpec;
    sq::Texture* texAmbi;
    bool alpha;

    void bind_textures();
};
typedef sq::ResHolder<std::string, Skin> SkinHolder;


class Model : public Object {
public:
    using Object::Object;
    void create();

    sq::Texture* texAmbi;
    Mesh* mesh;
    Skin* skin;

    bool shad;
    bool subm;
    glm::vec3 pos, rot, sca;
    glm::mat4 modelMat;
};

}
}
