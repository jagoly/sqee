#pragma once
#include "forward.hpp"

#include <vector>

namespace sq {

class Skin : NonCopyable {
public:
    struct Mode { enum : int { norm=1, diff=2, spec=4, ambi=8, all=1|2|4|8}; };

    void load(const string& _filePath, ResHolder<Texture>& _texH);
    void bind_textures(uint _mtrl = 0, int _override = Mode::all);
    int get_mode(uint _mtrl = 0);
    uint mtrl_num();

private:
    struct Material {
        int mode = 0;
        sq::Texture* norm = nullptr;
        sq::Texture* diff = nullptr;
        sq::Texture* spec = nullptr;
        sq::Texture* ambi = nullptr;
    };
    std::vector<Material> mtrlVec;
};

}
