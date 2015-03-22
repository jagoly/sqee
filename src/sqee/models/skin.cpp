#include "gl/gl_ext_3_3.hpp"
#include "app/logging.hpp"
#include "gl/textures.hpp"
#include "misc/files.hpp"
#include "models/skin.hpp"

using namespace sq;

void Skin::create(const string& _path) {
    string path = "res/models/skins/" + _path + ".sqs";
    std::vector<std::vector<string>> fileVec(sq::get_words_from_file(path));

    struct TexPaths {
        bool punch, shadow;
        int wrapMode = -1;
        string norm, diff, spec;
    }; std::vector<TexPaths> TexPathsVec;

    string section = "";
    for (auto& line : fileVec) {
        const string& key = line[0];
        if (key[0] == '#') continue;
        if (key == "{") {
            if (!section.empty()) throw;
            section = line[1]; continue;
        }
        if (key == "}") {
            if (section.empty()) throw;
            section.clear(); continue;
        }

        if (section == "header") {
            if (key == "mCount")
                mtrlVec.resize(std::stoi(line[1]));
            else throw;
            continue;
        }

        if (section == "materials") {
            if (key == "material")
                TexPathsVec.emplace_back();
            else if (key == "wrap") {
                if      (line[1] == "clamp") TexPathsVec.back().wrapMode = 0;
                else if (line[1] == "repeat") TexPathsVec.back().wrapMode = 1;
                else if (line[1] == "mirror") TexPathsVec.back().wrapMode = 2;
                else throw;
            }
            else if (key == "punch") {
                if      (line[1] == "no") TexPathsVec.back().punch = false;
                else if (line[1] == "yes") TexPathsVec.back().punch = true;
                else throw;
            }
            else if (key == "shadow") {
                if      (line[1] == "no") TexPathsVec.back().shadow = false;
                else if (line[1] == "yes") TexPathsVec.back().shadow = true;
                else throw;
            }
            else if (key == "norm")
                TexPathsVec.back().norm = line[1];
            else if (key == "diff")
                TexPathsVec.back().diff = line[1];
            else if (key == "spec")
                TexPathsVec.back().spec = line[1];
            else throw;
            continue;
        }
    }

    #ifdef SQEE_DEBUG
    if (mtrlVec.size() != TexPathsVec.size())
        log_warning("mCount mismatch when loading skin from $0", path);
    #endif

    for (uint i = 0; i < TexPathsVec.size(); i++) {
        const TexPaths& paths = TexPathsVec[i];
        Material& mtrl = mtrlVec[i];

        mtrl.punch = paths.punch;
        mtrl.shadow = paths.shadow;

        Texture::Preset preset;
        if      (paths.wrapMode == 0) preset = Texture::Preset::M_C;
        else if (paths.wrapMode == 1) preset = Texture::Preset::M_R;
        else if (paths.wrapMode == 2) preset = Texture::Preset::M_M;

        if (!paths.norm.empty()) {
            const string name = "models/norm/" + paths.norm;
            if (!(mtrl.norm = res::texture().get(name))) {
                mtrl.norm = res::texture().add(name);
                mtrl.norm->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, 3, preset);
                mtrl.norm->buffer_file(name, 0);
                mtrl.norm->gen_mipmap();
            } mtrl.glMode = mtrl.glMode | 1;
        }
        if (!paths.diff.empty()) {
            const string name = "models/diff/" + paths.diff;
            if (!(mtrl.diff = res::texture().get(name))) {
                mtrl.diff = res::texture().add(name);
                mtrl.diff->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, preset);
                mtrl.diff->buffer_file(name, 0);
                mtrl.diff->gen_mipmap();
            } mtrl.glMode = mtrl.glMode | 2;
        }
        if (!paths.spec.empty()) {
            const string name = "models/spec/" + paths.spec;
            if (!(mtrl.spec = res::texture().get(name))) {
                mtrl.spec = res::texture().add(name);
                mtrl.spec->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, 4, preset);
                mtrl.spec->buffer_file(name, 0);
                mtrl.spec->gen_mipmap();
            } mtrl.glMode = mtrl.glMode | 4;
        }
    }
}

Skin::~Skin() {
    // No cleanup needed
}

std::forward_list<uint> Skin::filtered(char _punch, char _shadow) {
    std::forward_list<uint> retList;
    for (uint i = 0; i < mtrlVec.size(); i++) {
        if (_punch != -1 && _punch != mtrlVec[i].punch) continue;
        if (_shadow != -1 && _shadow != mtrlVec[i].shadow) continue;
        retList.push_front(i);
    } return retList;
}

void Skin::bind_textures(uint _mtrl) {
    const Material& mtrl = mtrlVec[_mtrl];
    if (mtrl.norm != nullptr) mtrl.norm->bind(gl::TEXTURE0);
    if (mtrl.diff != nullptr) mtrl.diff->bind(gl::TEXTURE1);
    if (mtrl.spec != nullptr) mtrl.spec->bind(gl::TEXTURE2);
}

void Skin::bind_textures(uint _mtrl, bool _norm, bool _diff, bool _spec) {
    const Material& mtrl = mtrlVec[_mtrl];
    if (_norm) mtrl.norm->bind(gl::TEXTURE0);
    if (_diff) mtrl.diff->bind(gl::TEXTURE1);
    if (_spec) mtrl.spec->bind(gl::TEXTURE2);
}


ResHolder<Skin>& sq::res::skin() {
    static ResHolder<Skin> holder;
    return holder;
}
