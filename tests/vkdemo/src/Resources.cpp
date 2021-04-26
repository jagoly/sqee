#include "Resources.hpp"

#include <sqee/misc/Json.hpp>
#include <sqee/misc/Files.hpp>

using namespace demo;

//============================================================================//

ResourceCaches::ResourceCaches()
{
    meshes.assign_factory([](const String& key)
    {
        auto result = std::make_unique<sq::VulkMesh>();
        result->load_from_file(sq::build_string("assets/", key, ".sqm"), false);
        return result;
    });

    textures.assign_factory([](const String& key)
    {
        auto result = std::make_unique<sq::VulkTexture>();
        result->load_from_file_2D(sq::build_string("assets/", key));
        return result;
    });

    pipelines.assign_factory([this](const JsonValue& key)
    {
        auto result = std::make_unique<sq::Pipeline>();
        result->load_from_json(key, passConfigMap);
        return result;
    });

    materials.assign_factory([this](const JsonValue& key)
    {
        auto result = std::make_unique<sq::VulkMaterial>();
        result->load_from_json(key, pipelines, textures);
        return result;
    });
}

ResourceCaches::~ResourceCaches() = default;
