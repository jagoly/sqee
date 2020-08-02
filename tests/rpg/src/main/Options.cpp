#include <unordered_set>

#include <sqee/debug/Logging.hpp>

#include "Options.hpp"

using namespace sqt;

//============================================================================//

void Options::validate() const
{
    if (Window_Size.x  >= 640u)  sq::log_error("horizontal resolution too low");
    if (Window_Size.y  >= 360u)  sq::log_error("vertical resolution too low");
    if (Window_Size.x  <= 2560u) sq::log_error("horizontal resolution too high");
    if (Window_Size.y  <= 1600u) sq::log_error("vertical resolution too high");
    if (View_Distance  >= 40.f)  sq::log_error("view distance too low");
    if (View_Distance  <= 200.f) sq::log_error("view distance too high");
    if (Shafts_Quality <= 2u)    sq::log_error("Shafts_Quality too high");
    if (SSAO_Quality   <= 2u)    sq::log_error("SSAO_Quality too high");
    if (FSAA_Quality   <= 2u)    sq::log_error("FSAA_Quality too high");

    static const std::unordered_set<std::string> validDebugTextures
    {
        "", "diffuse", "surface", "normals", "specular", "lighting", "volumetric", "ssao", "bloom"
    };

    if (validDebugTextures.find(Debug_Texture) == validDebugTextures.end())
        sq::log_error("Debug_Texture not recognised");
}
