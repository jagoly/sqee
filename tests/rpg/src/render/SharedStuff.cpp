#include "../main/Options.hpp"

#include "SharedStuff.hpp"

using namespace sqt::render;

//============================================================================//

TargetTextures::TargetTextures(const Options& options)
{
    Depth_FullSize.allocate_storage(sq::TexFormat::DEP24S8, options.Window_Size, false);
    Depth_HalfSize.allocate_storage(sq::TexFormat::DEP24S8, options.Window_Size / 2u, false);
    Depth_QterSize.allocate_storage(sq::TexFormat::DEP24S8, options.Window_Size / 4u, false);

    Gbuffer_MainDiff.allocate_storage(sq::TexFormat::RGB8_UN, options.Window_Size, false);
    Gbuffer_MainSurf.allocate_storage(sq::TexFormat::RGB8_SN, options.Window_Size, false);
    Gbuffer_MainNorm.allocate_storage(sq::TexFormat::RGB8_SN, options.Window_Size, false);
    Gbuffer_MainSpec.allocate_storage(sq::TexFormat::RGB8_UN, options.Window_Size, false);

    Lighting_Main.allocate_storage(sq::TexFormat::RGB16_FP, options.Window_Size, false);
    Lighting_Main.set_filter_mode(true, false);

    if (options.Shafts_Quality > 0u)
        Volumetric_Shafts.allocate_storage(sq::TexFormat::RGB8_UN, options.Window_Size / 2u, false),
        Volumetric_Shafts.set_filter_mode(true, false);

    if (options.SSAO_Quality > 0u)
        Effects_SSAO.allocate_storage(sq::TexFormat::R8_UN, options.Window_Size / 2u, false),
        Effects_SSAO.set_filter_mode(true, false);

    if (options.Bloom_Enable == true)
        Effects_Bloom.allocate_storage(sq::TexFormat::RGB8_UN, options.Window_Size / 4u, false),
        Effects_Bloom.set_filter_mode(true, false);

    Composite_Main.allocate_storage(sq::TexFormat::RGBA8_UN, options.Window_Size, false);
    Composite_Main.set_filter_mode(true, false);
}
