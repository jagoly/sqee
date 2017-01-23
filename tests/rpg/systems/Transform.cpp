#include <sqee/dop/Functions.hpp>

#include "Transform.hpp"

namespace dop = sq::dop;
namespace maths = sq::maths;
using namespace sqt::sys;

//============================================================================//

void sqt::sys::system_refresh_transforms(WorldStuff& stuff)
{
    auto& tables = stuff.tables;

    auto joinResult = dop::joined(tables.entity, tables.transform);

    uint8_t maxNesting = 0u;

    //========================================================//

    for (auto& joinEntry : joinResult)
    {
        const auto& entity = std::get<1>(joinEntry);
        auto& transform = std::get<2>(joinEntry);

        maxNesting = maths::max(entity.nesting, maxNesting);

        const Vec3F& position = transform.localPosition;
        const QuatF& rotation = transform.localRotation;
        const float& scale = transform.localScale;

        // calculate the local space matrix
        transform.localMatrix = maths::translate(Mat4F(), position);
        transform.localMatrix *= Mat4F(Mat3F(rotation) * Mat3F(scale));

        transform.worldMatrix = transform.localMatrix;
    }

    //========================================================//

    //for (uint8_t level = 1u; level <= maxNesting; ++level)
    //{
        for (auto& joinEntry : joinResult)
        {
            const auto& entity = std::get<1>(joinEntry);
            auto& transform = std::get<2>(joinEntry);

            //if (entity.nesting != level) continue;

            if (auto parent = tables.transform.try_get(entity.parent))
            {
                Mat4F& worldMatrix = transform.worldMatrix;
                worldMatrix = parent->worldMatrix * worldMatrix;
            }
        }
    //}

    //========================================================//

    for (auto& tf : tables.transform.mData)
    {
        tf.worldPosition = Vec3F(tf.worldMatrix[3]);
        tf.worldRotation = maths::normalize(QuatF(tf.worldMatrix));
        tf.worldScale = maths::length(Vec3F(tf.worldMatrix[0]));
    }
}
