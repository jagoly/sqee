#pragma once
#include <sqee/forward.hpp>

#include <sqee/models/skeleton.hpp>

namespace sqt {
namespace ent {

class Model {
public:
    void load(const string& _meshPath, const string& _skinPath,
              const string& _animPath);

    glm::mat4 modelMat;

    sq::Mesh* mesh;
    sq::Skin* skin;
    sq::Skeleton skeleton;

    void tick();
    void calc(double _accum);
};

}
}
