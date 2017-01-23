#pragma once

#include <sqee/builtins.hpp>

namespace sqt { namespace sys {

//============================================================================//

class MessageSystem;
class EntitySystem;
class TransformSystem;
class CullingSystem;
class ModelSystem;
class SkeletonSystem;
class LightingSystem;
class RenderSystem;
class SoundSystem;

//============================================================================//

template <class Data, class System>
class SystemEdit : public sq::MoveOnly
{
public:

    SystemEdit(int32_t id, Data& data, System& system)
        : mId(id), mData(data), mSystem(system) {}

    const Data& data() const { return data; }

protected:

    const int32_t mId;
    Data& mData;

    System& mSystem;
};

//============================================================================//

}} // namespace sqt::sys
