#include <models/skin.hpp>
#include <misc/files.hpp>

using namespace sq;

void Skin::load(const string& _filePath, TexHolder& _texH) {
    string filePath = SQ_MODELS "skins/" + _filePath + ".sq_skin";
    vector<vector<string>> fileVec(sq::get_words_from_file(filePath));

    struct TexPaths { string norm, diff, spec, ambi; };
    vector<TexPaths> TexPathsVec;

    string section = "";
    for (const vector<string>& line : fileVec) {
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
            if (key == "mCount") mtrlVec.resize(std::stoi(line[1]));
            else throw; // invalid key
            continue;
        }

        if (section == "materials") {
            if (key == "material")
                TexPathsVec.emplace_back();
            else if (key == "norm")
                TexPathsVec.back().norm = line[1];
            else if (key == "diff")
                TexPathsVec.back().diff = line[1];
            else if (key == "spec")
                TexPathsVec.back().spec = line[1];
            else if (key == "ambi")
                TexPathsVec.back().ambi = line[1];
            else throw;
            continue;
        }
    }

    #ifdef SQEE_DEBUG
    if (mtrlVec.size() != TexPathsVec.size())
        cout << "WARNING: mCount mismatch when loading skin from \""
             << filePath << "\"" << endl;
    #endif

    for (uint i = 0; i < TexPathsVec.size(); i++) {
        const TexPaths& paths = TexPathsVec[i];
        Material& mtrl = mtrlVec[i];
        if (!paths.norm.empty()) {
            mtrl.mode = mtrl.mode | Mode::norm;
            const string name = "models/norm/" + paths.norm;
            if (!(mtrl.norm = _texH.get(name))) {
                mtrl.norm = _texH.add(name);
                mtrl.norm->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, name, Texture::Preset::L_C);
            }
        }
        if (!paths.diff.empty()) {
            mtrl.mode = mtrl.mode | Mode::diff;
            const string name = "models/diff/" + paths.diff;
            if (!(mtrl.diff = _texH.get(name))) {
                mtrl.diff = _texH.add(name);
                mtrl.diff->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, name, Texture::Preset::L_C);
            }
        }
        if (!paths.spec.empty()) {
            mtrl.mode = mtrl.mode | Mode::spec;
            const string name = "models/spec/" + paths.spec;
            if (!(mtrl.spec = _texH.get(name))) {
                mtrl.spec = _texH.add(name);
                mtrl.spec->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, name, Texture::Preset::L_C);
            }
        }
        if (!paths.ambi.empty()) {
            mtrl.mode = mtrl.mode | Mode::ambi;
            const string name = "models/ambi/" + paths.ambi;
            if (!(mtrl.ambi = _texH.get(name))) {
                mtrl.ambi = _texH.add(name);
                mtrl.ambi->create(gl::TEXTURE_2D, gl::RED, gl::R8, name, Texture::Preset::L_C);
            }
        }
    }
}

void Skin::load_simple(const string& _norm, const string& _diff, const string& _spec,
                       const string& _ambi, TexHolder& _texH) {
    mtrlVec.resize(1);
    Material& mtrl = mtrlVec.front();
    if (!_norm.empty()) {
        mtrl.mode = mtrl.mode | Mode::norm;
        const string name = "models/norm/" + _norm;
        if (!(mtrl.norm = _texH.get(name))) {
            mtrl.norm = _texH.add(name);
            mtrl.norm->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, name, Texture::Preset::L_C);
        }
    }
    if (!_diff.empty()) {
        mtrl.mode = mtrl.mode | Mode::diff;
        const string name = "models/diff/" + _diff;
        if (!(mtrl.diff = _texH.get(name))) {
            mtrl.diff = _texH.add(name);
            mtrl.diff->create(gl::TEXTURE_2D, gl::RGBA, gl::RGBA8, name, Texture::Preset::L_C);
        }
    }
    if (!_spec.empty()) {
        mtrl.mode = mtrl.mode | Mode::spec;
        const string name = "models/spec/" + _spec;
        if (!(mtrl.spec = _texH.get(name))) {
            mtrl.spec = _texH.add(name);
            mtrl.spec->create(gl::TEXTURE_2D, gl::RGB, gl::RGB8, name, Texture::Preset::L_C);
        }
    }
    if (!_ambi.empty()) {
        mtrl.mode = mtrl.mode | Mode::ambi;
        const string name = "models/ambi/" + _ambi;
        if (!(mtrl.ambi = _texH.get(name))) {
            mtrl.ambi = _texH.add(name);
            mtrl.ambi->create(gl::TEXTURE_2D, gl::RED, gl::R8, name, Texture::Preset::L_C);
        }
    }
}

void Skin::bind_textures(uint _mtrl, int _override) {
    const Material& mtrl = mtrlVec[_mtrl];
    if (mtrl.mode & _override & Mode::norm) mtrl.norm->bind(gl::TEXTURE0);
    if (mtrl.mode & _override & Mode::diff) mtrl.diff->bind(gl::TEXTURE1);
    if (mtrl.mode & _override & Mode::spec) mtrl.spec->bind(gl::TEXTURE2);
    if (mtrl.mode & _override & Mode::ambi) mtrl.ambi->bind(gl::TEXTURE3);
}

int Skin::get_mode(uint _mtrl) {
    return mtrlVec[_mtrl].mode;
}

uint Skin::mtrl_num() {
    return mtrlVec.size();
}
