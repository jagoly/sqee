#include "../Options.hpp"
#include "SharedStuff.hpp"

using namespace sqt::render;

void TargetTextures::update_options() {

    static const auto& options = Options::get();

    Depth_FullSize.allocate_storage(options.Window_Size, false);
    Depth_HalfSize.allocate_storage(options.Window_Size/2u, false);
    Depth_QterSize.allocate_storage(options.Window_Size/4u, false);

    Gbuffer_MainDiff.allocate_storage(options.Window_Size, false);
    Gbuffer_MainSurf.allocate_storage(options.Window_Size, false);
    Gbuffer_MainNorm.allocate_storage(options.Window_Size, false);
    Gbuffer_MainSpec.allocate_storage(options.Window_Size, false);

    Lighting_Main.allocate_storage(options.Window_Size, false);

    Composite_Main.allocate_storage(options.Window_Size, false);

    if (options.Shafts_Quality == 0u) Volumetric_Shafts.delete_object();
    else Volumetric_Shafts.allocate_storage(options.Window_Size/2u, false);

    if (options.SSAO_Quality == 0u) Effects_SSAO.delete_object();
    else Effects_SSAO.allocate_storage(options.Window_Size/2u, false);

    if (options.Bloom_Enable == false) Effects_Bloom.delete_object();
    else Effects_Bloom.allocate_storage(options.Window_Size/4u, false);
}

GenericShaders::GenericShaders() {
    VS_MainStencil.add_uniform("matrix"); // mat4
    VS_ReflStencil.add_uniform("matrix"); // mat4
}

void GenericShaders::update_options() {
    preprocs.load(VS_FullScreen, "generic/FullScreen_vs");
    preprocs.load(VS_MainStencil, "generic/MainStencil_vs");
    preprocs.load(VS_ReflStencil, "generic/ReflStencil_vs");
    preprocs.load(FS_PassThrough, "generic/PassThrough_fs");
    preprocs.load(FS_FillSpace, "generic/FillSpace_fs");
}
