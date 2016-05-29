#pragma once

#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/maths/Vectors.hpp>

namespace sqt {

class Ambient final : sq::NonCopyable {
public:
    Ambient();

    void configure();
    void update();

    // User Properties /////
    Vec3F PROP_colour = {0.5f, 0.5f, 0.5f};

    // System Properties /////
    sq::UniformBuffer ubo;
};

}
