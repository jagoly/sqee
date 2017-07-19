#include <fstream>
#include <optional>

#include <sqee/redist/gl_loader.hpp>
#include <sqee/redist/nl_json.hpp>

#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/misc/Files.hpp>

#include <sqee/render/Material.hpp>

using namespace sq;
using Json = nlohmann::json;

namespace { // anonymous

std::optional<Vec3F> impl_as_Vec3F(const string& str)
{
    SQASSERT(str.size() >= 11u, "");
    SQASSERT(str.front() != ' ', "");
    SQASSERT(str.back() != ' ', "");

    const auto endX = str.find_first_of(' ', 0u);
    const auto startY = str.find_first_not_of(' ', endX);
    const auto endY = str.find_first_of(' ', startY);
    const auto startZ = str.find_first_not_of(' ', endY);

    // if str has less or more than three words
    if (startZ == string::npos) return {};
    if (str.find(' ', startZ) != string::npos) return {};

    const char* strX = str.c_str() + 0u;
    const char* strY = str.c_str() + startY;
    const char* strZ = str.c_str() + startZ;

    Vec3F result; char* tail;

    result.x = std::strtof(strX, &tail);
    SQASSERT(tail != strX, "");

    result.y = std::strtof(strY, &tail);
    SQASSERT(tail != strY, "");

    result.z = std::strtof(strZ, &tail);
    SQASSERT(tail != strZ, "");

    return result;
}

std::optional<string> impl_try_get(const Json& json, const string& key)
{
    const auto iter = json.find(key);
    if (iter == json.end()) return {};
    return iter->get<string>();
}

} // anonymous namespace

Material::Material(const string& path, ResourceCache<Texture2D>& textures)
{
    const string::size_type splitPos = path.find(':');
    log_assert(splitPos != string::npos, "bad path '%s'", path);

    const string package = path.substr(0u, splitPos);
    const string name = path.substr(splitPos + 1u);

    std::ifstream srcFile("assets/packages/" + package + "/materials.json");
    const auto json = Json::parse(srcFile).at(name);

    if (auto diffuse = impl_try_get(json, "diffuse"))
    {
        if (auto o = impl_as_Vec3F(*diffuse)) mDiffuseColour = *o;
        else mDiffuseTexture = textures.acquire(*diffuse);
    }

    if (auto normal = impl_try_get(json, "normal"))
    {
        mNormalTexture = textures.acquire(*normal);
    }

    if (auto specular = impl_try_get(json, "specular"))
    {
        if (auto o = impl_as_Vec3F(*specular)) mSpecularColour = *o;
        else mSpecularTexture = textures.acquire(*specular);
    }

    if (auto mask = impl_try_get(json, "mask"))
    {
        mMaskTexture = textures.acquire(*mask);
    }
}