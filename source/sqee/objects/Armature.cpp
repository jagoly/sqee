#include <sqee/objects/Armature.hpp>

#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/maths/Functions.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/misc/Parsing.hpp>

using namespace sq;

// todo:
//  Implement binary formats for armatures and animations. The purpose of these
//  would be to reduce file size, and to allow much faster loading. Once I have
//  those, I may want to obsolete the text based format entirely.

thread_local std::vector<Mat4F> gTempMats;

//============================================================================//

void Armature::load_from_file(const String& path)
{
    const JsonValue json = sq::parse_json_from_file(path);

    const auto& jBones = json.at("bones").get_ref<const JsonValue::array_t&>();
    const auto& jBlocks = json.at("blocks").get_ref<const JsonValue::object_t&>();

    mRestSample.resize(size_t(json.at("animSampleSize")));

    mBoneNames.reserve(jBones.size());
    mBoneInfos.reserve(jBones.size());

    mBaseMats.reserve(jBones.size());
    mInverseMats.reserve(jBones.size());

    mBlockNames.reserve(jBlocks.size());

    uint32_t dataOffset = 0u;

    //--------------------------------------------------------//

    for (size_t boneIndex = 0u; boneIndex < jBones.size(); ++boneIndex)
    {
        const auto& jBone = jBones[boneIndex].get_ref<const JsonValue::object_t&>();

        mBoneNames.emplace_back(jBone.at("name").get_ref<const String&>());

        BoneInfo& info = mBoneInfos.emplace_back();

        info.parent = [&]()
        {
            const auto& jParent = jBone.at("parent");

            if (jParent.is_null() == true)
                return int8_t(-1);

            const auto& jParentStr = jParent.get_ref<const String&>();

            // parent can be any bone with a lower index
            for (size_t parentIndex = 0u; parentIndex < boneIndex; ++parentIndex)
                if (mBoneNames[parentIndex] == jParentStr)
                    return int8_t(parentIndex);

            SQEE_THROW("invalid parent '{}'", jParentStr);
        }();

        for (const auto& jFlag : jBone.at("flags").get_ref<const JsonValue::array_t&>())
        {
            const auto& jFlagStr = jFlag.get_ref<const String&>();
            if (jFlagStr == "Billboard") info.flags |= BoneFlag::Billboard;
            else SQEE_THROW("invalid flag '{}'", jFlagStr);
        }

        Bone& restBone = *reinterpret_cast<Bone*>(mRestSample.data() + dataOffset);

        mTrackNames.emplace_back("offset");
        mTrackInfos.push_back({int8_t(boneIndex), int8_t(-1), TrackType::Float3, dataOffset});
        dataOffset += sizeof(Vec3F);

        mTrackNames.emplace_back("rotation");
        mTrackInfos.push_back({int8_t(boneIndex), int8_t(-1), TrackType::Quaternion, dataOffset});
        dataOffset += sizeof(QuatF);

        mTrackNames.emplace_back("scale");
        mTrackInfos.push_back({int8_t(boneIndex), int8_t(-1), TrackType::Float3, dataOffset});
        dataOffset += sizeof(Vec3F);

        jBone.at("offset").get_to(restBone.offset);
        jBone.at("rotation").get_to(restBone.rotation);
        jBone.at("scale").get_to(restBone.scale);

        restBone.rotation = maths::normalize(restBone.rotation);

        mBaseMats.emplace_back(maths::transform(restBone.offset, restBone.rotation, restBone.scale));
        if (info.parent != -1)
            mBaseMats.back() = mBaseMats[info.parent] * mBaseMats.back();

        mInverseMats.emplace_back(maths::affine_inverse(mBaseMats.back()));
    }

    //--------------------------------------------------------//

    for (const auto& [jBlockKey, jBlock] : jBlocks)
    {
        const size_t blockIndex = mBlockNames.size();
        mBlockNames.emplace_back(jBlockKey);

        const auto& jTracks = jBlock.get_ref<const JsonValue::object_t&>();

        for (const auto& [jTrackKey, jTrack] : jTracks)
        {
            mTrackNames.emplace_back(jTrackKey);

            const auto& jTrackType = jTrack.at("type").get_ref<const String&>();
            const auto& jTrackDefault = jTrack.at("default");

            if (jTrackType == "Float")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Float, dataOffset});
                jTrackDefault.at(0).get_to(*reinterpret_cast<float*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(float);
            }
            else if (jTrackType == "Float2")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Float2, dataOffset});
                jTrackDefault.get_to(*reinterpret_cast<Vec2F*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(Vec2F);
            }
            else if (jTrackType == "Float3")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Float3, dataOffset});
                jTrackDefault.get_to(*reinterpret_cast<Vec3F*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(Vec3F);
            }
            else if (jTrackType == "Float4")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Float4, dataOffset});
                jTrackDefault.get_to(*reinterpret_cast<Vec4F*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(Vec4F);
            }
            else if (jTrackType == "Int")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Int, dataOffset});
                jTrackDefault.at(0).get_to(*reinterpret_cast<int*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(int);
            }
            else if (jTrackType == "Int2")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Int2, dataOffset});
                jTrackDefault.get_to(*reinterpret_cast<Vec2I*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(Vec2I);
            }
            else if (jTrackType == "Int3")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Int3, dataOffset});
                jTrackDefault.get_to(*reinterpret_cast<Vec3I*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(Vec3I);
            }
            else if (jTrackType == "Int4")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Int4, dataOffset});
                jTrackDefault.get_to(*reinterpret_cast<Vec4I*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(Vec4I);
            }
            else if (jTrackType == "Angle")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Angle, dataOffset});
                jTrackDefault.at(0).get_to(*reinterpret_cast<float*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(float);
            }
            else if (jTrackType == "Quaternion")
            {
                mTrackInfos.push_back({int8_t(-1), int8_t(blockIndex), TrackType::Quaternion, dataOffset});
                jTrackDefault.get_to(*reinterpret_cast<QuatF*>(mRestSample.data() + dataOffset));
                dataOffset += sizeof(QuatF);
            }
            else SQEE_THROW("invalid track type");
        }
    }

    //--------------------------------------------------------//

    mTrackNames.shrink_to_fit();
    mTrackInfos.shrink_to_fit();

    if (dataOffset != mRestSample.size())
        SQEE_THROW("animSampleSize does not match tracks");
}

//============================================================================//

uint8_t Armature::get_bone_index(TinyString name) const
{
    for (size_t i = 0u; i < mBoneNames.size(); ++i)
        if (mBoneNames[i] == name)
            return uint8_t(i);

    SQEE_THROW("armature has no bone named '{}'", name);
}

uint8_t Armature::get_block_index(TinyString name) const
{
    for (size_t i = 0u; i < mBlockNames.size(); ++i)
        if (mBlockNames[i] == name)
            return uint8_t(i);

    SQEE_THROW("armature has no block named '{}'", name);
}

uint16_t Armature::get_block_track_index(uint8_t blockIndex, TinyString trackName) const
{
    SQASSERT(blockIndex < mBlockNames.size(), "invalid block index");

    for (size_t i = 0u; i < mTrackInfos.size(); ++i)
        if (mTrackInfos[i].blockIndex == int8_t(blockIndex) && mTrackNames[i] == trackName)
            return uint16_t(i);

    SQEE_THROW("armature block '{}' has no track named '{}'", mBlockNames[blockIndex], trackName);
}

//============================================================================//

int8_t Armature::bone_from_json(const JsonValue& json) const
{
    if (json.is_null() == true) return int8_t(-1);
    return int8_t(get_bone_index(json.get_ref<const String&>()));
}

JsonValue Armature::bone_to_json(int8_t index) const
{
    if (index == -1) return nullptr;
    return StringView(mBoneNames.at(index));
}

//============================================================================//

Animation Armature::load_animation_from_file(const String& path) const
{
    if (auto text = try_read_text_from_file(path + ".sqa"))
        return impl_load_animation_text(std::move(*text));

    else if (auto text = try_read_text_from_file(path))
        return impl_load_animation_text(std::move(*text));

    else SQEE_THROW("could not find animation '{}'", path);
}

//============================================================================//

Animation Armature::impl_load_animation_text(String&& text) const
{
    Animation result;
    result.tracks.reserve(mTrackInfos.size());

    enum class Section { None, Header, BoneTracks, BlockTracks };
    Section section = Section::None;

    Animation::Track* track = nullptr;
    TrackType trackType = {};

    std::byte* bytePtr = nullptr;

    //--------------------------------------------------------//

    const auto assert_num_tokens = [](const std::vector<std::string_view>& tokens, size_t lineNum, size_t expected)
    {
        if (tokens.size() != expected)
            SQEE_THROW("{}: wrong number of tokens for track type", lineNum);
    };

    const auto initialise_track = [&](uint16_t trackIndex, const std::vector<std::string_view>& tokens, size_t lineNum)
    {
        if (size_t(trackIndex) != result.tracks.size())
            SQEE_THROW("{}: tracks out of order", lineNum);

        trackType = mTrackInfos[trackIndex].type;

        if (tokens.size() == 3u) // animated value
        {
            if      (trackType == TrackType::Float)      track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(float));
            else if (trackType == TrackType::Float2)     track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(Vec2F));
            else if (trackType == TrackType::Float3)     track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(Vec3F));
            else if (trackType == TrackType::Float4)     track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(Vec4F));
            else if (trackType == TrackType::Int)        track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(int));
            else if (trackType == TrackType::Int2)       track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(Vec2I));
            else if (trackType == TrackType::Int3)       track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(Vec3I));
            else if (trackType == TrackType::Int4)       track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(Vec4I));
            else if (trackType == TrackType::Angle)      track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(float));
            else if (trackType == TrackType::Quaternion) track = &result.tracks.emplace_back(bytePtr, result.frameCount * sizeof(QuatF));
            else    SQEE_UNREACHABLE();
        }
        else if (trackType == TrackType::Float)
        {
            assert_num_tokens(tokens, lineNum, 4u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(float));
            parse_number(*(reinterpret_cast<float*&>(bytePtr)++), tokens[3]);
        }
        else if (trackType == TrackType::Float2)
        {
            assert_num_tokens(tokens, lineNum, 5u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(Vec2F));
            parse_numbers(*(reinterpret_cast<Vec2F*&>(bytePtr)++), tokens[3], tokens[4]);
        }
        else if (trackType == TrackType::Float3)
        {
            assert_num_tokens(tokens, lineNum, 6u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(Vec3F));
            parse_numbers(*(reinterpret_cast<Vec3F*&>(bytePtr)++), tokens[3], tokens[4], tokens[5]);
        }
        else if (trackType == TrackType::Float4)
        {
            assert_num_tokens(tokens, lineNum, 7u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(Vec4F));
            parse_numbers(*(reinterpret_cast<Vec4F*&>(bytePtr)++), tokens[3], tokens[4], tokens[5], tokens[6]);
        }
        else if (trackType == TrackType::Int)
        {
            assert_num_tokens(tokens, lineNum, 4u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(int));
            parse_number(*(reinterpret_cast<int*&>(bytePtr)++), tokens[3]);
        }
        else if (trackType == TrackType::Int2)
        {
            assert_num_tokens(tokens, lineNum, 5u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(Vec2I));
            parse_numbers(*(reinterpret_cast<Vec2I*&>(bytePtr)++), tokens[3], tokens[4]);
        }
        else if (trackType == TrackType::Int3)
        {
            assert_num_tokens(tokens, lineNum, 6u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(Vec3I));
            parse_numbers(*(reinterpret_cast<Vec3I*&>(bytePtr)++), tokens[3], tokens[4], tokens[5]);
        }
        else if (trackType == TrackType::Int4)
        {
            assert_num_tokens(tokens, lineNum, 7u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(Vec4I));
            parse_numbers(*(reinterpret_cast<Vec4I*&>(bytePtr)++), tokens[3], tokens[4], tokens[5], tokens[6]);
        }
        else if (trackType == TrackType::Angle)
        {
            assert_num_tokens(tokens, lineNum, 4u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(float));
            parse_number(*(reinterpret_cast<float*&>(bytePtr)++), tokens[3]);
        }
        else if (trackType == TrackType::Quaternion)
        {
            assert_num_tokens(tokens, lineNum, 7u);
            track = &result.tracks.emplace_back(bytePtr, sizeof(QuatF));
            parse_numbers_normalize(*(reinterpret_cast<QuatF*&>(bytePtr)++), tokens[3], tokens[4], tokens[5], tokens[6]);
        }
        else SQEE_UNREACHABLE();
    };

    const auto append_track_frame = [&](const std::vector<std::string_view>& tokens, size_t lineNum)
    {
        if (bytePtr >= track->data + track->size)
            SQEE_THROW("{}: too many frames for track", lineNum);

        switch (trackType) {

        case TrackType::Float:
            assert_num_tokens(tokens, lineNum, 1u);
            parse_number(*(reinterpret_cast<float*&>(bytePtr)++), tokens[0]);
            return;

        case TrackType::Float2:
            assert_num_tokens(tokens, lineNum, 2u);
            parse_numbers(*(reinterpret_cast<Vec2F*&>(bytePtr)++), tokens[0], tokens[1]);
            return;

        case TrackType::Float3:
            assert_num_tokens(tokens, lineNum, 3u);
            parse_numbers(*(reinterpret_cast<Vec3F*&>(bytePtr)++), tokens[0], tokens[1], tokens[2]);
            return;

        case TrackType::Float4:
            assert_num_tokens(tokens, lineNum, 4u);
            parse_numbers(*(reinterpret_cast<Vec4F*&>(bytePtr)++), tokens[0], tokens[1], tokens[2], tokens[3]);
            return;

        case TrackType::Int:
            assert_num_tokens(tokens, lineNum, 1u);
            parse_number(*(reinterpret_cast<int*&>(bytePtr)++), tokens[0]);
            return;

        case TrackType::Int2:
            assert_num_tokens(tokens, lineNum, 2u);
            parse_numbers(*(reinterpret_cast<Vec2I*&>(bytePtr)++), tokens[0], tokens[1]);
            return;

        case TrackType::Int3:
            assert_num_tokens(tokens, lineNum, 3u);
            parse_numbers(*(reinterpret_cast<Vec3I*&>(bytePtr)++), tokens[0], tokens[1], tokens[2]);
            return;

        case TrackType::Int4:
            assert_num_tokens(tokens, lineNum, 4u);
            parse_numbers(*(reinterpret_cast<Vec4I*&>(bytePtr)++), tokens[0], tokens[1], tokens[2], tokens[3]);
            return;

        case TrackType::Angle:
            assert_num_tokens(tokens, lineNum, 1u);
            parse_number(*(reinterpret_cast<float*&>(bytePtr)++), tokens[0]);
            return;

        case TrackType::Quaternion:
            assert_num_tokens(tokens, lineNum, 4u);
            parse_numbers_normalize(*(reinterpret_cast<QuatF*&>(bytePtr)++), tokens[0], tokens[1], tokens[2], tokens[3]);
            return;

        } SQEE_UNREACHABLE();
    };

    //--------------------------------------------------------//

    for (const auto& [tokens, lineNum] : TokenisedFile::from_string(std::move(text)).lines)
    {
        if (tokens[0].front() == '#') continue;

        if (tokens[0] == "SECTION")
        {
            if      (tokens[1] == "Header")      section = Section::Header;
            else if (tokens[1] == "BoneTracks")  section = Section::BoneTracks;
            else if (tokens[1] == "BlockTracks") section = Section::BlockTracks;
            else    SQEE_THROW("{}: invalid section", lineNum);
        }

        else if (section == Section::Header)
        {
            if (tokens[0] == "BoneCount")
            {
                const auto boneCount = parse_number<size_t>(tokens[1]);
                if (boneCount != get_bone_count())
                    SQEE_THROW("{}: bone count mismatch", lineNum);
            }

            else if (tokens[0] == "BlockTrackCount")
            {
                const auto blockTrackCount = parse_number<size_t>(tokens[1]);
                if (get_bone_count() * 3u + blockTrackCount != mTrackInfos.size())
                    SQEE_THROW("{}: block track count mismatch", lineNum);
            }

            else if (tokens[0] == "ByteCount")
            {
                result.bytes = std::vector<std::byte>(parse_number<size_t>(tokens[1]));
                bytePtr = result.bytes.data();
            }

            else if (tokens[0] == "FrameCount")
                result.frameCount = parse_number<uint>(tokens[1]);

            else SQEE_THROW("{}: invalid header key", lineNum);
        }

        else if (section == Section::BoneTracks)
        {
            if (tokens[0] == "TRACK")
            {
                const uint8_t boneIndex = get_bone_index(tokens[1]);

                if      (tokens[2] == "offset")   initialise_track(uint16_t(boneIndex*3+0), tokens, lineNum);
                else if (tokens[2] == "rotation") initialise_track(uint16_t(boneIndex*3+1), tokens, lineNum);
                else if (tokens[2] == "scale")    initialise_track(uint16_t(boneIndex*3+2), tokens, lineNum);
                else    SQEE_THROW("{}: invalid bone track", lineNum);
            }

            else append_track_frame(tokens, lineNum);
        }

        else if (section == Section::BlockTracks)
        {
            if (tokens[0] == "TRACK")
            {
                const uint8_t blockIndex = get_block_index(tokens[1]);
                const uint16_t trackIndex = get_block_track_index(uint8_t(blockIndex), tokens[2]);

                initialise_track(trackIndex, tokens, lineNum);
            }

            else append_track_frame(tokens, lineNum);
        }

        else SQEE_THROW("missing SECTION");
    }

    //--------------------------------------------------------//

    return result;
}

//============================================================================//

Animation Armature::make_null_animation(uint frameCount) const
{
    Animation result;

    result.frameCount = frameCount;

    result.bytes = mRestSample;
    result.tracks.reserve(mTrackInfos.size());

    std::byte* bytePtr = result.bytes.data();

    for (const TrackInfo& info : mTrackInfos)
    {
        if      (info.type == TrackType::Float)      result.tracks.emplace_back(bytePtr, sizeof(float));
        else if (info.type == TrackType::Float2)     result.tracks.emplace_back(bytePtr, sizeof(Vec2F));
        else if (info.type == TrackType::Float3)     result.tracks.emplace_back(bytePtr, sizeof(Vec3F));
        else if (info.type == TrackType::Float4)     result.tracks.emplace_back(bytePtr, sizeof(Vec4F));
        else if (info.type == TrackType::Int)        result.tracks.emplace_back(bytePtr, sizeof(int));
        else if (info.type == TrackType::Int2)       result.tracks.emplace_back(bytePtr, sizeof(Vec2I));
        else if (info.type == TrackType::Int3)       result.tracks.emplace_back(bytePtr, sizeof(Vec3I));
        else if (info.type == TrackType::Int4)       result.tracks.emplace_back(bytePtr, sizeof(Vec4I));
        else if (info.type == TrackType::Angle)      result.tracks.emplace_back(bytePtr, sizeof(float));
        else if (info.type == TrackType::Quaternion) result.tracks.emplace_back(bytePtr, sizeof(QuatF));
        else    SQEE_UNREACHABLE();

        bytePtr += result.tracks.back().size;
    }

    return result;
}

//============================================================================//

void Armature::compute_sample(const Animation& animation, float time, AnimSample& out) const
{
    SQASSERT(animation.tracks.size() == mTrackInfos.size(), "track count mismatch");
    SQASSERT(out.size() == mRestSample.size(), "sample size mismatch");

    const Animation::SampleTime sample = animation.compute_sample_time(time);

    for (size_t i = 0u; i < mTrackInfos.size(); ++i)
    {
        const TrackInfo& info = mTrackInfos[i];
        const Animation::Track& track = animation.tracks[i];
        std::byte* const ptr = out.data() + info.offset;

        switch (info.type) {

        case TrackType::Float:
            *reinterpret_cast<float*>(ptr) = track.size == sizeof(float) ?
                *reinterpret_cast<const float*>(track.data) :
                maths::mix (
                    reinterpret_cast<const float*>(track.data)[sample.frameA],
                    reinterpret_cast<const float*>(track.data)[sample.frameB],
                    sample.factor
                );
            continue;

        case TrackType::Float2:
            *reinterpret_cast<Vec2F*>(ptr) = track.size == sizeof(Vec2F) ?
                *reinterpret_cast<const Vec2F*>(track.data) :
                maths::mix (
                    reinterpret_cast<const Vec2F*>(track.data)[sample.frameA],
                    reinterpret_cast<const Vec2F*>(track.data)[sample.frameB],
                    sample.factor
                );
            continue;

        case TrackType::Float3:
            *reinterpret_cast<Vec3F*>(ptr) = track.size == sizeof(Vec3F) ?
                *reinterpret_cast<const Vec3F*>(track.data) :
                maths::mix (
                    reinterpret_cast<const Vec3F*>(track.data)[sample.frameA],
                    reinterpret_cast<const Vec3F*>(track.data)[sample.frameB],
                    sample.factor
                );
            continue;

        case TrackType::Float4:
            *reinterpret_cast<Vec4F*>(ptr) = track.size == sizeof(Vec4F) ?
                *reinterpret_cast<const Vec4F*>(track.data) :
                maths::mix (
                    reinterpret_cast<const Vec4F*>(track.data)[sample.frameA],
                    reinterpret_cast<const Vec4F*>(track.data)[sample.frameB],
                    sample.factor
                );
            continue;

        case TrackType::Int:
            *reinterpret_cast<int*>(ptr) = track.size == sizeof(int) ?
                *reinterpret_cast<const int*>(track.data) :
                reinterpret_cast<const int*>(track.data)[sample.frameA];
            continue;

        case TrackType::Int2:
            *reinterpret_cast<Vec2I*>(ptr) = track.size == sizeof(Vec2I) ?
                *reinterpret_cast<const Vec2I*>(track.data) :
                reinterpret_cast<const Vec2I*>(track.data)[sample.frameA];
            continue;

        case TrackType::Int3:
            *reinterpret_cast<Vec3I*>(ptr) = track.size == sizeof(Vec3I) ?
                *reinterpret_cast<const Vec3I*>(track.data) :
                reinterpret_cast<const Vec3I*>(track.data)[sample.frameA];
            continue;

        case TrackType::Int4:
            *reinterpret_cast<Vec4I*>(ptr) = track.size == sizeof(Vec4I) ?
                *reinterpret_cast<const Vec4I*>(track.data) :
                reinterpret_cast<const Vec4I*>(track.data)[sample.frameA];
            continue;

        case TrackType::Angle:
            *reinterpret_cast<float*>(ptr) = track.size == sizeof(float) ?
                *reinterpret_cast<const float*>(track.data) :
                maths::mix_radians (
                    reinterpret_cast<const float*>(track.data)[sample.frameA],
                    reinterpret_cast<const float*>(track.data)[sample.frameB],
                    sample.factor
                );
            continue;

        case TrackType::Quaternion:
            *reinterpret_cast<QuatF*>(ptr) = track.size == sizeof(QuatF) ?
                *reinterpret_cast<const QuatF*>(track.data) :
                maths::slerp (
                    reinterpret_cast<const QuatF*>(track.data)[sample.frameA],
                    reinterpret_cast<const QuatF*>(track.data)[sample.frameB],
                    sample.factor
                );
            continue;

        } SQEE_UNREACHABLE();
    }
}

void Armature::blend_samples(const AnimSample& a, const AnimSample& b, float factor, AnimSample& out) const
{
    SQASSERT(a.size() == mRestSample.size(), "sample size mismatch");
    SQASSERT(b.size() == mRestSample.size(), "sample size mismatch");
    SQASSERT(out.size() == mRestSample.size(), "sample size mismatch");

    for (const TrackInfo& info : mTrackInfos)
    {
        const std::byte* const ptrA = a.data() + info.offset;
        const std::byte* const ptrB = b.data() + info.offset;
        std::byte* const ptrOut = out.data() + info.offset;

        switch (info.type) {

        case TrackType::Float:
            *reinterpret_cast<float*>(ptrOut) = maths::mix (
                *reinterpret_cast<const float*>(ptrA), *reinterpret_cast<const float*>(ptrB), factor
            );
            continue;

        case TrackType::Float2:
            *reinterpret_cast<Vec2F*>(ptrOut) = maths::mix (
                *reinterpret_cast<const Vec2F*>(ptrA), *reinterpret_cast<const Vec2F*>(ptrB), factor
            );
            continue;

        case TrackType::Float3:
            *reinterpret_cast<Vec3F*>(ptrOut) = maths::mix (
                *reinterpret_cast<const Vec3F*>(ptrA), *reinterpret_cast<const Vec3F*>(ptrB), factor
            );
            continue;

        case TrackType::Float4:
            *reinterpret_cast<Vec4F*>(ptrOut) = maths::mix (
                *reinterpret_cast<const Vec4F*>(ptrA), *reinterpret_cast<const Vec4F*>(ptrB), factor
            );
            continue;

        case TrackType::Int:
            *reinterpret_cast<int*>(ptrOut) = *reinterpret_cast<const int*>(ptrA);
            continue;

        case TrackType::Int2:
            *reinterpret_cast<Vec2I*>(ptrOut) = *reinterpret_cast<const Vec2I*>(ptrA);
            continue;

        case TrackType::Int3:
            *reinterpret_cast<Vec3I*>(ptrOut) = *reinterpret_cast<const Vec3I*>(ptrA);
            continue;

        case TrackType::Int4:
            *reinterpret_cast<Vec4I*>(ptrOut) = *reinterpret_cast<const Vec4I*>(ptrA);
            continue;

        case TrackType::Angle:
            *reinterpret_cast<float*>(ptrOut) = maths::mix_radians (
                *reinterpret_cast<const float*>(ptrA), *reinterpret_cast<const float*>(ptrB), factor
            );
            continue;

        case TrackType::Quaternion:
            *reinterpret_cast<QuatF*>(ptrOut) = maths::slerp (
                *reinterpret_cast<const QuatF*>(ptrA), *reinterpret_cast<const QuatF*>(ptrB), factor
            );
            continue;

        } SQEE_UNREACHABLE();
    }
}

//============================================================================//

Mat4F Armature::compute_bone_matrix(const AnimSample& sample, uint8_t index) const
{
    SQASSERT(sample.size() == mRestSample.size(), "sample size mismatch");
    SQASSERT(index < get_bone_count(), "invalid bone index");

    const Bone& bone = reinterpret_cast<const Bone*>(sample.data())[index];
    Mat4F result = maths::transform(bone.offset, bone.rotation, bone.scale);

    for (int8_t pIndex = mBoneInfos[index].parent; pIndex != -1; pIndex = mBoneInfos[pIndex].parent)
    {
        const Bone& pBone = reinterpret_cast<const Bone*>(sample.data())[pIndex];
        result = maths::transform(pBone.offset, pBone.rotation, pBone.scale) * result;
    }

    return result;
}

//============================================================================//

std::vector<Mat4F> Armature::compute_skeleton_matrices(const AnimSample& sample) const
{
    SQASSERT(sample.size() == mRestSample.size(), "sample size mismatch");

    std::vector<Mat4F> result;
    result.reserve(get_bone_count());

    for (size_t i = 0u; i < get_bone_count(); ++i)
    {
        const Bone& bone = reinterpret_cast<const Bone*>(sample.data())[i];
        const int8_t parentIndex = mBoneInfos[i].parent;

        Mat4F relMatrix = maths::transform(bone.offset, bone.rotation, bone.scale);

        // todo: billboards

        if (parentIndex < 0) result.push_back(relMatrix);
        else result.push_back(result[parentIndex] * relMatrix);
    }

    return result;
}

//============================================================================//

Mat4F Armature::compute_model_matrix(const AnimSample& sample, Mat4F modelMatrix, uint8_t index) const
{
    SQASSERT(sample.size() == mRestSample.size(), "sample size mismatch");
    SQASSERT(index < get_bone_count(), "invalid index");

    const auto bones = reinterpret_cast<const Bone*>(sample.data());

    Mat4F result = maths::transform(bones[index].offset, bones[index].rotation, bones[index].scale);

    // apply the transforms of all parents
    for (int8_t i = mBoneInfos[index].parent; i != -1; i = mBoneInfos[i].parent)
        result = maths::transform(bones[i].offset, bones[i].rotation, bones[i].scale) * result;

    return modelMatrix * result * mInverseMats[index];
}

//============================================================================//

void Armature::compute_model_matrices (
    const AnimSample& sample, Mat4F viewMatrix, Mat4F invViewMatrix, Mat4F modelMatrix, Vec2F billboardScale, Mat34F* modelMats, size_t len
) const
{
    SQASSERT(sample.size() == mRestSample.size(), "sample size mismatch");
    SQASSERT(len == get_bone_count(), "bone count mismatch");

    // cache matrices as we go for use as parents
    gTempMats.clear();
    gTempMats.reserve(get_bone_count());

    for (size_t i = 0u; i < get_bone_count(); ++i)
    {
        const Bone& bone = reinterpret_cast<const Bone*>(sample.data())[i];
        const auto& [parent, flags] = mBoneInfos[i];

        const Mat4F localMatrix = maths::transform(bone.offset, bone.rotation, bone.scale);
        const Mat4F& parentMatrix = parent == -1 ? modelMatrix : gTempMats[parent];

        Mat4F matrix = gTempMats.emplace_back(parentMatrix * localMatrix) * mInverseMats[i];

        if (flags & BoneFlag::Billboard)
        {
            // todo: make billboards properly handle non uniform scaling
            const Mat3F rotateScale = Mat3F(matrix) * maths::transpose(Mat3F(modelMatrix));
            const Vec3F translate = Vec3F((viewMatrix * matrix)[3]);

            matrix = invViewMatrix * maths::transform(translate, rotateScale, Vec3F(billboardScale, -1.f));
        }

        modelMats[i] = Mat34F(maths::transpose(matrix));
    }
}

//============================================================================//

Animation::SampleTime Animation::compute_sample_time(float time) const
{
    const float animTotalTime = float(frameCount);

    time = std::fmod(time, animTotalTime);
    if (std::signbit(time)) time += animTotalTime;

    const uint frameA = uint(time);
    const uint frameB = (frameA + 1u) == frameCount ? 0u : (frameA + 1u);

    const float factor = std::modf(time, &time);

    return { frameA, frameB, factor };
}
