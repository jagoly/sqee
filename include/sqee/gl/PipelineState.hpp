#pragma once

#include <sqee/gl/Context.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Program.hpp>

#include <sqee/misc/ResourceHandle.hpp>

namespace sq {

//============================================================================//

/// OpenGL Pipeline State Object.
struct PipelineState final
{
    using TextureHandle = Handle<String, Texture2D>;
    using ProgramHandle = Handle<ProgramKey, Program>;

    //--------------------------------------------------------//

    Optional<Context::Clip_Distance>  clip_distance;
    Optional<Context::Depth_Clamp>    depth_clamp;
    Optional<Context::Scissor_Test>   scissor_test;
    //Optional<Context::Alpha_Coverage> alpha_coverage;
    Optional<Context::Cull_Face>      cull_face;
    Optional<Context::Depth_Test>     depth_test;
    Optional<Context::Stencil_Test>   stencil_test;
    Optional<Context::Blend_Mode>     blend_mode;
    Optional<Context::Depth_Compare>  depth_compare;

    //--------------------------------------------------------//

    Optional<Context::Scissor_Params> scissor_params;
    Optional<Context::Stencil_Params> stencil_params;
    Optional<Context::Stencil_Custom> stencil_custom;
    Optional<Context::Blend_Custom> blend_custom;
    Optional<Vec2U> viewport;

    //--------------------------------------------------------//

    ProgramHandle shader_program;

    Array<TextureHandle, 4u> texture_handles;
    Array<uint8_t, 4u> texture_bindings;

    Array<UniformBuffer*, 4> ubo_handles;
    Array<uint8_t, 4> ubo_bindings;
};

//============================================================================//

} // namespace sq
