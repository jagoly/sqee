#include "sqee/redist/gl_ext_3_3.hpp"
#include "sqee/app/logging.hpp"
#include "sqee/gl/textures.hpp"
#include "sqee/misc/files.hpp"
#include "sqee/render/skin.hpp"

using namespace sq;

void Skin::create(const string& _path) {
    string path = res::skin_path() + _path + ".sqs";
    vector<vector<string>> fileVec(sq::get_words_from_file(path));

    struct TexPaths {
        int wrapMode = -1;
        unordered_set<string> extraSet;
        string norm, diff, spec;
    }; vector<TexPaths> TexPathsVec;

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
            if (key == "material") TexPathsVec.emplace_back();
            else if (key == "wrap") {
                if      (line[1] == "clamp") TexPathsVec.back().wrapMode = 0;
                else if (line[1] == "repeat") TexPathsVec.back().wrapMode = 1;
                else if (line[1] == "mirror") TexPathsVec.back().wrapMode = 2;
                else throw;
            }
            else if (key == "diff") TexPathsVec.back().diff = line[1];
            else if (key == "norm") TexPathsVec.back().norm = line[1];
            else if (key == "spec") TexPathsVec.back().spec = line[1];
            else if (key == "extra")
                for (uint i = 1u; i < line.size(); i++)
                    TexPathsVec.back().extraSet.emplace(line[i]);
            else throw;
            continue;
        }
    }

    #ifdef SQEE_DEBUG
    if (mtrlVec.size() != TexPathsVec.size())
        log_warning("mCount mismatch when loading skin from %s", path);
    #endif

    for (uint i = 0; i < TexPathsVec.size(); i++) {
        const TexPaths& paths = TexPathsVec[i];
        Material& mtrl = mtrlVec[i];

        mtrl.punch = paths.extraSet.count("punch");

        Texture::Preset preset;
        if      (paths.wrapMode == 0) preset = Texture2D::M_C();
        else if (paths.wrapMode == 1) preset = Texture2D::M_R();
        else if (paths.wrapMode == 2) preset = Texture2D::M_M();

        if (!paths.diff.empty()) {
            const string name = "diff/" + paths.diff;
            if (!(mtrl.diff = res::tex2D().get(name))) {
                mtrl.diff = res::tex2D().add(name);
                mtrl.diff->create(gl::RGBA, gl::RGBA8, 4);
                mtrl.diff->set_preset(preset);
                mtrl.diff->buffer_file(name);
                mtrl.diff->gen_mipmap();
            } mtrl.glMode = mtrl.glMode | 1;
        }
        if (!paths.norm.empty()) {
            const string name = "norm/" + paths.norm;
            if (!(mtrl.norm = res::tex2D().get(name))) {
                mtrl.norm = res::tex2D().add(name);
                mtrl.norm->create(gl::RGB, gl::RGB8, 3);
                mtrl.norm->set_preset(preset);
                mtrl.norm->buffer_file(name);
                mtrl.norm->gen_mipmap();
            } mtrl.glMode = mtrl.glMode | 2;
        }
        if (!paths.spec.empty()) {
            const string name = "spec/" + paths.spec;
            if (!(mtrl.spec = res::tex2D().get(name))) {
                mtrl.spec = res::tex2D().add(name);
                mtrl.spec->create(gl::RGBA, gl::RGBA8, 4);
                mtrl.spec->set_preset(preset);
                mtrl.spec->buffer_file(name);
                mtrl.spec->gen_mipmap();
            } mtrl.glMode = mtrl.glMode | 4;
        }
    }
}

Skin::~Skin() {
    // No cleanup needed
}

void Skin::bind_textures(uint _mtrl) {
    const Material& mtrl = mtrlVec[_mtrl];
    if (mtrl.diff != nullptr) mtrl.diff->bind(gl::TEXTURE0);
    if (mtrl.norm != nullptr) mtrl.norm->bind(gl::TEXTURE1);
    if (mtrl.spec != nullptr) mtrl.spec->bind(gl::TEXTURE2);
}

void Skin::bind_textures(uint _mtrl, bool _diff, bool _norm, bool _spec) {
    const Material& mtrl = mtrlVec[_mtrl];
    if (_diff) mtrl.diff->bind(gl::TEXTURE0);
    if (_norm) mtrl.norm->bind(gl::TEXTURE1);
    if (_spec) mtrl.spec->bind(gl::TEXTURE2);
}


ResHolder<Skin>& sq::res::skin() {
    static ResHolder<Skin> holder;
    return holder;
}
string& sq::res::skin_path() {
    static string path;
    return path;
}
