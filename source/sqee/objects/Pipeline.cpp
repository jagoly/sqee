#include <sqee/objects/Pipeline.hpp>

#include <sqee/core/Algorithms.hpp>
#include <sqee/core/Utilities.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/objects/Mesh.hpp>
#include <sqee/vk/Helpers.hpp>

#include <spirv_cross.hpp>

using namespace sq;
namespace cross = spirv_cross;

//============================================================================//

static vk::PipelineRasterizationStateCreateInfo impl_make_rasterization_state(const String& cullFace)
{
    return vk::PipelineRasterizationStateCreateInfo {
        {}, false, false, vk::PolygonMode::eFill,
        [&]() {
            if (cullFace == "Disable") return vk::CullModeFlagBits::eNone;
            if (cullFace == "Back")    return vk::CullModeFlagBits::eBack;
            if (cullFace == "Front")   return vk::CullModeFlagBits::eFront;
            SQEE_THROW("invalid pipeline cullFace string '{}'", cullFace);
        }(),
        vk::FrontFace::eClockwise, false, 0.f, false, 0.f, 1.f
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
    if (colourBlend == "Accumulate") // TODO
        return vk::PipelineColorBlendAttachmentState {
            false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111)
        };
    if (colourBlend == "Alpha")
        return vk::PipelineColorBlendAttachmentState {
            true, vk::BlendFactor::eSrcAlpha, vk::BlendFactor::eOneMinusSrcAlpha, vk::BlendOp::eAdd,
            vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::ColorComponentFlags(0b1111)
        };
    if (colourBlend == "PremAlpha") // TODO
        return vk::PipelineColorBlendAttachmentState {
            false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111)
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
    std::swap(mMaterialSetLayout, other.mMaterialSetLayout);
    std::swap(mPipelineLayout, other.mPipelineLayout);
    std::swap(mPipeline, other.mPipeline);
    return *this;
}

Pipeline::~Pipeline()
{
    const auto& ctx = VulkanContext::get();
    if (mMaterialSetLayout) ctx.device.destroy(mMaterialSetLayout);
    if (mPipelineLayout) ctx.device.destroy(mPipelineLayout);
    if (mPipeline) ctx.device.destroy(mPipeline);
}

//============================================================================//

// todo: remove asserts relating to shaders, to support live editing

void Pipeline::load_from_json(const JsonValue& json, const PassConfigMap& passes)
{
    std::array<std::vector<vk::DescriptorSetLayoutBinding>, DESCRIPTOR_SET_COUNT> setBindings;

    const auto add_descriptor = [&setBindings](uint set, uint binding, vk::DescriptorType type, vk::ShaderStageFlagBits stage)
    {
        SQASSERT(set < DESCRIPTOR_SET_COUNT, "descriptor set index out of range");

        if (setBindings[set].size() <= binding)
            setBindings[set].resize(binding + 1u);

        vk::DescriptorSetLayoutBinding& dslb = setBindings[set][binding];

        if (dslb.descriptorCount != 0u)
        {
            SQASSERT(dslb.descriptorType == type, "descriptor mismatch between stages");
            dslb.stageFlags |= stage;
        }
        else dslb = { binding, type, 1u, stage, nullptr };
    };

    //std::vector<vk::PushConstantRange> pushConstantRanges;

    //--------------------------------------------------------//

    const auto reflect_shader = [&](const std::vector<std::byte>& code, vk::ShaderStageFlagBits stage)
    {
        const auto compiler = cross::Compiler(reinterpret_cast<const uint32_t*>(code.data()), code.size() / 4u);
        const auto resources = compiler.get_shader_resources();

        SQASSERT(resources.storage_buffers.empty(), "todo");
        SQASSERT(resources.subpass_inputs.empty(), "todo");
        SQASSERT(resources.storage_images.empty(), "todo");
        SQASSERT(resources.storage_buffers.empty(), "todo");
        SQASSERT(resources.atomic_counters.empty(), "todo");
        SQASSERT(resources.acceleration_structures.empty(), "todo");
        SQASSERT(resources.separate_images.empty(), "todo");
        SQASSERT(resources.separate_samplers.empty(), "todo");

        SQASSERT(resources.push_constant_buffers.empty(), "todo");
        //for (const auto& pcb : resources.push_constant_buffers)
        //{
        //    const auto& blockType = compiler.get_type(pcb.type_id);
        //
        //    // for ease of use, this range includes members that aren't used
        //    vk::PushConstantRange& range = pushConstantRanges.emplace_back(stage, 256u, 0u);
        //
        //    for (uint i = 0u; i < blockType.member_types.size(); ++i)
        //    {
        //        const auto& mt = compiler.get_type(blockType.member_types[i]);
        //        SQASSERT(mt.array.empty(), "push constant block must not contain arrays");
        //
        //        PushConstantInfo& entry = mPushConstantMap[compiler.get_member_name(blockType.self, i)];
        //        entry.type = [&mt]() -> TinyString {
        //            if (mt.columns == 1u && mt.basetype == cross::SPIRType::Int && mt.vecsize == 1u) return "int";
        //            if (mt.columns == 1u && mt.basetype == cross::SPIRType::Int && mt.vecsize == 2u) return "Vec2I";
        //            if (mt.columns == 1u && mt.basetype == cross::SPIRType::Int && mt.vecsize == 3u) return "Vec3I";
        //            if (mt.columns == 1u && mt.basetype == cross::SPIRType::Int && mt.vecsize == 4u) return "Vec4I";
        //            if (mt.columns == 1u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 1u) return "float";
        //            if (mt.columns == 1u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 2u) return "Vec2F";
        //            if (mt.columns == 1u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 3u) return "Vec3F";
        //            if (mt.columns == 1u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 4u) return "Vec4F";
        //            if (mt.columns == 3u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 4u) return "Mat34F";
        //            if (mt.columns == 4u && mt.basetype == cross::SPIRType::Float && mt.vecsize == 4u) return "Mat4F";
        //            SQASSERT(false, "push constant block member has invalid type"); return "";
        //        }();
        //        entry.stages = entry.stages | stage;
        //        entry.offset = compiler.type_struct_member_offset(blockType, i);
        //
        //        range.offset = std::min(range.offset, entry.offset);
        //        range.size = std::max(range.size, entry.offset + mt.columns * mt.vecsize * 4u);
        //    }
        //}

        for (const auto& ubo : resources.uniform_buffers)
        {
            const uint set = compiler.get_decoration(ubo.id, spv::DecorationDescriptorSet);
            const uint binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);

            add_descriptor(set, binding, vk::DescriptorType::eUniformBuffer, stage);

            // only need to reflect the material param block
            if (set != MATERIAL_SET_INDEX) continue;

            // already found material block in another shader stage
            if (mMaterialParamMap.empty() == false) continue;

            SQASSERT(binding == 0u, "material block binding must be zero");

            const auto& blockType = compiler.get_type(ubo.type_id);
            mMaterialParamBlockSize = compiler.get_declared_struct_size(blockType);

            for (uint i = 0u; i < blockType.member_types.size(); ++i)
            {
                const auto& mt = compiler.get_type(blockType.member_types[i]);
                SQASSERT(mt.array.empty(), "material block must not contain arrays");
                SQASSERT(mt.columns == 1u, "material block must not contain matrices");

                MaterialParamInfo& entry = mMaterialParamMap[compiler.get_member_name(blockType.self, i)];
                entry.type = [&mt]() -> TinyString {
                    if (mt.basetype == cross::SPIRType::Int && mt.vecsize == 1u) return "int";
                    if (mt.basetype == cross::SPIRType::Int && mt.vecsize == 2u) return "Vec2I";
                    if (mt.basetype == cross::SPIRType::Int && mt.vecsize == 3u) return "Vec3I";
                    if (mt.basetype == cross::SPIRType::Int && mt.vecsize == 4u) return "Vec4I";
                    if (mt.basetype == cross::SPIRType::Float && mt.vecsize == 1u) return "float";
                    if (mt.basetype == cross::SPIRType::Float && mt.vecsize == 2u) return "Vec2F";
                    if (mt.basetype == cross::SPIRType::Float && mt.vecsize == 3u) return "Vec3F";
                    if (mt.basetype == cross::SPIRType::Float && mt.vecsize == 4u) return "Vec4F";
                    SQASSERT(false, "material block member has invalid type"); return "";
                }();
                entry.offset = compiler.type_struct_member_offset(blockType, i);
            }
        }

        for (const auto& tex : resources.sampled_images)
        {
            const uint set = compiler.get_decoration(tex.id, spv::DecorationDescriptorSet);
            const uint binding = compiler.get_decoration(tex.id, spv::DecorationBinding);

            add_descriptor(set, binding, vk::DescriptorType::eCombinedImageSampler, stage);

            // only need to reflect material set textures
            if (set != MATERIAL_SET_INDEX) continue;

            const auto& texType = compiler.get_type(tex.type_id);

            // todo: array/cube textures
            SQASSERT(texType.image.dim == spv::Dim::Dim2D, "unsupported texture in material set");
            SQASSERT(texType.image.depth == false, "unsupported texture in material set");
            SQASSERT(texType.image.arrayed == false, "unsupported texture in material set");
            SQASSERT(texType.image.ms == false, "unsupported texture in material set");

            const TinyString name = compiler.get_name(tex.id);
            const TinyString type = "Texture2D";

            mTextureMap[name] = TextureInfo { type, binding };
        }
    };

    //--------------------------------------------------------//

    // setup pass config, with a useful error message
    {
        const String& key = json.at("pass");
        const auto iter = passes.find(key);

        if (iter != passes.end()) mPassConfig = &iter->second;
        else SQEE_THROW("unknown pass config '{}'", key);
    }

    const auto& ctx = VulkanContext::get();

    const auto vertexShaderCode = read_bytes_from_file(build_string("shaders/", json.at("vertexShader"), ".vert.spv"));
    reflect_shader(vertexShaderCode, vk::ShaderStageFlagBits::eVertex);

    const auto fragmentShaderCode = read_bytes_from_file(build_string("shaders/", json.at("fragmentShader"), ".frag.spv"));
    reflect_shader(fragmentShaderCode, vk::ShaderStageFlagBits::eFragment);

    // make sure unused bindings have correct binding index
    for (uint i = 0u; i < DESCRIPTOR_SET_COUNT; ++i)
        for (uint j = 0u; j < setBindings[i].size(); ++j)
            setBindings[i][j].binding = j;

    // create descriptor set and pipeline layouts
    {
        std::array<vk::DescriptorSetLayout, DESCRIPTOR_SET_COUNT> setLayouts;

        setLayouts[0] = mPassConfig->setLayout0;
        setLayouts[1] = mPassConfig->setLayout1;

        for (uint i = 2u; i < DESCRIPTOR_SET_COUNT; ++i)
            setLayouts[i] = vk_create_descriptor_set_layout(ctx, {}, setBindings[i]);

        mMaterialSetLayout = setLayouts[MATERIAL_SET_INDEX];
        //mPipelineLayout = vk_create_pipeline_layout(ctx, {}, setLayouts, pushConstantRanges);
        mPipelineLayout = vk_create_pipeline_layout(ctx, {}, setLayouts, nullptr);

        for (uint i = 2u; i < DESCRIPTOR_SET_COUNT; ++i)
            if (i != MATERIAL_SET_INDEX)
                ctx.device.destroy(setLayouts[i]);
    }

    //--------------------------------------------------------//

    const std::vector<JsonValue>& jsonAttributes = json.at("attributes");
    Mesh::Attributes attributes = {};

    if (algo::find(jsonAttributes, "TexCoords") != jsonAttributes.end()) attributes |= Mesh::Attribute::TexCoords;
    if (algo::find(jsonAttributes, "Normals") != jsonAttributes.end()) attributes |= Mesh::Attribute::Normals;
    if (algo::find(jsonAttributes, "Tangents") != jsonAttributes.end()) attributes |= Mesh::Attribute::Tangents;
    if (algo::find(jsonAttributes, "Colours") != jsonAttributes.end()) attributes |= Mesh::Attribute::Colours;
    if (algo::find(jsonAttributes, "Bones") != jsonAttributes.end()) attributes |= Mesh::Attribute::Bones;

    const auto vertexConfig = Mesh::VertexConfig ( attributes );

    // todo: constants from json
    const auto specialisation = SpecialisationConstants (
        0u, int(mPassConfig->viewport.x), 1u, int(mPassConfig->viewport.y), 2u, int(mPassConfig->samples), mPassConfig->constants
    );

    const auto shaderModules = ShaderModules (
        ctx, { reinterpret_cast<const uint32_t*>(vertexShaderCode.data()), vertexShaderCode.size() },
        {}, { reinterpret_cast<const uint32_t*>(fragmentShaderCode.data()), fragmentShaderCode.size() },
        &specialisation.info
    );

    const std::vector<JsonValue>& jsonColourBlend = json.at("colourBlend");
    std::vector<vk::PipelineColorBlendAttachmentState> colourBlendAttachments;
    colourBlendAttachments.reserve(jsonColourBlend.size());

    for (const JsonValue& colourBlend : jsonColourBlend)
        colourBlendAttachments.emplace_back(impl_make_color_blend_state(colourBlend));

    //--------------------------------------------------------//

    mPipeline = vk_create_graphics_pipeline (
        ctx, mPipelineLayout, mPassConfig->renderPass, mPassConfig->subpass, shaderModules.stages, vertexConfig.state,
        vk::PipelineInputAssemblyStateCreateInfo { {}, vk::PrimitiveTopology::eTriangleList, false },
        impl_make_rasterization_state(json.at("cullFace")),
        impl_make_multisample_state(mPassConfig->samples, json.at("alphaCoverage")),
        impl_make_depth_stencil_state(mPassConfig->stencil, json.at("depthTest")),
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
