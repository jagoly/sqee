#pragma once

#include <gl/gl.hpp>
#include <gl/textures.hpp>
#include <misc/containers.hpp>

namespace sq {

class Skin {
public:
    struct Mode { enum : int { norm=1, diff=2, spec=4, ambi=8, all=1|2|4|8}; };

    void load(const string& _filePath, TexHolder& _texH);
    void load_simple(const string& _norm, const string& _diff, const string& _spec,
                     const string& _ambi, TexHolder& _texH);
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
    vector<Material> mtrlVec;
};

}
