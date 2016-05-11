#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

// Forward Declarations /////
namespace sq { class Texture2D; }

namespace sq {

/// The SQEE Skin class
class Skin : NonCopyable {
public:
    Skin() = default;
    Skin(const string& _path);
    void create(const string& _path);

    struct Material {
        bool punch = false;
        sq::Texture2D* diff = nullptr;
        sq::Texture2D* norm = nullptr;
        sq::Texture2D* spec = nullptr;
        Vec3I glDNS = {0, 0, 0};
    }; vector<Material> mtrlVec;

    bool hasPunchThru = false;

    void bind_textures(uint _mtrl) const;
    void bind_textures(uint _mtrl, bool _diff, bool _norm, bool _spec) const;
};

}
