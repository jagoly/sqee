#include <sqee/gl/gl_ext_3_3.hpp>
#include <sqee/gl/maths.hpp>

#include "../resbank.hpp"
#include "model.hpp"

using namespace sqt::ent;

void Model::load(const string& _meshPath, const string& _skinPath,
                 const string& _animPath)
{
    if (!(mesh = resBank().meshH.get(_meshPath))) {
        mesh = resBank().meshH.add(_meshPath);
        mesh->load(_meshPath);
    }

    if (!(skin = resBank().skinH.get(_skinPath))) {
        skin = resBank().skinH.add(_skinPath);
        skin->load(_skinPath, resBank().texH);
    }

    sq::Animation* anim;
    if (!(anim = resBank().animH.get(_animPath))) {
        anim = resBank().animH.add(_animPath);
        anim->load(_animPath);
    }

    skeleton.setup(anim);
}
