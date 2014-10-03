#pragma once

#include "object.hpp"

namespace sqt {
namespace obj {

struct KeyFrame {GLuint vboP=0, vboN=0, vboTc=0, vboT=0;};

struct Mesh {
    typedef std::shared_ptr<Mesh> Ptr;
    void load(std::string& _filePath, int _offset);
    ~Mesh();

    void tick();

    GLuint vao;
    GLuint ibo;
    uint iCount;

    std::vector<KeyFrame> keyFrames;
    uint keyCurrent, keyNext;
    uint tickCurrent;

    std::vector<ushort> keyTimes;

    bool anim;
    uint offset;
    uint span;
    uint frac;
    double accum;

    std::vector<std::vector<sq::Vertex>> vertVec;
    std::vector<glm::uvec3> faceVec;
};
typedef sq::ResHolder<sq::SIPair, Mesh::Ptr> MeshHolder;

struct Skin {
    typedef std::shared_ptr<Skin> Ptr;
    void load(std::string& _filePath, int _offset, sq::TexHolder* _texHolder);

    sq::Texture::Ptr texNorm;
    sq::Texture::Ptr texDiff;
    sq::Texture::Ptr texSpec;
    sq::Texture::Ptr texAmbi;
    bool alpha;
};
typedef sq::ResHolder<sq::SIPair, Skin::Ptr> SkinHolder;


class Model : public Object {
public:
    typedef std::shared_ptr<Model> Ptr;
    using Object::Object;
    void create();

    sq::Texture::Ptr texAmbi;

    bool shad;
    glm::vec3 pos, rot, sca;
    glm::mat4 modelMat;

    Mesh::Ptr mesh;
    Skin::Ptr skin;
};

}
}
