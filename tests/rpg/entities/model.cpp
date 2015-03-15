#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/models/animation.hpp>
#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>

#include "model.hpp"

using namespace sqt::ent;

void Model::load(const string& _meshPath, const string& _skinPath, const string& _animPath) {
    if (!(mesh = sq::res::mesh().get(_meshPath))) {
        mesh = sq::res::mesh().add(_meshPath);
        mesh->create(_meshPath);
    }

    if (!(skin = sq::res::skin().get(_skinPath))) {
        skin = sq::res::skin().add(_skinPath);
        skin->create(_skinPath);
    }

    sq::Animation* anim;
    if (!(anim = sq::res::animation().get(_animPath))) {
        anim = sq::res::animation().add(_animPath);
        anim->create(_animPath);
    }

    skeleton.setup(anim);
}
