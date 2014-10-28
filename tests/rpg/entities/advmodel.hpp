#include <vector>
#include <array>
#include <memory>

#include <sqee/gl/gl.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/misc/containers.hpp>

namespace sqt {

struct AdvMesh {
    ~AdvMesh();

    struct Vertex {
        Vertex(float _x, float _y, float _z, float _nx, float _ny, float _nz,
               float _u, float _v, std::array<int, 8> _b, std::array<float, 8> _w)
            : x(_x), y(_y), z(_z), nx(_nx), ny(_ny), nz(_nz), u(_u), v(_v), b(_b), w(_w) {}
        const float x, y, z, nx, ny, nz, u, v;
        const std::array<int, 8> b;
        const std::array<float, 8> w;
    };

    struct Face {
        Face(uchar _t, uint _a, uint _b, uint _c)
            : t(_t), a(_a), b(_b), c(_c) {}
        const uchar t; const uint a, b, c;
    };

    void load(const std::string& _filePath);

    GLuint vboP, vboN, vboB, vboW, vboTc;

    GLuint vao;
    GLuint ibo, tboTn, vboTn;
    uint iCount;

    std::vector<Vertex> vertVec;
    std::vector<Face> faceVec;

    void bind_buffers();
};
typedef sq::ResHolder<std::string, AdvMesh> AdvMeshHolder;

struct AdvSkin {
    void load(const std::string& _filePath, sq::TexHolder& _texH);

    sq::Texture* texNorm;
    sq::Texture* texDiff;
    sq::Texture* texSpec;

    void bind_textures();
};
typedef sq::ResHolder<std::string, AdvSkin> AdvSkinHolder;


class Skeleton {
public:
    struct Pose {
        Pose() = default;
        Pose(uint _bCount, const float* _quatData, const float* _offsData) {
            memcpy(quatData, _quatData, _bCount*4*sizeof(float));
            memcpy(offsData, _offsData, _bCount*3*sizeof(float));
        }
        float quatData[40*4];
        float offsData[40*3];
    };

    void load(const std::string& _filePath);

    uint bCount;
    std::vector<Pose> poseVec;
    std::vector<std::pair<uint, Pose&>> kfrVec;
};


class SkeletonAnim {
public:
    // change to unique when qt creator fixed
    typedef std::shared_ptr<SkeletonAnim> Ptr;

    SkeletonAnim(Skeleton& _skeleton)
        : skeleton(_skeleton), pCrnt(&_skeleton.kfrVec[0]), pNext(&_skeleton.kfrVec[0]) {}

    Skeleton::Pose pose;

    void tick();
    void jump(int _frame);
    void calc(double _accum);

private:
    Skeleton& skeleton;
    std::pair<uint, Skeleton::Pose&>* pCrnt;
    std::pair<uint, Skeleton::Pose&>* pNext;
    uint keyProg = 0;
    uint keyInd = 0;
};


}
