#pragma once

#include "../resbank.hpp"

namespace sqt {
namespace ent {

class Model {
public:
    void load(const string& _meshPath, const string& _skinPath, const string& _animPath);

    glm::mat4 modelMat;

    sq::Mesh* mesh;
    sq::Texture* texNorm;
    sq::Texture* texDiff;
    sq::Texture* texSpec;

    sq::Skeleton skeleton;

    void tick();
    void calc(double _accum);

    void bind_textures();
};

}
}
