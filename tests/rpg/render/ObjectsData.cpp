#include <sqee/dop/Functions.hpp>

#include <sqee/render/Mesh.hpp>
#include <sqee/render/Material.hpp>
#include <sqee/render/Armature.hpp>

#include "ObjectsData.hpp"

namespace dop = sq::dop;
namespace maths = sq::maths;

//============================================================================//

namespace sqt { namespace render {

//============================================================================//

void refresh_render_tables(ObjectsData& tables)
{
    const auto& stuff = sys::static_WorldStuff();

    //========================================================//

    tables.mModelTable.mIds.clear();
    tables.mModelTable.mData.clear();

    for (auto& entry : dop::joined(stuff.tables.transform, stuff.tables.model))
    {
        const auto& id = std::get<0>(entry);
        const auto& transform = std::get<1>(entry);
        const auto& model = std::get<2>(entry);

        tables.mModelTable.insert(id, {});
        auto& data = tables.mModelTable.mData.back();

        data.materials = model.materials;
        data.materials.shrink_to_fit();

        data.hasMaskTexture = false;

        for (const auto& material : data.materials)
            data.hasMaskTexture |= material->has_mask_texture();

        data.mesh = model.mesh;

        data.modelMatrix = maths::scale(transform.worldMatrix, model.stretch);
        data.isMirrored = std::signbit(maths::determinant(data.modelMatrix));
    }

    //========================================================//

    tables.mSkeletonTable.mIds.clear();
    tables.mSkeletonTable.mData.clear();

    for (auto& entry : dop::joined(stuff.tables.skeleton))
    {
        const auto& id = std::get<0>(entry);
        const auto& skeleton = std::get<1>(entry);

        tables.mSkeletonTable.insert(id, {});
        auto& data = tables.mSkeletonTable.mData.back();

        data.ubo.reserve("bones", 960u);
        data.ubo.create_and_allocate();

        data.armature = skeleton.armature;

        const auto& pose = data.armature->poseMap.at("Dead");
        auto uboData = sq::Armature::make_UboData(pose);
        data.ubo.update("bones", uboData.data());
    }
}

//============================================================================//

}} // namespace sqt::render
