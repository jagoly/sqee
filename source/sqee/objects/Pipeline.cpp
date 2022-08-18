#include <sqee/objects/Pipeline.hpp>

#include <sqee/core/Strings.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/objects/Mesh.hpp>
#include <sqee/vk/Helpers.hpp>
#include <sqee/vk/VulkanContext.hpp>

#include <spirv_cross.hpp>

using namespace sq;
namespace cross = spirv_cross;

//============================================================================//

static vk::PipelineRasterizationStateCreateInfo impl_make_rasterization_state(const String& cullFace)
{
    return vk::PipelineRasterizationStateCreateInfo {
        {}, true, false, vk::PolygonMode::eFill,
        [&]() {
            if (cullFace == "Disable") return vk::CullModeFlagBits::eNone;
            if (cullFace == "Back")    return vk::CullModeFlagBits::eBack;
            if (cullFace == "Front")   return vk::CullModeFlagBits::eFront;
            SQEE_THROW("invalid pipeline cullFace string '{}'", cullFace);
        }(),
        vk::FrontFace::eClockwise, false, 0.f, 0.f, 0.f, 1.f
    };
}

static vk::PipelineMultisampleStateCreateInfo impl_make_multisample_state(vk::SampleCountFlagBits samples, const String& alphaCoverage)
{
    if (alphaCoverage == "Disable")
        return vk::PipelineMultisampleStateCreateInfo {
            {}, samples, false, 0.f, nullptr, false, false
        };
    if (alphaCoverage == "Enable")
        return vk::PipelineMultisampleStateCreateInfo {
            {}, samples, false, 0.f, nullptr, true, false
        };
    if (alphaCoverage == "Conditional")
        return vk::PipelineMultisampleStateCreateInfo {
            {}, samples, false, 0.f, nullptr, samples > vk::SampleCountFlagBits::e1, false
        };

    SQEE_THROW("invalid pipeline alphaCoverage string '{}'", alphaCoverage);
}

static vk::PipelineDepthStencilStateCreateInfo impl_make_depth_stencil_state(vk::StencilOpState stencil, const String& depthTest)
{
    const bool stencilEnable = (stencil != vk::StencilOpState());

    if (depthTest == "Disable")
        return vk::PipelineDepthStencilStateCreateInfo {
            {}, false, false, {}, false, stencilEnable, stencil, stencil, 0.f, 0.f
        };
    if (depthTest == "Keep")
        return vk::PipelineDepthStencilStateCreateInfo {
            {}, true, false, vk::CompareOp::eLessOrEqual, false, stencilEnable, stencil, stencil, 0.f, 0.f
        };
    if (depthTest == "Replace")
        return vk::PipelineDepthStencilStateCreateInfo {
            {}, true, true, vk::CompareOp::eLess, false, stencilEnable, stencil, stencil, 0.f, 0.f
        };

    SQEE_THROW("invalid pipeline depthTest string '{}'", depthTest);
}

static vk::PipelineColorBlendAttachmentState impl_make_color_blend_state(const String& colourBlend)
{
    if (colourBlend == "Disable")
        return vk::PipelineColorBlendAttachmentState {
            false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111)
        };
    if (colourBlend == "Accumulate")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOne, vk::BlendOp::eAdd,
            vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOne, vk::BlendOp::eAdd, vk::ColorComponentFlags(0b1111)
        };
    if (colourBlend == "Alpha")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd,
            vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlags(0b1111)
        };
    if (colourBlend == "PreAlpha")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd,
            vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcColor, vk::BlendOp::eAdd, vk::ColorComponentFlags(0b1111)
        };
    if (colourBlend == "RevSubtract")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eReverseSubtract,
            vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eReverseSubtract, vk::ColorComponentFlags(0b1111)
        };

    SQEE_THROW("invalid pipeline colourBlend string '{}'", colourBlend);
}

//============================================================================//

Pipeline::Pipeline(Pipeline&& other)
{
    *this = std::move(other);
}

Pipeline& Pipeline::operator=(Pipeline&& other)
{
    std::swap(mPipeline, other.mPipeline);
    mPassConfig = other.mPassConfig;
    mPushConstantMap = std::move(other.mPushConstantMap);
    return *this;
}

Pipeline::~Pipeline()
{
    const auto& ctx = VulkanContext::get();
    if (mPipeline) ctx.device.destroy(mPipeline);
}

//============================================================================//

// todo: remove asserts relating to shaders, to support live editing

void Pipeline::load_from_json(const JsonValue& json, const PassConfigMap& passes)
{
    // setup pass config, with a useful error message
    {
        const auto& key = json.at("pass").get_ref<const String&>();
        const auto iter = passes.find(key);

        if (iter != passes.end()) mPassConfig = &iter->second;
        else SQEE_THROW("unknown pass config '{}'", key);

        if (bool(mPassConfig->renderPass) == false)
            return; // disabled render pass
    }

    //--------------------------------------------------------//

    const auto reflect_shader = [&](const std::vector<std::byte>& code)
    {
        const auto compiler = cross::Compiler(reinterpret_cast<const uint32_t*>(code.data()), code.size() / 4u);
        const auto resources = compiler.get_shader_resources();

        SQASSERT(resources.subpass_inputs.empty(), "todo");
        SQASSERT(resources.storage_images.empty(), "todo");
        SQASSERT(resources.atomic_counters.empty(), "todo");
        SQASSERT(resources.acceleration_structures.empty(), "todo");
        SQASSERT(resources.separate_images.empty(), "todo");
        SQASSERT(resources.separate_samplers.empty(), "todo");

        for (const auto& pcb : resources.push_constant_buffers)
        {
            const auto& blockType = compiler.get_type(pcb.type_id);

            for (uint i = 0u; i < blockType.member_types.size(); ++i)
            {
                const auto& mt = compiler.get_type(blockType.member_types[i]);
                const auto& memberName = compiler.get_member_name(blockType.self, i);

                if (mt.array.empty() == false)
                    SQEE_THROW("push constant '{}' is an array", memberName);

                PushConstantInfo& entry = mPushConstantMap[memberName];

                entry.type = [&]() -> PushConstantType {
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::Int && mt.vecsize == 1u) return PushConstantType::Int;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::Int && mt.vecsize == 2u) return PushConstantType::Vec2I;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::Int && mt.vecsize == 3u) return PushConstantType::Vec3I;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::Int && mt.vecsize == 4u) return PushConstantType::Vec4I;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::UInt && mt.vecsize == 1u) return PushConstantType::Uint;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::UInt && mt.vecsize == 2u) return PushConstantType::Vec2U;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::UInt && mt.vecsize == 3u) return PushConstantType::Vec3U;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::UInt && mt.vecsize == 4u) return PushConstantType::Vec4U;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 1u) return PushConstantType::Float;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 2u) return PushConstantType::Vec2F;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 3u) return PushConstantType::Vec3F;
                    if (mt.columns == 1u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 4u) return PushConstantType::Vec4F;
                    if (mt.columns == 2u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 3u) return PushConstantType::Mat23F;
                    if (mt.columns == 3u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 4u) return PushConstantType::Mat34F;
                    if (mt.columns == 4u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 4u) return PushConstantType::Mat4F;
                    SQEE_THROW("push constant '{}' has invalid type", memberName);
                }();

                entry.offset = uint8_t(compiler.type_struct_member_offset(blockType, i));
            }
        }
    };

    //--------------------------------------------------------//

    const auto& ctx = VulkanContext::get();

    const auto vertexShaderCode = read_bytes_from_file (
        build_string("shaders/", json.at("vertexShader").get_ref<const String&>(), ".vert.spv")
    );
    reflect_shader(vertexShaderCode);

    const auto fragmentShaderCode = read_bytes_from_file (
        build_string("shaders/", json.at("fragmentShader").get_ref<const String&>(), ".frag.spv")
    );
    reflect_shader(fragmentShaderCode);

    //--------------------------------------------------------//

    const auto& jAttributes = json.at("attributes").get_ref<const JsonValue::array_t&>();

    Mesh::Attributes attributes = {};
    Mesh::Attributes ignoredAttributes = {};

    for (const auto& element : jAttributes)
    {
        StringView sv = element.get_ref<const String&>();

        const bool ignored = sv.front() == '#';
        if (ignored) sv.remove_prefix(1u);

        const Mesh::Attribute attribute = [&]() {
            if (sv == "TexCoords") return Mesh::Attribute::TexCoords;
            if (sv == "Normals")   return Mesh::Attribute::Normals;
            if (sv == "Tangents")  return Mesh::Attribute::Tangents;
            if (sv == "Colours")   return Mesh::Attribute::Colours;
            if (sv == "Bones")     return Mesh::Attribute::Bones;
            SQEE_THROW("invalid mesh attribute '{}'", sv);
        }();

        attributes |= attribute;
        if (ignored) ignoredAttributes |= attribute;
    }

    const auto vertexConfig = Mesh::VertexConfig(attributes, ignoredAttributes);

    //--------------------------------------------------------//

    const auto& jColourBlend = json.at("colourBlend").get_ref<const JsonValue::array_t&>();

    std::vector<vk::PipelineColorBlendAttachmentState> colourBlendAttachments;
    colourBlendAttachments.reserve(jColourBlend.size());

    for (const auto& element : jColourBlend)
        colourBlendAttachments.emplace_back(impl_make_color_blend_state(element.get_ref<const String&>()));

    //--------------------------------------------------------//

    // todo: spec constants from json
    const auto specialisation = SpecialisationConstants (
        0u, int(mPassConfig->viewport.x), 1u, int(mPassConfig->viewport.y), 2u, int(mPassConfig->samples), mPassConfig->constants
    );

    const auto shaderModules = ShaderModules (
        ctx, { reinterpret_cast<const uint32_t*>(vertexShaderCode.data()), vertexShaderCode.size() },
        {}, { reinterpret_cast<const uint32_t*>(fragmentShaderCode.data()), fragmentShaderCode.size() },
        &specialisation.info
    );

    //--------------------------------------------------------//

    mPipeline = vk_create_graphics_pipeline (
        ctx, mPassConfig->pipelineLayout, mPassConfig->renderPass, mPassConfig->subpass, shaderModules.stages, vertexConfig.state,
        vk::PipelineInputAssemblyStateCreateInfo { {}, vk::PrimitiveTopology::eTriangleList, false },
        impl_make_rasterization_state(json.at("cullFace").get_ref<const String&>()),
        impl_make_multisample_state(mPassConfig->samples, json.at("alphaCoverage").get_ref<const String&>()),
        impl_make_depth_stencil_state(mPassConfig->stencil, json.at("depthTest").get_ref<const String&>()),
        vk::Viewport { 0.f, float(mPassConfig->viewport.y), float(mPassConfig->viewport.x), -float(mPassConfig->viewport.y), 0.f, 1.f },
        vk::Rect2D { {0, 0}, {mPassConfig->viewport.x, mPassConfig->viewport.y} },
        colourBlendAttachments, {}
    );
}

//============================================================================//

void Pipeline::bind(vk::CommandBuffer cmdbuf) const
{
    cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
}

//============================================================================//

Pipeline::PushConstantInfo Pipeline::get_push_constant_info(SmallString name) const
{
    const auto iter = mPushConstantMap.find(name);
    if (iter != mPushConstantMap.end()) return iter->second;

    SQEE_THROW("pipeline does not have push constant '{}'", name);
}
