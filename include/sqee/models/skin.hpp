#pragma once
#include "forward.hpp"

#include <vector>

#include "misc/resholder.hpp"

namespace sq {

class Skin {
public:
    struct Mode {
        enum : int { norm=1, diff=2, spec=4, ambi=8, all=1|2|4|8};
    };

    struct Material {
        int mode = 0;
        sq::Texture* norm = nullptr;
        sq::Texture* diff = nullptr;
        sq::Texture* spec = nullptr;
        sq::Texture* ambi = nullptr;
    };

    void create(const string& _filePath);
    ~Skin();

    void bind_textures(uint _mtrl = 0, int _override = Mode::all);

    int get_mode(uint _i);
    uint get_count();

private:
    std::vector<Material> mtrlVec;
};

namespace res { ResHolder<Skin>& skin(); }

}
