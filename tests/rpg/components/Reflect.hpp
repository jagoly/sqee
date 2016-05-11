#pragma once

#include <sqee/forward.hpp>

#include <sqee/maths/Volumes.hpp>

#include <sqee/gl/UniformBuffer.hpp>

#include <sqee/ecs/Component.hpp>
#include <sqee/ecs/Entity.hpp>

namespace sqt {

class TransformComponent;
class ModelComponent;

class ReflectComponent : public sq::Component {
public:
    ReflectComponent();

    static string type() { return "Reflect"; }

    void setup_depends(sq::Entity* _e);

    // User Properties /////
    float PROP_factor = 1.f;

    // System Properties /////
    sq::UniformBuffer ubo;
    Vec3F normal, trans;

    // Hard Depended Components /////
    const TransformComponent* DEP_Transform;
    const ModelComponent* DEP_Model;
};

}
