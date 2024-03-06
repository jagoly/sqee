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

//==============================================================================

inline vk::PipelineRasterizationStateCreateInfo impl_make_rasterization_state(JsonAny jCullFace)
{
    return vk::PipelineRasterizationStateCreateInfo {
        {}, true, false, vk::PolygonMode::eFill,
        [&]() {
            const auto cullFace = jCullFace.as<StringView>();
            if (cullFace == "Disable") return vk::CullModeFlagBits::eNone;
            if (cullFace == "Back")    return vk::CullModeFlagBits::eBack;
            if (cullFace == "Front")   return vk::CullModeFlagBits::eFront;
            jCullFace.throw_with_context("invalid cull face mode");
        }(),
        vk::FrontFace::eClockwise, false, 0.f, 0.f, 0.f, 1.f
    };
}

inline vk::PipelineMultisampleStateCreateInfo impl_make_multisample_state(vk::SampleCountFlagBits samples, JsonAny jAlphaCoverage)
{
    const auto alphaCoverage = jAlphaCoverage.as<StringView>();

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

    jAlphaCoverage.throw_with_context("invalid alpha coverage mode");
}

inline vk::PipelineDepthStencilStateCreateInfo impl_make_depth_stencil_state(vk::StencilOpState stencil, JsonAny jDepthTest)
{
    const bool stencilEnable = (stencil != vk::StencilOpState());
    const auto depthTest = jDepthTest.as<StringView>();

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

    jDepthTest.throw_with_context("invalid depth test mode");
}

inline vk::PipelineColorBlendAttachmentState impl_make_color_blend_state(JsonAny jBlendMode)
{
    const auto blendMode = jBlendMode.as<StringView>();

    if (blendMode == "Disable")
        return vk::PipelineColorBlendAttachmentState {
            false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111)
        };
    if (blendMode == "Accumulate")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOne, vk::BlendOp::eAdd,
            vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOne, vk::BlendOp::eAdd, vk::ColorComponentFlags(0b1111)
        };
    if (blendMode == "Alpha")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd,
            vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlags(0b1111)
        };
    if (blendMode == "PreAlpha")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd,
            vk::BlendFactor::eOne, vk::BlendFactor::eOneMinusSrcColor, vk::BlendOp::eAdd, vk::ColorComponentFlags(0b1111)
        };
    if (blendMode == "RevSubtract")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eReverseSubtract,
            vk::BlendFactor::eOne, vk::BlendFactor::eOne, vk::BlendOp::eReverseSubtract, vk::ColorComponentFlags(0b1111)
        };

    jBlendMode.throw_with_context("invalid colour blend mode");
}

//==============================================================================

Pipeline::Pipeline(Pipeline&& other)
{
    *this = std::move(other);
}

Pipeline& Pipeline::operator=(Pipeline&& other)
{
    std::swap(mPipeline, other.mPipeline);
    mPassConfig = other.mPassConfig;
    mPushConstants = std::move(other.mPushConstants);
    return *this;
}

Pipeline::~Pipeline()
{
    const auto& ctx = VulkanContext::get();
    if (mPipeline) ctx.device.destroy(mPipeline);
}

//==============================================================================

// todo: remove asserts relating to shaders, to support live editing

void Pipeline::load_from_minified_json(const String& minified, const PassConfigMap& passes)
{
    SQASSERT(mPassConfig == nullptr, "pipeline already loaded");

    const auto document = JsonDocument::parse_string(minified, "Pipeline");
    const auto json = document.root().as<JsonObject>();

    //----------------------------------------------------------

    // because the minified json is used as a resource key, pipeline json must be in a consistent order
    // we ensure this by iterating through the object instead of doing lookups

    auto jsonRootIter = json.begin();

    const auto next_element = [&](StringView nextKey) -> JsonAny
    {
        if (jsonRootIter == json.end()) json.throw_with_context("missing key '{}'", nextKey);
        auto [key, value] = *(jsonRootIter++);
        if (key != nextKey) value.throw_with_context("key should be '{}'", nextKey);
        return value;
    };

    //----------------------------------------------------------

    // Note that we always reflect push constants, so that we can create DrawItems for a
    // pass even if the pass is disabled. The offsets won't ever change, so DrawItems don't
    // need to be reloaded if the pass gets enabled later.

    const auto reflect_shader = [this](const std::vector<std::byte>& code, bool fragment)
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
                const auto memberName = SmallString(compiler.get_member_name(blockType.self, i));

                const auto& mt = compiler.get_type(blockType.member_types[i]);

                if (mt.array.empty() == false)
                    SQEE_THROW("push constant '{}' is an array", memberName);

                const auto memberType = [&]() -> PushConstantType {
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

                const auto memberOffset = uint8_t(compiler.type_struct_member_offset(blockType, i));

                bool foundExisting = false;
                if (fragment == true)
                {
                    for (PushConstant& other : mPushConstants)
                    {
                        if (other.name == memberName || other.offset == memberOffset)
                        {
                            if (other.name == memberName && other.offset == memberOffset && other.type == memberType)
                            {
                                foundExisting = true;
                                other.fragment = true;
                                break;
                            }
                            SQEE_THROW("push constant (name: {}, offset: {}) does not match", memberName, memberOffset);
                        }
                    }
                }

                if (foundExisting == false)
                {
                    const auto iter = ranges::find_if(mPushConstants, [&](const PushConstant& other) { return other.offset > memberOffset; });
                    mPushConstants.insert(iter, PushConstant{memberName, memberType, memberOffset, !fragment, fragment});
                }
            }
        }
    };

    const auto jVertexShader = next_element("vertexShader");
    const auto vertexShaderPath = string_concat("shaders/", jVertexShader.as<StringView>(), ".vert.spv");
    const auto vertexShaderCode = try_read_bytes_from_file(vertexShaderPath);
    if (!vertexShaderCode) jVertexShader.throw_with_context("could not read file '{}'", vertexShaderPath);

    const auto jFragmentShader = next_element("fragmentShader");
    const auto fragmentShaderPath = string_concat("shaders/", jFragmentShader.as<StringView>(), ".frag.spv");
    const auto fragmentShaderCode = try_read_bytes_from_file(fragmentShaderPath);
    if (!fragmentShaderCode) jFragmentShader.throw_with_context("could not read file '{}'", fragmentShaderPath);

    reflect_shader(*vertexShaderCode, false);
    reflect_shader(*fragmentShaderCode, true);

    //----------------------------------------------------------

    const auto jPass = next_element("pass");

    if (auto iter = passes.find(jPass.as<TinyString>()); iter != passes.end())
        mPassConfig = &iter->second;
    else
        jPass.throw_with_context("invalid pass name");

    // if the pass is disabled, then we only need to do reflection
    if (check_pass_enabled() == false) return;

    //----------------------------------------------------------

    const auto jAttributes = next_element("attributes");

    constexpr auto attributeMap = std::array {
        std::pair(StringView("TexCoords"), Mesh::Attribute::TexCoords),
        std::pair(StringView("Normals"),   Mesh::Attribute::Normals),
        std::pair(StringView("Tangents"),  Mesh::Attribute::Tangents),
        std::pair(StringView("Colours"),   Mesh::Attribute::Colours),
        std::pair(StringView("Bones"),     Mesh::Attribute::Bones),
    };

    auto attributeMapIter = attributeMap.begin();

    Mesh::Attributes attributes = {};
    Mesh::Attributes ignoredAttributes = {};

    for (const auto [_, jAttribute] : jAttributes.as<JsonArray>())
    {
        auto sv = jAttribute.as<StringView>();
        const bool ignored = sv.front() == '#';
        if (ignored) sv.remove_prefix(1u);

        attributeMapIter = std::find_if(attributeMapIter, attributeMap.end(), [&](const auto& item) { return sv == item.first; });
        if (attributeMapIter == attributeMap.end()) jAttribute.throw_with_context("attribute invalid or out of order");

        attributes |= attributeMapIter->second;
        if (ignored) ignoredAttributes |= attributeMapIter->second;
    }

    const auto vertexConfig = Mesh::VertexConfig(attributes, ignoredAttributes);

    //----------------------------------------------------------

    const auto jCullFace = next_element("cullFace");
    const auto jAlphaCoverage = next_element("alphaCoverage");
    const auto jDepthTest = next_element("depthTest");

    const auto jBlendModes = next_element("blendModes").as<JsonArray>();

    std::vector<vk::PipelineColorBlendAttachmentState> colourBlendAttachments;
    colourBlendAttachments.reserve(jBlendModes.size());

    for (const auto [_, jBlendMode] : jBlendModes)
        colourBlendAttachments.emplace_back(impl_make_color_blend_state(jBlendMode));

    //----------------------------------------------------------

    const auto& ctx = VulkanContext::get();

    // todo: spec constants from json
    const auto specialisation = SpecialisationConstants (
        0u, int(mPassConfig->viewport.x), 1u, int(mPassConfig->viewport.y), 2u, int(mPassConfig->samples), mPassConfig->constants
    );

    const auto shaderModules = ShaderModules (
        ctx, { reinterpret_cast<const uint32_t*>(vertexShaderCode->data()), vertexShaderCode->size() },
        {}, { reinterpret_cast<const uint32_t*>(fragmentShaderCode->data()), fragmentShaderCode->size() },
        &specialisation.info
    );

    //----------------------------------------------------------

    if (jsonRootIter != json.end()) jsonRootIter->second.throw_with_context("unrecognised extra key");

    mPipeline = vk_create_graphics_pipeline (
        ctx, mPassConfig->pipelineLayout, mPassConfig->renderPass, mPassConfig->subpass, shaderModules.stages, vertexConfig.state,
        vk::PipelineInputAssemblyStateCreateInfo { {}, vk::PrimitiveTopology::eTriangleList, false },
        impl_make_rasterization_state(jCullFace),
        impl_make_multisample_state(mPassConfig->samples, jAlphaCoverage),
        impl_make_depth_stencil_state(mPassConfig->stencil, jDepthTest),
        vk::Viewport { 0.f, float(mPassConfig->viewport.y), float(mPassConfig->viewport.x), -float(mPassConfig->viewport.y), 0.f, 1.f },
        vk::Rect2D { {0, 0}, {mPassConfig->viewport.x, mPassConfig->viewport.y} },
        colourBlendAttachments, {}
    );
}

//==============================================================================

void Pipeline::bind(vk::CommandBuffer cmdbuf) const
{
    cmdbuf.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
}
