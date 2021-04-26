#include <sqee/vk/Pipeline.hpp>

#include <sqee/core/Algorithms.hpp>
#include <sqee/core/Utilities.hpp>
#include <sqee/debug/Assert.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/misc/Json.hpp>
#include <sqee/vk/Helpers.hpp>
#include <sqee/vk/VulkMesh.hpp>

#include <spirv_cross/spirv_cross.hpp>

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
            log_error("invalid pipeline cullFace string '{}'", cullFace);
        }(),
        vk::FrontFace::eClockwise, false, 0.f, false, 0.f, 1.f
    };
}

static vk::PipelineDepthStencilStateCreateInfo impl_make_depth_stencil_state(const String& depthTest)
{
    if (depthTest == "Disable")
        return vk::PipelineDepthStencilStateCreateInfo {
            {}, false, false, {}, false, false, {}, {}, 0.f, 0.f
        };
    if (depthTest == "Keep")
        return vk::PipelineDepthStencilStateCreateInfo {
            {}, true, false, vk::CompareOp::eLessOrEqual, false, false, {}, {}, 0.f, 0.f
        };
    if (depthTest == "Replace")
        return vk::PipelineDepthStencilStateCreateInfo {
            {}, true, true, vk::CompareOp::eLess, false, false, {}, {}, 0.f, 0.f
        };

    log_error("invalid pipeline depthTest string '{}'", depthTest);
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
        SQASSERT(resources.push_constant_buffers.empty(), "todo");
        SQASSERT(resources.separate_images.empty(), "todo");
        SQASSERT(resources.separate_samplers.empty(), "todo");

        for (const auto& ubo : resources.uniform_buffers)
        {
            const uint set = compiler.get_decoration(ubo.id, spv::DecorationDescriptorSet);
            const uint binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);

            add_descriptor(set, binding, vk::DescriptorType::eUniformBuffer, stage);
            if (set != MATERIAL_SET_INDEX) continue;

            SQASSERT(binding == 0u, "material block binding must be zero");

            const auto& uboType = compiler.get_type(ubo.type_id);
            mParamBlockSize = compiler.get_declared_struct_size(uboType);

            for (uint i = 0u; i < uboType.member_types.size(); ++i)
            {
                const auto& memberType = compiler.get_type(uboType.member_types[i]);

                SQASSERT(memberType.array.empty(), "material block must not contain arrays");
                SQASSERT(memberType.columns == 1u, "material block must not contain matrices");

                const TinyString name = compiler.get_member_name(uboType.self, i);
                const TinyString type = [&memberType]() {
                    if (memberType.basetype == cross::SPIRType::Int && memberType.vecsize == 1u) return "int";
                    if (memberType.basetype == cross::SPIRType::Int && memberType.vecsize == 2u) return "Vec2I";
                    if (memberType.basetype == cross::SPIRType::Int && memberType.vecsize == 3u) return "Vec3I";
                    if (memberType.basetype == cross::SPIRType::Int && memberType.vecsize == 4u) return "Vec4I";
                    if (memberType.basetype == cross::SPIRType::Float && memberType.vecsize == 1u) return "float";
                    if (memberType.basetype == cross::SPIRType::Float && memberType.vecsize == 2u) return "Vec2F";
                    if (memberType.basetype == cross::SPIRType::Float && memberType.vecsize == 3u) return "Vec3F";
                    if (memberType.basetype == cross::SPIRType::Float && memberType.vecsize == 4u) return "Vec4F";
                    SQASSERT(false, "material block member has invalid type"); return "";
                }();
                const uint offset = compiler.type_struct_member_offset(uboType, i);

                mParamMap[name] = ParamInfo { type, offset };
            }
        }

        for (const auto& tex : resources.sampled_images)
        {
            const uint set = compiler.get_decoration(tex.id, spv::DecorationDescriptorSet);
            const uint binding = compiler.get_decoration(tex.id, spv::DecorationBinding);

            add_descriptor(set, binding, vk::DescriptorType::eCombinedImageSampler, stage);
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

    const auto& ctx = VulkanContext::get();

    const auto vertexShaderCode = get_bytes_from_file(build_string("shaders/", json.at("vertexShader"), ".vert.spv"));
    reflect_shader(vertexShaderCode, vk::ShaderStageFlagBits::eVertex);

    const auto fragmentShaderCode = get_bytes_from_file(build_string("shaders/", json.at("fragmentShader"), ".frag.spv"));
    reflect_shader(fragmentShaderCode, vk::ShaderStageFlagBits::eFragment);

    // make sure unused bindings have correct binding index
    for (uint i = 0u; i < DESCRIPTOR_SET_COUNT; ++i)
        for (uint j = 0u; j < setBindings[i].size(); ++j)
            setBindings[i][j].binding = j;

    // create descriptor set and pipeline layouts
    {
        std::array<vk::DescriptorSetLayout, DESCRIPTOR_SET_COUNT> setLayouts;

        for (uint i = 0u; i < DESCRIPTOR_SET_COUNT; ++i)
            setLayouts[i] = vk_create_descriptor_set_layout(ctx, {}, setBindings[i]);

        mMaterialSetLayout = setLayouts[MATERIAL_SET_INDEX];
        mPipelineLayout = vk_create_pipeline_layout(ctx, {}, setLayouts, nullptr);

        for (uint i = 0u; i < DESCRIPTOR_SET_COUNT; ++i)
            if (i != MATERIAL_SET_INDEX)
                ctx.device.destroy(setLayouts[i]);
    }

    //--------------------------------------------------------//

    const std::vector<JsonValue>& jsonAttributes = json.at("attributes");

    vk::Flags<VulkMesh::Attribute> attributes {};
    if (algo::find(jsonAttributes, "TexCoords") != jsonAttributes.end()) attributes |= VulkMesh::Attribute::TexCoords;
    if (algo::find(jsonAttributes, "Normals") != jsonAttributes.end()) attributes |= VulkMesh::Attribute::Normals;
    if (algo::find(jsonAttributes, "Tangents") != jsonAttributes.end()) attributes |= VulkMesh::Attribute::Tangents;
    if (algo::find(jsonAttributes, "Colours") != jsonAttributes.end()) attributes |= VulkMesh::Attribute::Colours;
    if (algo::find(jsonAttributes, "Bones") != jsonAttributes.end()) attributes |= VulkMesh::Attribute::Bones;

    // todo: cullFace | polygonMode | depthTest | colourBlend

    //--------------------------------------------------------//

    const auto& pass = passes.at(json.at("pass"));

    const auto shaderModules = ShaderModules (
        ctx, { reinterpret_cast<const uint32_t*>(vertexShaderCode.data()), vertexShaderCode.size() },
        {}, { reinterpret_cast<const uint32_t*>(fragmentShaderCode.data()), fragmentShaderCode.size() }
    );

    const auto vertexConfig = VulkMesh::VertexConfig ( attributes );

    mPipeline = vk_create_graphics_pipeline (
        ctx, mPipelineLayout, pass.renderPass, 0u, shaderModules.stages, vertexConfig.state,
        vk::PipelineInputAssemblyStateCreateInfo { {}, vk::PrimitiveTopology::eTriangleList, false },
        impl_make_rasterization_state(json.at("cullFace")),
        vk::PipelineMultisampleStateCreateInfo { {}, pass.samples, false, 0.f, nullptr, false, false },
        impl_make_depth_stencil_state(json.at("depthTest")),
        vk::Viewport { 0.f, float(pass.viewport.y), float(pass.viewport.x), -float(pass.viewport.y), 0.f, 1.f },
        vk::Rect2D { {0, 0}, {pass.viewport.x, pass.viewport.y} },
        vk::PipelineColorBlendAttachmentState { false, {}, {}, {}, {}, {}, {}, vk::ColorComponentFlags(0b1111) },
        nullptr
    );
}
