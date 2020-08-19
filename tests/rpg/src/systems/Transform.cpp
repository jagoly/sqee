#include "Transform.hpp"

#include <sqee/maths/Functions.hpp>
#include <sqee/misc/DopFunctions.hpp>

namespace dop = sq::dop;
namespace maths = sq::maths;
using namespace sqt::sys;

//============================================================================//

void sqt::sys::system_refresh_nesting(WorldStuff& stuff)
{
    stuff.nestingRootGroup.clear();

    for (auto& table : stuff.nestingTables)
        table.clear();

    //--------------------------------------------------------//

    for (auto& [id, entity] : dop::joined(stuff.tables.entity))
    {
        uint8_t nestLevel = 0u;

        for (int32_t grand = entity.parent; grand != -1; ++nestLevel)
            grand = stuff.tables.entity.get(grand).parent;

        SQASSERT(nestLevel <= 8u, "too much nesting");

        if (nestLevel == 0u) stuff.nestingRootGroup.insert(id);
        else stuff.nestingTables[--nestLevel].insert(id, entity.parent);
    }
}

//============================================================================//

void sqt::sys::system_refresh_transforms(WorldStuff& stuff)
{
    for (auto& transform : stuff.tables.transform.mData)
    {
        const Vec3F& position = transform.localPosition;
        const QuatF& rotation = transform.localRotation;
        const float& scale = transform.localScale;

        transform.localMatrix = maths::translate(Mat4F(), position) * Mat4F(Mat3F(rotation) * Mat3F(scale));

        transform.worldMatrix = transform.localMatrix;
    }

    //--------------------------------------------------------//

    for (auto& nestingTable : stuff.nestingTables)
    {
        for (auto& [id, parent, transform] : dop::joined(nestingTable, stuff.tables.transform))
        {
            if (auto parentTransform = stuff.tables.transform.try_get(parent))
                transform.worldMatrix = parentTransform->worldMatrix * transform.worldMatrix;
        }
    }

    //--------------------------------------------------------//

    for (auto& transform : stuff.tables.transform.mData)
    {
        transform.worldPosition = Vec3F(transform.worldMatrix[3]);
        transform.worldRotation = maths::normalize(QuatF(transform.worldMatrix));
        transform.worldScale = maths::length(Vec3F(transform.worldMatrix[0]));
    }

    //--------------------------------------------------------//

    for (auto& [id, transform, model] : dop::joined(stuff.tables.transform, stuff.tables.model))
    {
        model.matrix = maths::scale(transform.worldMatrix, model.stretch);
    }
}
