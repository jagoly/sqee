#include <sqee/redist/gl_ext_4_1.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

Skin::~Skin() {
    // empty
}

Skin::Skin(const string& _path) {
    create(_path);
}

void Skin::create(const string& _path) {
    string path = res::path() + "skins/" + _path + ".sqs";
    vector<vector<string>> fileVec = tokenise_file(path);

    struct MtrlSpec {
        int wrapMode = -1;
        unordered_set<string> extraSet;
        string norm, diff, spec;
    }; vector<MtrlSpec> specVec;

    string section = "";
    for (auto& line : fileVec) {
        const string& key = line[0]; if (key[0] == '#') continue;
        if (key == "{") { if (!section.empty()) throw; section = line[1]; continue; }
        if (key == "}") { if (section.empty()) throw; section.clear(); continue; }

        if (section == "header") { throw; }

        else if (section == "materials") {
            if (key == "material") specVec.emplace_back();
            else if (key == "wrap") {
                if      (line[1] == "clamp") specVec.back().wrapMode = 0;
                else if (line[1] == "repeat") specVec.back().wrapMode = 1;
                else if (line[1] == "mirror") specVec.back().wrapMode = 2;
                else throw;
            }
            else if (key == "diff") specVec.back().diff = line[1];
            else if (key == "norm") specVec.back().norm = line[1];
            else if (key == "spec") specVec.back().spec = line[1];
            else if (key == "extra")
                for (uint i = 1u; i < line.size(); i++)
                    specVec.back().extraSet.emplace(line[i]);
            else throw;
        }

        else throw;
    }

    for (const MtrlSpec& spec : specVec) {
        mtrlVec.emplace_back();

        mtrlVec.back().punch = spec.extraSet.count("punch");

        Texture::Preset preset;
        if      (spec.wrapMode == 0) preset = Texture::MipmapClamp();
        else if (spec.wrapMode == 1) preset = Texture::MipmapRepeat();
        else if (spec.wrapMode == 2) preset = Texture::MipmapMirror();

        if (spec.diff.empty() == false) {
            const string name = "diff/" + spec.diff;
            if ((mtrlVec.back().diff = res::tex2D().get(name)) == nullptr) {
                mtrlVec.back().diff = res::tex2D().add(name);
                mtrlVec.back().diff->create(gl::RGBA, gl::RGBA8, 4u, true);
                mtrlVec.back().diff->set_preset(preset);
                mtrlVec.back().diff->buffer_auto(name);
            } mtrlVec.back().glDNS.x = true;
        }
        if (spec.norm.empty() == false) {
            const string name = "norm/" + spec.norm;
            if ((mtrlVec.back().norm = res::tex2D().get(name)) == nullptr) {
                mtrlVec.back().norm = res::tex2D().add(name);
                mtrlVec.back().norm->create(gl::RGB, gl::RGB8, 3u, true);
                mtrlVec.back().norm->set_preset(preset);
                mtrlVec.back().norm->buffer_auto(name);
            } mtrlVec.back().glDNS.y = true;
        }
        if (spec.spec.empty() == false) {
            const string name = "spec/" + spec.spec;
            if ((mtrlVec.back().spec = res::tex2D().get(name)) == nullptr) {
                mtrlVec.back().spec = res::tex2D().add(name);
                mtrlVec.back().spec->create(gl::RGB, gl::RGB8, 3u, true);
                mtrlVec.back().spec->set_preset(preset);
                mtrlVec.back().spec->buffer_auto(name);
            } mtrlVec.back().glDNS.z = true;
        }
    }
}

void Skin::bind_textures(uint _mtrl) const {
    const Material& mtrl = mtrlVec[_mtrl];
    if (mtrl.diff != nullptr) mtrl.diff->bind(gl::TEXTURE0);
    if (mtrl.norm != nullptr) mtrl.norm->bind(gl::TEXTURE1);
    if (mtrl.spec != nullptr) mtrl.spec->bind(gl::TEXTURE2);
}

void Skin::bind_textures(uint _mtrl, bool _diff, bool _norm, bool _spec) const {
    const Material& mtrl = mtrlVec[_mtrl];
    if (_diff) mtrl.diff->bind(gl::TEXTURE0);
    if (_norm) mtrl.norm->bind(gl::TEXTURE1);
    if (_spec) mtrl.spec->bind(gl::TEXTURE2);
}


ResHolder<Skin>& sq::res::skin() {
    static ResHolder<Skin> holder;
    return holder;
}
