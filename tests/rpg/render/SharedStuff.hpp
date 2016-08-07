#pragma once

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/PreProcessor.hpp>

#include <sqee/render/Volume.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Shaders.hpp>

namespace sqt { namespace render {

extern "C" const float data_CubeVertices [8*3];
extern "C" const uchar data_CubeIndices  [12*3];
extern "C" const float data_SphereVertices [24*3];
extern "C" const uchar data_SphereIndices  [44*3];
extern "C" const float data_ConeVertices [18*3];
extern "C" const uchar data_ConeIndices  [32*3];


struct StencilVolumes : sq::NonCopyable {

    sq::Volume Unit_Cube {data_CubeVertices, data_CubeIndices, 8u, 36u};

    sq::Volume Light_Ortho {data_CubeVertices, data_CubeIndices, 8u, 36u};
    sq::Volume Light_Point {data_SphereVertices, data_SphereIndices, 24u, 132u};
    sq::Volume Light_Spot {data_ConeVertices, data_ConeIndices, 18u, 96u};
};


struct TargetTextures : sq::NonCopyable {

    sq::Texture2D Depth_FullSize {gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::NearestClamp()};
    sq::Texture2D Depth_HalfSize {gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::NearestClamp()};
    sq::Texture2D Depth_QterSize {gl::DEPTH_STENCIL, gl::DEPTH24_STENCIL8, sq::Texture::NearestClamp()};

    sq::Texture2D Gbuffer_MainDiff {gl::RGB, gl::RGB8, sq::Texture::NearestClamp()};
    sq::Texture2D Gbuffer_MainSurf {gl::RGB, gl::RGB10, sq::Texture::NearestClamp()};
    sq::Texture2D Gbuffer_MainNorm {gl::RGB, gl::RGB10, sq::Texture::NearestClamp()};
    sq::Texture2D Gbuffer_MainSpec {gl::RGB, gl::RGB8, sq::Texture::NearestClamp()};

    sq::Texture2D Lighting_Main {gl::RGB, gl::RGB16F, sq::Texture::LinearClamp()};

    sq::Texture2D Volumetric_Shafts {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};

    sq::Texture2D Effects_SSAO {gl::RED, gl::R8, sq::Texture::LinearClamp()};
    sq::Texture2D Effects_Bloom {gl::RGB, gl::RGB8, sq::Texture::LinearClamp()};

    sq::Texture2D Composite_Main {gl::RGBA, gl::RGBA8, sq::Texture::LinearClamp()};

    void update_options();
};


struct GenericShaders : sq::NonCopyable {

    GenericShaders();

    sq::Shader VS_FullScreen {gl::VERTEX_SHADER};
    sq::Shader VS_MainStencil {gl::VERTEX_SHADER};
    sq::Shader VS_ReflStencil {gl::VERTEX_SHADER};

    sq::Shader FS_PassThrough {gl::FRAGMENT_SHADER};
    sq::Shader FS_FillSpace {gl::FRAGMENT_SHADER};

    sq::PreProcessor preprocs; sq::Pipeline pipeline;

    void update_options();
};


struct SharedStuff {
    const StencilVolumes& volumes;
    const TargetTextures& textures;
    const GenericShaders& shaders;
};

}}
