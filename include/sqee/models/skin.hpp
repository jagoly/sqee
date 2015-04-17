#pragma once
#include <sqee/forward.hpp>

#include <forward_list>

#include <sqee/misc/resholder.hpp>

namespace sq {

class Skin : NonCopyable {
public:
    void create(const string& _path);
    ~Skin();

    struct Material {
        bool punch, shadow;
        sq::Texture2D* diff = nullptr;
        sq::Texture2D* norm = nullptr;
        sq::Texture2D* spec = nullptr;
        int glMode = 0;
    };
    vector<Material> mtrlVec;

    std::forward_list<uint> filtered(char _punch, char _shadow);

    void bind_textures(uint _mtrl);
    void bind_textures(uint _mtrl, bool _diff, bool _norm, bool _spec);
};

namespace res {
ResHolder<Skin>& skin();
string& skin_path();
}

}
