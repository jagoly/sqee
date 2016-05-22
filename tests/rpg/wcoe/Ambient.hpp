#pragma once

#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sqt {

class Ambient final : sq::NonCopyable {
public:
    Ambient();

    void configure();
    void update();

    bool  PROP_enabled = false;
    Vec3F PROP_colour  = {0.5f, 0.5f, 0.5f};

    sq::UniformBuffer ubo;
};

}
