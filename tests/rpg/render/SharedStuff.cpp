#include "../Options.hpp"
#include "SharedStuff.hpp"

using namespace sqt::render;

//============================================================================//

TargetTextures::TargetTextures()
{
    Lighting_Main.set_filter_mode(true);
    Volumetric_Shafts.set_filter_mode(true);
    Effects_SSAO.set_filter_mode(true);
    Effects_Bloom.set_filter_mode(true);
    Composite_Main.set_filter_mode(true);
}

//============================================================================//

void TargetTextures::update_options(const Options& options)
{
    Depth_FullSize.allocate_storage(options.Window_Size);
    Depth_HalfSize.allocate_storage(options.Window_Size / 2u);
    Depth_QterSize.allocate_storage(options.Window_Size / 4u);

    Gbuffer_MainDiff.allocate_storage(options.Window_Size);
    Gbuffer_MainSurf.allocate_storage(options.Window_Size);
    Gbuffer_MainNorm.allocate_storage(options.Window_Size);
    Gbuffer_MainSpec.allocate_storage(options.Window_Size);

    Lighting_Main.allocate_storage(options.Window_Size);

    Composite_Main.allocate_storage(options.Window_Size);

    if (options.Shafts_Quality == 0u) Volumetric_Shafts.delete_object();
    else Volumetric_Shafts.allocate_storage(options.Window_Size / 2u);

    if (options.SSAO_Quality == 0u) Effects_SSAO.delete_object();
    else Effects_SSAO.allocate_storage(options.Window_Size / 2u);

    if (options.Bloom_Enable == false) Effects_Bloom.delete_object();
    else Effects_Bloom.allocate_storage(options.Window_Size / 4u);
}
