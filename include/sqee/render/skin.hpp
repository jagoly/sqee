#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/resholder.hpp>

namespace sq {

class Skin : NonCopyable {
public:
    void create(const string& _path);
    ~Skin();

    struct Material {
        bool punch = false;
        sq::Texture2D* diff = nullptr;
        sq::Texture2D* norm = nullptr;
        sq::Texture2D* spec = nullptr;
        ivec3 glDNS;
    }; vector<Material> mtrlVec;

    void bind_textures(uint _mtrl);
    void bind_textures(uint _mtrl, bool _diff, bool _norm, bool _spec);
};

namespace res {
ResHolder<Skin>& skin();
string& skin_path();
}

}
