#pragma once
#include <sqee/forward.hpp>

#include <sqee/gl/textures.hpp>
#include <sqee/misc/containers.hpp>
#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>
#include <sqee/models/skeleton.hpp>

namespace sqt {

class ResBank {
public:
    ResBank();

    sq::ResHolder<sq::Texture> texH;
    sq::ResHolder<sq::Mesh> meshH;
    sq::ResHolder<sq::Skin> skinH;
    sq::ResHolder<sq::Animation> animH;

    sq::Texture tex2D_8080FF;
    sq::Texture tex2D_000000;
    sq::Texture tex2D_FFFFFF;

    sq::Texture tex2DA_8080FF;
    sq::Texture tex2DA_000000;
    sq::Texture tex2DA_FFFFFF;
};


ResBank& resBank();

}
