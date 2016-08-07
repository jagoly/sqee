#include <map>

#include "Animation.hpp"

#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/Skeleton.hpp"
#include "../components/LightSpot.hpp"
#include "../components/LightPoint.hpp"
#include "../components/LightOrtho.hpp"

using namespace sqt;
namespace maths = sq::maths;

struct AnimationSystem::Impl {

    struct EntityData {
        TransformComponent* transform = nullptr;
        ModelComponent* model = nullptr;
        SkeletonComponent* skeleton = nullptr;
        LightSpotComponent* lightSpot = nullptr;
        LightPointComponent* lightPoint = nullptr;
    };

    std::map<const sq::Entity*, EntityData> entityDataMap;
};

AnimationSystem::AnimationSystem() {
    impl = std::make_unique<Impl>();
}

AnimationSystem::~AnimationSystem() = default;

void AnimationSystem::configure_entity(sq::Entity* _entity) {
    Impl::EntityData& data = impl->entityDataMap[_entity];

    data.transform = _entity->try_get_component<TransformComponent>();
    data.model = _entity->try_get_component<ModelComponent>();
    data.skeleton = _entity->try_get_component<SkeletonComponent>();
    data.lightSpot = _entity->try_get_component<LightSpotComponent>();
    data.lightPoint = _entity->try_get_component<LightPointComponent>();
}

void AnimationSystem::refresh_entity(sq::Entity* _entity) {
    Impl::EntityData& data = impl->entityDataMap.at(_entity);

    if (data.transform != nullptr) {}

    if (data.model != nullptr) {}
}
