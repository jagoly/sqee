#include "../resbank.hpp"
#include "model.hpp"

using namespace sqt::ent;

void Model::load(const string& _meshPath, const string& _skinPath, const string& _animPath) {
    if (!(mesh = resBank().meshH.get(_meshPath))) {
        mesh = resBank().meshH.add(_meshPath);
        mesh->load(_meshPath);
    }

    skin.load(_skinPath, resBank().texH);

    sq::Animation* anim;
    if (!(anim = resBank().animH.get(_animPath))) {
        anim = resBank().animH.add(_animPath);
        anim->load(_animPath);
    }

    skeleton.setup(anim);
}