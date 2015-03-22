#pragma once
#include "forward.hpp"

#include <forward_list>
#include <vector>

#include "misc/resholder.hpp"

namespace sq {

class Skin : NonCopyable {
public:
    void create(const string& _path);
    ~Skin();

    struct Material {
        bool punch, shadow;
        sq::Texture* norm = nullptr;
        sq::Texture* diff = nullptr;
        sq::Texture* spec = nullptr;
        int glMode = 0;
    };
    std::vector<Material> mtrlVec;

    std::forward_list<uint> filtered(char _punch, char _shadow);

    void bind_textures(uint _mtrl);
    void bind_textures(uint _mtrl, bool _norm, bool _diff, bool _spec);
};

namespace res { ResHolder<Skin>& skin(); }

}
