#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/ResHolder.hpp>

namespace sq {

/// The SQEE Skin class
class Skin : NonCopyable {
public:
    Skin() = default; ~Skin();
    Skin(const string& _path);
    void create(const string& _path);

    struct Material {
        bool punch = false;
        sq::Texture2D* diff = nullptr;
        sq::Texture2D* norm = nullptr;
        sq::Texture2D* spec = nullptr;
        ivec3 glDNS;
    }; vector<Material> mtrlVec;

    void bind_textures(uint _mtrl) const;
    void bind_textures(uint _mtrl, bool _diff, bool _norm, bool _spec) const;
};

namespace res {
ResHolder<Skin>& skin();
}

}
