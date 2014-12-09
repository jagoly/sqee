#include <fstream>
#include <sstream>

#include <models/skin.hpp>
#include <misc/files.hpp>

using namespace sq;

void Skin::load(const string& _filePath, TexHolder& _texH) {
    string filePath = SQ_MODELS "skins/" + _filePath + ".sq_skin";
    std::ifstream src(filePath);

    if (!src.is_open()) cout << "ERROR: Couldn't open file \"" << filePath << "\"" << endl;

    struct TexPaths {
        string norm, diff, spec, ambi;
    }; vector<TexPaths> TexPathsVec;

    string line;
    string section = "";
    while (std::getline(src, line)) {
        vector<string> vec;
        {   std::stringstream stream(line); string val;
            while (stream >> val) vec.emplace_back(val);
        }

        if (vec.empty() || vec[0] == "#") continue;

        if (vec[0] == "{") {
            if (!section.empty()) throw; // already in a section
            section = vec[1]; continue;
        }
        if (vec[0] == "}") {
            if (section.empty()) throw; // not in a section
            section = ""; continue;
        }

        if (section == "header") {
            if (vec[0] == "mCount")
                mtrlVec.resize(std::stoi(vec[1]));
            else throw;
        }

        if (section == "materials") {
            if (line == "material")
                TexPathsVec.emplace_back();
            else if (vec[0] == "norm")
                TexPathsVec.back().norm = vec[1];
            else if (vec[0] == "diff")
                TexPathsVec.back().diff = vec[1];
            else if (vec[0] == "spec")
                TexPathsVec.back().spec = vec[1];
            else if (vec[0] == "ambi")
                TexPathsVec.back().ambi = vec[1];
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
