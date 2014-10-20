#include <vector>
#include <memory>

#include <libsqee/gl/gl.hpp>
#include <libsqee/gl/textures.hpp>
#include <libsqee/misc/containers.hpp>

namespace sqt {

struct AdvMesh {
    ~AdvMesh();

    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
        uint b[8]{};
        float w[8]{};
    };

    void load(const std::string& _filePath);

    uint bCount=0, wCount=0;
    GLuint vboP, vboN, vboB, vboW, vboTc;

    GLuint vao;
    GLuint ibo, tboTn, vboTn;
    uint iCount;

    std::vector<Vertex> vertVec;
    std::vector<glm::uvec4> faceVec;

    void bind_buffers();
};
typedef sq::ResHolder<std::string, AdvMesh> AdvMeshHolder;

class Skeleton {
    struct Bone {
        typedef std::weak_ptr<Bone> Ptr;

        Ptr parent;
        std::vector<Ptr> children;
        glm::vec3 rotBase;
        glm::vec3 posBase;

        glm::mat4 mat;

        void rotate_abs(glm::vec3 _rot);
        void rotate_rel(glm::vec3 _rot);
    };
};

struct AdvSkin {
    void load(std::string& _filePath, sq::TexHolder* _texH);

    sq::Texture* texNorm;
    sq::Texture* texDiff;
    sq::Texture* texSpec;

    void bind_textures();
};
typedef sq::ResHolder<std::string, AdvSkin> AdvSkinHolder;

}
