#pragma once

#include <sqee/gl/Textures.hpp>

#include "Manager.hpp"

namespace sqt {

class Material : sq::NonCopyable {
public:
    Material(const string& _path);

    HandleTexture mask, diff, norm, spec;

    void bind_dns() const;
};

}
