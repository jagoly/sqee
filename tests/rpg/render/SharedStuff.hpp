#pragma once

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/PreProcessor.hpp>

#include <sqee/render/Volume.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>

namespace sqt {

//============================================================================//

class Options; // Forward Declaration

extern "C" const float data_CubeVertices [8*3];
extern "C" const uchar data_CubeIndices  [12*3];
extern "C" const float data_SphereVertices [24*3];
extern "C" const uchar data_SphereIndices  [44*3];
extern "C" const float data_ConeVertices [18*3];
extern "C" const uchar data_ConeIndices  [32*3];

//============================================================================//

namespace render {

//============================================================================//

struct StencilVolumes : sq::NonCopyable
{
    sq::Volume Unit_Cube { data_CubeVertices, data_CubeIndices, 8u, 36u };

    sq::Volume Light_Ortho { data_CubeVertices, data_CubeIndices, 8u, 36u };
    sq::Volume Light_Point { data_SphereVertices, data_SphereIndices, 24u, 132u };
    sq::Volume Light_Spot { data_ConeVertices, data_ConeIndices, 18u, 96u };
};

//============================================================================//

struct TargetTextures : sq::NonCopyable
{
    TargetTextures();

    sq::Texture2D Depth_FullSize { sq::Texture::Format::DEP24S8 };
    sq::Texture2D Depth_HalfSize { sq::Texture::Format::DEP24S8 };
    sq::Texture2D Depth_QterSize { sq::Texture::Format::DEP24S8 };

    sq::Texture2D Gbuffer_MainDiff { sq::Texture::Format::RGB8_UN };
    sq::Texture2D Gbuffer_MainSurf { sq::Texture::Format::RGB8_SN };
    sq::Texture2D Gbuffer_MainNorm { sq::Texture::Format::RGB8_SN };
    sq::Texture2D Gbuffer_MainSpec { sq::Texture::Format::RGB8_UN };

    sq::Texture2D Lighting_Main { sq::Texture::Format::RGB16_FP };

    sq::Texture2D Volumetric_Shafts { sq::Texture::Format::RGB8_UN };

    sq::Texture2D Effects_SSAO { sq::Texture::Format::R8_UN };
    sq::Texture2D Effects_Bloom { sq::Texture::Format::RGB8_UN };

    sq::Texture2D Composite_Main { sq::Texture::Format::RGBA8_UN };

    void update_options(const Options& options);
};

//============================================================================//

struct GenericShaders : sq::NonCopyable
{
    GenericShaders();

    sq::Shader VS_FullScreen { sq::Shader::Stage::Vertex };
    sq::Shader VS_MainStencil { sq::Shader::Stage::Vertex };
    sq::Shader VS_ReflStencil { sq::Shader::Stage::Vertex };

    sq::Shader FS_PassThrough { sq::Shader::Stage::Fragment };
    sq::Shader FS_FillSpace { sq::Shader::Stage::Fragment };

    sq::PreProcessor preprocs;

    void update_options(const Options& options);
};

//============================================================================//

struct SharedStuff
{
    const StencilVolumes& volumes;
    const TargetTextures& textures;
    const GenericShaders& shaders;

    const Options& options;
    sq::Context& context;
};

//============================================================================//

}} // namespace sqt::render
