#include <sqee/objects/DrawItem.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/misc/Parsing.hpp>
#include <sqee/objects/Armature.hpp>
#include <sqee/objects/Mesh.hpp>
#include <sqee/objects/Pipeline.hpp>
#include <sqee/objects/Texture.hpp>

#include <charconv> // hex numbers

using namespace sq;

//============================================================================//

std::vector<DrawItem> DrawItem::load_from_json(const String& path, const Armature& armature, MeshCache& meshes, PipelineCache& pipelines, TextureCache& textures)
{
    using PushConstantType = Pipeline::PushConstantType;
    using TrackType = Armature::TrackType;

    constexpr auto get_push_constant_type = [](ParamType type)
    {
        switch (type) {
        case ParamType::ConstFloat:        return PushConstantType::Float;
        case ParamType::ConstFloat2:       return PushConstantType::Vec2F;
        case ParamType::ConstFloat3:       return PushConstantType::Vec3F;
        case ParamType::ConstFloat4:       return PushConstantType::Vec4F;
        case ParamType::ConstInt:          return PushConstantType::Int;
        case ParamType::ConstInt2:         return PushConstantType::Vec2I;
        case ParamType::ConstInt3:         return PushConstantType::Vec3I;
        case ParamType::ConstInt4:         return PushConstantType::Vec4I;
        case ParamType::ConstPackedBits:   return PushConstantType::Uint;
        case ParamType::ModelMatIndex:     return PushConstantType::Uint;
        case ParamType::NormalMatIndex:    return PushConstantType::Uint;
        case ParamType::TextureIndex:      return PushConstantType::Uint;
        case ParamType::AnimFloat:         return PushConstantType::Float;
        case ParamType::AnimFloat2:        return PushConstantType::Vec2F;
        case ParamType::AnimFloat3:        return PushConstantType::Vec3F;
        case ParamType::AnimFloat4:        return PushConstantType::Vec4F;
        case ParamType::AnimInt:           return PushConstantType::Int;
        case ParamType::AnimInt2:          return PushConstantType::Vec2I;
        case ParamType::AnimInt3:          return PushConstantType::Vec3I;
        case ParamType::AnimInt4:          return PushConstantType::Vec4I;
        case ParamType::ConstTexTransform: return PushConstantType::Mat23F;
        case ParamType::AnimTexTransform:  return PushConstantType::Mat23F;
        } SQEE_UNREACHABLE();
    };

    const auto document = JsonDocument::parse_file(path);
    const auto json = document.root().as<JsonObject>();

    const auto jsonAssets = json["assets"].as<JsonObject>();

    const auto jsonTextures = jsonAssets["textures"].as<JsonObject>();
    const auto jsonMeshes = jsonAssets["meshes"].as<JsonObject>();
    const auto jsonPipelines = jsonAssets["pipelines"].as<JsonObject>();

    //--------------------------------------------------------//

    const auto jsonItems = json["items"].as<JsonArray>();

    std::vector<DrawItem> result;
    result.reserve(jsonItems.size());

    for (const auto [_, jItem] : jsonItems | views::json_as<JsonObject>)
    {
        DrawItem& item = result.emplace_back();

        if (const auto jCondition = jItem["condition"]; !jCondition.is_null())
            item.condition = jCondition.as<TinyString>();

        const auto jPipeline = jsonPipelines[jItem["pipeline"].as<StringView>()];
        item.pipeline = pipelines.acquire_safe(jPipeline.dump(false));
        if (!item.pipeline.good()) jPipeline.throw_with_context(item.pipeline.error());

        const auto jMesh = jsonMeshes[jItem["mesh"].as<StringView>()];
        item.mesh = meshes.acquire_safe(jMesh.as<String>());
        if (!item.mesh.good()) jMesh.throw_with_context(item.mesh.error());

        item.subMesh = item.mesh->json_as_sub_mesh_index(jItem["subMesh"]);

        item.order = jItem["order"].as<int16_t>();

        if (const auto jVisibility = jItem["visibility"]; !jVisibility.is_null())
        {
            const auto [blockName, trackName] = split_string<'/'>(jVisibility.as<StringView>());
            if (blockName.empty() || trackName.empty()) jVisibility.throw_with_context("invalid block/track string");

            const int16_t trackIndex = armature.get_block_track_index(blockName, trackName);
            if (trackIndex < 0) jVisibility.throw_with_context("block or track does not exist");

            const auto& trackInfo = armature.get_track_infos()[trackIndex];

            if (trackInfo.type == TrackType::Float)
            {
                item.visDataType = VisDataType::Float;
                item.visSampleOffset = trackInfo.offset;
            }
            else if (trackInfo.type == TrackType::Float4)
            {
                item.visDataType = VisDataType::Float;
                item.visSampleOffset = trackInfo.offset + 12u;
            }
            else if (trackInfo.type == TrackType::Int)
            {
                item.visDataType = VisDataType::Int;
                item.visSampleOffset = trackInfo.offset;
            }
            else jVisibility.throw_with_context("track has type {}, but must be Float/Float4/Int", trackInfo.type);
        }

        const auto jParams = jItem["params"].as<JsonObject>();
        item.params.reserve(item.pipeline->get_push_constants().size());

        for (const auto& pushConstant : item.pipeline->get_push_constants())
        {
            DrawItem::Param& param = item.params.emplace_back();
            const auto jParam = jParams[pushConstant.name].as<JsonObject>();

            const auto jType = jParam["type"]; param.type = jType.as<ParamType>();
            if (get_push_constant_type(param.type) != pushConstant.type)
                jType.throw_with_context("param of type {} cannot map to push constant of type {}", param.type, pushConstant.type);

            param.offset = pushConstant.offset;

            switch (param.type) {

            case ParamType::ConstFloat: param.constFloat = jParam["value"].as<float>(); continue;
            case ParamType::ConstFloat2: param.constFloat2 = jParam["value"].as<Vec2F>(); continue;
            case ParamType::ConstFloat3: param.constFloat3 = jParam["value"].as<Vec3F>(); continue;
            case ParamType::ConstFloat4: param.constFloat4 = jParam["value"].as<Vec4F>(); continue;
            case ParamType::ConstInt: param.constInt = jParam["value"].as<int>(); continue;
            case ParamType::ConstInt2: param.constInt2 = jParam["value"].as<Vec2I>(); continue;
            case ParamType::ConstInt3: param.constInt3 = jParam["value"].as<Vec3I>(); continue;
            case ParamType::ConstInt4: param.constInt4 = jParam["value"].as<Vec4I>(); continue;

            case ParamType::ConstPackedBits:
            {
                const auto jValue = jParam["value"]; const auto hex = jValue.as<StringView>();
                if (hex.starts_with("0x") && hex.length() == 10)
                {
                    const auto result = std::from_chars(hex.data() + 2, hex.data() + hex.length(), param.constPackedBits, 16);
                    if (result.ptr == hex.data() + hex.length() && result.ec == std::errc())
                        continue;
                }
                jValue.throw_with_context("could not parse as 32bit hex number");
            }

            case ParamType::ModelMatIndex:
            case ParamType::NormalMatIndex:
            {
                param.matrixIndex = uint(armature.json_as_bone_index(jParam["bone"]) + 1);
                continue;
            }

            case ParamType::TextureIndex:
            {
                const auto jTexture = jsonTextures[jParam["texture"].as<StringView>()];
                param.texture = textures.acquire_safe(jTexture.as<String>());
                if (!param.texture.good()) jTexture.throw_with_context(param.texture.error());
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
                const auto jTrack = jParam["track"];

                const auto [blockName, trackName] = split_string<'/'>(jTrack.as<StringView>());
                if (blockName.empty() || trackName.empty()) jTrack.throw_with_context("invalid block/track string");

                const int16_t trackIndex = armature.get_block_track_index(blockName, trackName);
                if (trackIndex < 0) jTrack.throw_with_context("no block/track '{}/{}'", blockName, trackName);

                const auto& trackInfo = armature.get_track_infos()[trackIndex];

                if (TrackType(uint8_t(param.type) - uint8_t(ParamType::AnimFloat)) != trackInfo.type)
                    jType.throw_with_context("param of type {} cannot use track of type {}", param.type, trackInfo.type);

                param.sampleOffset = trackInfo.offset;

                continue;
            }

            case ParamType::ConstTexTransform:
            {
                const auto jValues = jParam["values"].as<JsonArray>();

                const Vec2F offset = {jValues[0].as_auto(), jValues[1].as_auto()};
                const float rotation = jValues[2].as_auto();
                const Vec2F scale = {jValues[3].as_auto(), jValues[4].as_auto()};

                const float rotSin = std::sin(rotation);
                const float rotCos = std::cos(rotation);

                param.constFloat2x3[0][0] = +rotCos * scale.x;
                param.constFloat2x3[0][1] = +rotSin * scale.y;
                param.constFloat2x3[0][2] = -offset.x;
                param.constFloat2x3[1][0] = -rotSin * scale.x;
                param.constFloat2x3[1][1] = +rotCos * scale.y;
                param.constFloat2x3[1][2] = -offset.y;

                continue;
            }

            case ParamType::AnimTexTransform:
            {
                const auto jTracks = jParam["tracks"];

                const auto [blockName, trackName0, trackName1, trackName2] = split_string<'/', '/', '/'>(jTracks.as<StringView>());
                if (blockName.empty() || trackName0.empty() || trackName1.empty() || trackName2.empty())
                    jTracks.throw_with_context("invalid block/offset/rotation/scale string");

                const int16_t trackIndex0 = armature.get_block_track_index(blockName, trackName0);
                if (trackIndex0 < 0) jTracks.throw_with_context("no block/track '{}/{}'", blockName, trackName0);
                const int16_t trackIndex1 = armature.get_block_track_index(blockName, trackName1);
                if (trackIndex1 < 0) jTracks.throw_with_context("no block/track '{}/{}'", blockName, trackName1);
                const int16_t trackIndex2 = armature.get_block_track_index(blockName, trackName2);
                if (trackIndex2 < 0) jTracks.throw_with_context("no block/track '{}/{}'", blockName, trackName2);

                const auto& trackInfo0 = armature.get_track_infos()[trackIndex0];
                const auto& trackInfo1 = armature.get_track_infos()[trackIndex1];
                const auto& trackInfo2 = armature.get_track_infos()[trackIndex2];

                if (trackInfo0.type != TrackType::Float2)
                    jTracks.throw_with_context("1st track has type {}, but offset must be Float2", trackInfo0.type);
                if (trackInfo1.type != TrackType::Angle)
                    jTracks.throw_with_context("2nd track has type {}, but rotation must be Angle", trackInfo1.type);
                if (trackInfo2.type != TrackType::Float2)
                    jTracks.throw_with_context("3rd track has type {}, but scale must be Float2", trackInfo2.type);

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

bool DrawItem::check_visibility(const AnimSample& sample) const
{
    switch (visDataType) {

    case VisDataType::None:
        return true;

    case VisDataType::Float:
        return *reinterpret_cast<const float*>(sample.data() + visSampleOffset) != 0.f;

    case VisDataType::Int:
        return *reinterpret_cast<const int*>(sample.data() + visSampleOffset) != 0;

    } SQEE_UNREACHABLE();
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

        case ParamType::ConstPackedBits:
            *reinterpret_cast<uint32_t*>(out.data() + param.offset) = param.constPackedBits;
            continue;

        case ParamType::ModelMatIndex:
            *reinterpret_cast<uint*>(out.data() + param.offset) = modelMatsIndex + param.matrixIndex;
            continue;

        case ParamType::NormalMatIndex:
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

        case ParamType::ConstTexTransform:
        {
            auto& matrix = *reinterpret_cast<float(*)[2][4]>(out.data() + param.offset);

            matrix[0][0] = param.constFloat2x3[0][0];
            matrix[0][1] = param.constFloat2x3[0][1];
            matrix[0][2] = param.constFloat2x3[0][2];
            matrix[1][0] = param.constFloat2x3[1][0];
            matrix[1][1] = param.constFloat2x3[1][1];
            matrix[1][2] = param.constFloat2x3[1][2];

            continue;
        }

        case ParamType::AnimTexTransform:
        {
            auto& matrix = *reinterpret_cast<float(*)[2][4]>(out.data() + param.offset);

            const Vec2F& offset = *reinterpret_cast<const Vec2F*>(sample.data() + param.sampleOffsets[0]);
            const float& rotation = *reinterpret_cast<const float*>(sample.data() + param.sampleOffsets[1]);
            const Vec2F& scale = *reinterpret_cast<const Vec2F*>(sample.data() + param.sampleOffsets[2]);

            const float rotSin = std::sin(rotation);
            const float rotCos = std::cos(rotation);

            matrix[0][0] = +rotCos * scale.x;
            matrix[0][1] = +rotSin * scale.y;
            matrix[0][2] = -offset.x;
            matrix[1][0] = -rotSin * scale.x;
            matrix[1][1] = +rotCos * scale.y;
            matrix[1][2] = -offset.y;

            continue;
        }

        } SQEE_UNREACHABLE();
    }
}
