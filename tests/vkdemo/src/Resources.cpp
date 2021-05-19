#include "Resources.hpp"

#include <sqee/misc/Json.hpp>
#include <sqee/misc/Files.hpp>

using namespace demo;

//============================================================================//

ResourceCaches::ResourceCaches()
{
    meshes.assign_factory([](const String& key)
    {
        auto result = std::make_unique<sq::Mesh>();
        result->load_from_file("assets/" + key);
        return result;
    });

    textures.assign_factory([](const String& key)
    {
        auto result = std::make_unique<sq::Texture>();
        result->load_from_file_2D("assets/" + key);
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
        auto result = std::make_unique<sq::Material>();
        result->load_from_json(key, pipelines, textures);
        return result;
    });
}

ResourceCaches::~ResourceCaches() = default;
