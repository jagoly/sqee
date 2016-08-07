#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/debug/Logging.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/gl/Context.hpp>
#include <sqee/misc/Files.hpp>

#include "Material.hpp"

using namespace sqt;

Material::Material(const string& _path) {
    string path = sq::static_path() + "materials/" + _path + ".mtrl";

    for (const auto& linePair : sq::tokenise_file(path)) {
        const vector<string>& line = linePair.first;
        const string& key = line.front();

        if (line.front().front() == '#') continue;
        else if (key == "mask") mask = acquire_Texture(line[1] + "_mask");
        else if (key == "diff") diff = acquire_Texture(line[1] + "_diff");
        else if (key == "norm") norm = acquire_Texture(line[1] + "_norm");
        else if (key == "spec") spec = acquire_Texture(line[1] + "_spec");
        else sq::log_error("Invalid key '%s' in material '%s'", key, path);
    }
}

void Material::bind_dns() const {
    static auto& context = sq::Context::get();
    if (diff.check()) context.bind_Texture(diff.get(), 0u);
    if (norm.check()) context.bind_Texture(norm.get(), 1u);
    if (spec.check()) context.bind_Texture(spec.get(), 2u);
}
