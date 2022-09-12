#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/EnumHelper.hpp>
#include <sqee/core/Types.hpp>
#include <sqee/core/Utilities.hpp>

#include <sqee/vk/PassConfig.hpp>
#include <sqee/vk/Vulkan.hpp>

namespace sq {

//============================================================================//

/// A Vulkan Pipeline defined in JSON, with reflection data.
///
/// Only push constant blocks are reflected. Textures must use descriptor
/// indexing with push constants. Every pipeline uses the same 128 byte push
/// constant range for all shader stages, regardless of what the shaders
/// actually use. This is to maintain layout compatibility.
///
class SQEE_API Pipeline : MoveOnly
{
public: //====================================================//

    enum class PushConstantType : uint8_t
    {
        Int, Vec2I, Vec3I, Vec4I,
        Uint, Vec2U, Vec3U, Vec4U,
        Float, Vec2F, Vec3F, Vec4F,
        Mat23F, Mat34F, Mat4F
    };

    struct PushConstantInfo
    {
        PushConstantType type;
        uint8_t offset;
    };

    //--------------------------------------------------------//

    Pipeline() = default;

    Pipeline(Pipeline&& other);
    Pipeline& operator=(Pipeline&& other);

    ~Pipeline();

    //--------------------------------------------------------//

    /// Load the pipeline from a json object.
    void load_from_json(const JsonValue& json, const PassConfigMap& passes);

    /// Bind the pipeline for rendering.
    void bind(vk::CommandBuffer cmdbuf) const;

    //--------------------------------------------------------//

    const PassConfig* get_pass_config() const { return mPassConfig; }

    bool check_pass_enabled() const { return bool(mPassConfig->renderPass); }

    vk::Pipeline get_pipeline() const { return mPipeline; }

    PushConstantInfo get_push_constant_info(SmallString name) const;

protected: //=================================================//

    const PassConfig* mPassConfig = nullptr;

    vk::Pipeline mPipeline;

    std::map<SmallString, PushConstantInfo> mPushConstantMap;
};

//============================================================================//

} // namespace sq

SQEE_ENUM_HELPER
(
    sq::Pipeline::PushConstantType,
    Int, Vec2I, Vec3I, Vec4I,
    Uint, Vec2U, Vec3U, Vec4U,
    Float, Vec2F, Vec3F, Vec4F,
    Mat23F, Mat34F, Mat4F
)
