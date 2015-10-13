#include <unordered_set>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/redist/tinyformat.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

template<typename... Args>
void throw_error(const string& _path, int _lnum, const string& _msg, const Args&... _args) {
    string message = tfm::format("Parsing SQS \"%s\"\nline %d: ", _path, _lnum);
    throw runtime_error(message + tfm::format(_msg.c_str(), _args...));
}

Skin::Skin(const string& _path) {
    create(_path);
}

void Skin::create(const string& _path) {
    string path = res::path() + "skins/" + _path + ".sqs";
    const auto fileVec = tokenise_file(path);

    struct MtrlSpec {
        int wrapMode = -1;
        std::unordered_set<string> flagsSet;
        string norm, diff, spec;
    }; vector<MtrlSpec> specVec;

    string section = "";
    for (const auto& linePair : fileVec) {
        const vector<string>& line = linePair.first;
        const uint lnum = linePair.second;
        const string& key = line.front();

        if (key.front() == '#') continue;
        if (key == "{") { if (section.empty()) section = line[1];
            else throw_error(path, lnum, "Misplaced {"); continue; }
        if (key == "}") { if (!section.empty()) section.clear();
            else throw_error(path, lnum, "Misplaced }"); continue; }

        if (section == "materials") {
            if (key == "material") specVec.emplace_back();
            else if (key == "wrap") {
                if      (line[1] == "clamp") specVec.back().wrapMode = 0;
                else if (line[1] == "repeat") specVec.back().wrapMode = 1;
                else if (line[1] == "mirror") specVec.back().wrapMode = 2;
                else throw_error(path, lnum, "Invalid wrap mode \"%s\"", line[1]);
            }
            else if (key == "diff") specVec.back().diff = line[1];
            else if (key == "norm") specVec.back().norm = line[1];
            else if (key == "spec") specVec.back().spec = line[1];
            else if (key == "flags")
                for (uint i = 1u; i < line.size(); i++)
                    specVec.back().flagsSet.emplace(line[i]);
            else throw_error(path, lnum, "Invalid property \"%s\"", key);
        }

        else throw_error(path, lnum, "Invalid section \"%s\"", section);
    }

    for (const MtrlSpec& spec : specVec) {
        mtrlVec.emplace_back();

        mtrlVec.back().punch = spec.flagsSet.count("punch");

        if (mtrlVec.back().punch) hasPunchThrough = true;

        Texture::Preset preset;
        if      (spec.wrapMode == 0) preset = Texture::MipmapClamp();
        else if (spec.wrapMode == 1) preset = Texture::MipmapRepeat();
        else if (spec.wrapMode == 2) preset = Texture::MipmapMirror();

        if (spec.diff.empty() == false) {
            const string name = "diff/" + spec.diff;
            if ((mtrlVec.back().diff = res::tex2D().get(name)) == nullptr) {
                mtrlVec.back().diff = res::tex2D().add(name, gl::RGBA, gl::RGBA8, preset);
                mtrlVec.back().diff->buffer_auto(name, true);
            } mtrlVec.back().glDNS.x = true;
        }
        if (spec.norm.empty() == false) {
            const string name = "norm/" + spec.norm;
            if ((mtrlVec.back().norm = res::tex2D().get(name)) == nullptr) {
                mtrlVec.back().norm = res::tex2D().add(name, gl::RGB, gl::RGB8, preset);
                mtrlVec.back().norm->buffer_auto(name, true);
            } mtrlVec.back().glDNS.y = true;
        }
        if (spec.spec.empty() == false) {
            const string name = "spec/" + spec.spec;
            if ((mtrlVec.back().spec = res::tex2D().get(name)) == nullptr) {
                mtrlVec.back().spec = res::tex2D().add(name, gl::RGB, gl::RGB8, preset);
                mtrlVec.back().spec->buffer_auto(name, true);
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
    if (_diff && mtrl.diff != nullptr) mtrl.diff->bind(gl::TEXTURE0);
    if (_norm && mtrl.norm != nullptr) mtrl.norm->bind(gl::TEXTURE1);
    if (_spec && mtrl.spec != nullptr) mtrl.spec->bind(gl::TEXTURE2);
}


ResHolder<Skin>& sq::res::skin() {
    static ResHolder<Skin> holder;
    return holder;
}
