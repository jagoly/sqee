#include <sqee/objects/DrawItem.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/objects/Armature.hpp>
#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Pipeline.hpp>
#include <sqee/objects/Texture.hpp>

using namespace sq;

//============================================================================//

std::vector<DrawItem> DrawItem::load_from_json(const String& path, const Armature& armature, MeshCache& meshes, PipelineCache& pipelines, TextureCache& textures)
{
    using PushConstantType = Pipeline::PushConstantType;
    using TrackType = Armature::TrackType;

    constexpr auto get_expected_push_constant_type = [](ParamType type)
    {
        switch (type) {
        case ParamType::ConstFloat:            return PushConstantType::Float;
        case ParamType::ConstFloat2:           return PushConstantType::Vec2F;
        case ParamType::ConstFloat3:           return PushConstantType::Vec3F;
        case ParamType::ConstFloat4:           return PushConstantType::Vec4F;
        case ParamType::ConstInt:              return PushConstantType::Int;
        case ParamType::ConstInt2:             return PushConstantType::Vec2I;
        case ParamType::ConstInt3:             return PushConstantType::Vec3I;
        case ParamType::ConstInt4:             return PushConstantType::Vec4I;
        case ParamType::ModelMatrixIndex:      return PushConstantType::Uint;
        case ParamType::NormalMatrixIndex:     return PushConstantType::Uint;
        case ParamType::TextureIndex:          return PushConstantType::Uint;
        case ParamType::AnimFloat:             return PushConstantType::Float;
        case ParamType::AnimFloat2:            return PushConstantType::Vec2F;
        case ParamType::AnimFloat3:            return PushConstantType::Vec3F;
        case ParamType::AnimFloat4:            return PushConstantType::Vec4F;
        case ParamType::AnimInt:               return PushConstantType::Int;
        case ParamType::AnimInt2:              return PushConstantType::Vec2I;
        case ParamType::AnimInt3:              return PushConstantType::Vec3I;
        case ParamType::AnimInt4:              return PushConstantType::Vec4I;
        case ParamType::AnimTextureTransform:  return PushConstantType::Mat23F;
        } SQEE_UNREACHABLE();
    };

    const JsonValue json = sq::parse_json_from_file(path);

    const auto& jsonAssets = json.at("assets").get_ref<const JsonValue::object_t&>();
    const auto& jsonItems = json.at("items").get_ref<const JsonValue::array_t&>();

    const auto& jsonTextures = jsonAssets.at("textures");
    const auto& jsonMeshes = jsonAssets.at("meshes");
    const auto& jsonPipelines = jsonAssets.at("pipelines");

    //--------------------------------------------------------//

    std::vector<DrawItem> result;
    result.reserve(jsonItems.size());

    for (const auto& jItem : jsonItems)
    {
        DrawItem& item = result.emplace_back();

        if (const auto& condition = jItem.at("condition"); !condition.is_null())
            condition.get_to(item.condition);

        item.mesh = meshes.acquire (
            jsonMeshes.at(jItem.at("mesh").get_ref<const String&>()).get_ref<const String&>()
        );

        item.pipeline = pipelines.acquire (
            jsonPipelines.at(jItem.at("pipeline").get_ref<const String&>())
        );

        if (const auto& subMesh = jItem.at("subMesh"); !subMesh.is_null())
            item.subMesh = int8_t(item.mesh->get_sub_mesh_index(subMesh.get_ref<const String&>()));

        const auto& jParams = jItem.at("params").get_ref<const JsonValue::object_t&>();
        item.params.reserve(jParams.size());

        for (const auto& [jParamKey, jParam] : jParams)
        {
            DrawItem::Param& param = item.params.emplace_back();

            jParam.at("type").get_to(param.type);

            const auto pushConstant = item.pipeline->get_push_constant_info(jParamKey);

            const auto expectedPushConstantType = get_expected_push_constant_type(param.type);
            if (pushConstant.type != expectedPushConstantType)
                SQEE_THROW("push constant has type '{}', but expected '{}'", pushConstant.type, expectedPushConstantType);

            param.offset = pushConstant.offset;

            switch (param.type) {

            case ParamType::ConstFloat: jParam.at("value").get_to(param.constFloat); continue;
            case ParamType::ConstFloat2: jParam.at("value").get_to(param.constFloat2); continue;
            case ParamType::ConstFloat3: jParam.at("value").get_to(param.constFloat3); continue;
            case ParamType::ConstFloat4: jParam.at("value").get_to(param.constFloat4); continue;
            case ParamType::ConstInt: jParam.at("value").get_to(param.constInt); continue;
            case ParamType::ConstInt2: jParam.at("value").get_to(param.constInt2); continue;
            case ParamType::ConstInt3: jParam.at("value").get_to(param.constInt3); continue;
            case ParamType::ConstInt4: jParam.at("value").get_to(param.constInt4); continue;

            case ParamType::ModelMatrixIndex:
            case ParamType::NormalMatrixIndex:
            {
                param.matrixIndex = uint(armature.bone_from_json(jParam.at("bone")) + 1);
                continue;
            }

            case ParamType::TextureIndex:
            {
                param.texture = textures.acquire (
                    jsonTextures.at(jParam.at("texture").get_ref<const String&>()).get_ref<const String&>()
                );
                continue;
            }

            case ParamType::AnimFloat:
            case ParamType::AnimFloat2:
            case ParamType::AnimFloat3:
            case ParamType::AnimFloat4:
            case ParamType::AnimInt:
            case ParamType::AnimInt2:
            case ParamType::AnimInt3:
            case ParamType::AnimInt4:
            {
                const uint8_t blockIndex = armature.get_block_index(jParam.at("block").get_ref<const String&>());
                const uint16_t trackIndex = armature.get_block_track_index(blockIndex, jParam.at("track").get_ref<const String&>());
                const auto& trackInfo = armature.get_track_infos()[trackIndex];

                const auto expectedTrackType = TrackType(uint8_t(param.type) - uint8_t(ParamType::AnimFloat));
                if (trackInfo.type != expectedTrackType)
                    SQEE_THROW("track has type '{}', but expected '{}'", trackInfo.type, expectedTrackType);

                param.sampleOffset = trackInfo.offset;

                continue;
            }

            case ParamType::AnimTextureTransform:
            {
                const uint8_t blockIndex = armature.get_block_index(jParam.at("block").get_ref<const String&>());

                const auto& jTracks = jParam.at("tracks").get_ref<const JsonValue::array_t&>();
                if (jTracks.size() != 3u)
                    SQEE_THROW("tracks has {} items, but must have 3", jTracks.size());

                const uint16_t trackIndex0 = armature.get_block_track_index(blockIndex, jTracks[0].get_ref<const String&>());
                const uint16_t trackIndex1 = armature.get_block_track_index(blockIndex, jTracks[1].get_ref<const String&>());
                const uint16_t trackIndex2 = armature.get_block_track_index(blockIndex, jTracks[2].get_ref<const String&>());

                const auto& trackInfo0 = armature.get_track_infos()[trackIndex0];
                const auto& trackInfo1 = armature.get_track_infos()[trackIndex1];
                const auto& trackInfo2 = armature.get_track_infos()[trackIndex2];

                if (trackInfo0.type != TrackType::Float2)
                    SQEE_THROW("track 0 has type '{}', but expected 'Float2'", trackInfo0.type);

                if (trackInfo1.type != TrackType::Angle)
                    SQEE_THROW("track 1 has type '{}', but expected 'Angle'", trackInfo1.type);

                if (trackInfo2.type != TrackType::Float2)
                    SQEE_THROW("track 2 has type '{}', but expected 'Float2'", trackInfo2.type);

                param.sampleOffsets[0] = trackInfo0.offset;
                param.sampleOffsets[1] = trackInfo1.offset;
                param.sampleOffsets[2] = trackInfo2.offset;

                continue;
            }

            } SQEE_UNREACHABLE();
        }

        // sort params by push constant offset
        const auto compare = [](const DrawItem::Param& lhs, const DrawItem::Param& rhs) { return lhs.offset < rhs.offset; };
        std::sort(item.params.begin(), item.params.end(), compare);
    }

    return result;
}

//============================================================================//

void DrawItem::compute_push_constants(const AnimSample& sample, uint modelMatsIndex, uint normalMatsIndex, std::array<std::byte, 128u>& out) const
{
    for (const Param& param : params)
    {
        switch (param.type) {

        case ParamType::ConstFloat:
            *reinterpret_cast<float*>(out.data() + param.offset) = param.constFloat;
            continue;

        case ParamType::ConstFloat2:
            *reinterpret_cast<Vec2F*>(out.data() + param.offset) = param.constFloat2;
            continue;

        case ParamType::ConstFloat3:
            *reinterpret_cast<Vec3F*>(out.data() + param.offset) = param.constFloat3;
            continue;

        case ParamType::ConstFloat4:
            *reinterpret_cast<Vec4F*>(out.data() + param.offset) = param.constFloat4;
            continue;

        case ParamType::ConstInt:
            *reinterpret_cast<int*>(out.data() + param.offset) = param.constInt;
            continue;

        case ParamType::ConstInt2:
            *reinterpret_cast<Vec2I*>(out.data() + param.offset) = param.constInt2;
            continue;

        case ParamType::ConstInt3:
            *reinterpret_cast<Vec3I*>(out.data() + param.offset) = param.constInt3;
            continue;

        case ParamType::ConstInt4:
            *reinterpret_cast<Vec4I*>(out.data() + param.offset) = param.constInt4;
            continue;

        case ParamType::ModelMatrixIndex:
            *reinterpret_cast<uint*>(out.data() + param.offset) = modelMatsIndex + param.matrixIndex;
            continue;

        case ParamType::NormalMatrixIndex:
            *reinterpret_cast<uint*>(out.data() + param.offset) = normalMatsIndex + param.matrixIndex;
            continue;

        case ParamType::TextureIndex:
            *reinterpret_cast<uint*>(out.data() + param.offset) = *param.texture->get_bindless_descriptor_index();
            continue;

        case ParamType::AnimFloat:
            *reinterpret_cast<float*>(out.data() + param.offset) = *reinterpret_cast<const float*>(sample.data() + param.sampleOffset);
            continue;

        case ParamType::AnimFloat2:
            *reinterpret_cast<Vec2F*>(out.data() + param.offset) = *reinterpret_cast<const Vec2F*>(sample.data() + param.sampleOffset);
            continue;

        case ParamType::AnimFloat3:
            *reinterpret_cast<Vec3F*>(out.data() + param.offset) = *reinterpret_cast<const Vec3F*>(sample.data() + param.sampleOffset);
            continue;

        case ParamType::AnimFloat4:
            *reinterpret_cast<Vec4F*>(out.data() + param.offset) = *reinterpret_cast<const Vec4F*>(sample.data() + param.sampleOffset);
            continue;

        case ParamType::AnimInt:
            *reinterpret_cast<int*>(out.data() + param.offset) = *reinterpret_cast<const int*>(sample.data() + param.sampleOffset);
            continue;

        case ParamType::AnimInt2:
            *reinterpret_cast<Vec2I*>(out.data() + param.offset) = *reinterpret_cast<const Vec2I*>(sample.data() + param.sampleOffset);
            continue;

        case ParamType::AnimInt3:
            *reinterpret_cast<Vec3I*>(out.data() + param.offset) = *reinterpret_cast<const Vec3I*>(sample.data() + param.sampleOffset);
            continue;

        case ParamType::AnimInt4:
            *reinterpret_cast<Vec4I*>(out.data() + param.offset) = *reinterpret_cast<const Vec4I*>(sample.data() + param.sampleOffset);
            continue;

        case ParamType::AnimTextureTransform:
        {
            const Vec2F& offset = *reinterpret_cast<const Vec2F*>(sample.data() + param.sampleOffsets[0]);
            const float& rotation = *reinterpret_cast<const float*>(sample.data() + param.sampleOffsets[1]);
            const Vec2F& scale = *reinterpret_cast<const Vec2F*>(sample.data() + param.sampleOffsets[2]);

            // mat2x3 gets padded to a mat2x4
            float* matrix = reinterpret_cast<float*>(out.data() + param.offset);

            const float rotSin = std::sin(rotation);
            const float rotCos = std::cos(rotation);

            // note that offset is inverted, logically we are moving the texture, not the UVs
            matrix[0*4 + 0] = +rotCos * scale.x;
            matrix[0*4 + 1] = +rotSin * scale.y;
            matrix[0*4 + 2] = -offset.x;
            matrix[1*4 + 0] = -rotSin * scale.x;
            matrix[1*4 + 1] = +rotCos * scale.y;
            matrix[1*4 + 2] = -offset.y;

            continue;
        }

        } SQEE_UNREACHABLE();
    }
}
