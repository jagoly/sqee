#pragma once

#include <memory>

#include <SFML/System.hpp>

#include <sqee/gl/gl.hpp>
#include <sqee/gl/camera.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/misc/containers.hpp>

#include "maps/level.hpp"
#include "entities/advmodel.hpp"

namespace sqt {

class Player {
public:
    // change to unique when qt creator fixed
    typedef std::shared_ptr<Player> Ptr;

    Player(AdvMeshHolder& _advMeshH, AdvSkinHolder& _advSkinH, sq::TexHolder& _texH)
        : advMeshH(_advMeshH), advSkinH(_advSkinH), texH(_texH) {}

    AdvMesh* mesh;
    AdvSkin* skin;
    SkeletonAnim::Ptr skel;

    bool moving = false;

    glm::mat4 modelMat;

    void test_init();

    void tick(char _moveDir, Level::Ptr& _level);
    void calc(double _accum, sq::Camera& _camera);

private:
    std::string layer;
    glm::ivec2 gridPos;

    glm::vec3 posCrnt;
    glm::vec3 posNext;

    AdvMeshHolder& advMeshH;
    AdvSkinHolder& advSkinH;
    sq::TexHolder& texH;

    Skeleton skeleton;
};

}
