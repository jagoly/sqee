#include <iostream>
#include <fstream>

#include <sqee/misc/files.hpp>

#include "model.hpp"

using namespace sqt::ent;

void Model::load(const string& _meshPath, const string& _skinPath, const string& _animPath) {
    if (!(mesh = resBank().meshH.get(_meshPath))) {
        mesh = resBank().meshH.add(_meshPath);
        mesh->load(_meshPath);
    }

    string skinPath = _skinPath + ".sq_skin";

    std::ifstream src;
    src.open(skinPath);

#ifdef SQEE_DEBUG
    if (!src.is_open())
        std::cout << "ERROR: Couldn't open file \"" << skinPath << "\"" << std::endl;
#endif

    string line;
    string section = "";

    uint gCount, pCount;
    bool norm = false, diff = false, spec = false;
    glm::uvec3 nSize, dSize, sSize;
    vector<string> normVec, diffVec, specVec;
    string skinID;
    int pNum = 0;
    while (std::getline(src, line)) {
        bool blank = true;
        for (const char* c = line.c_str(); *c && blank; c++) {
            blank = (*c == ' ');
        } if (blank) continue;
        char c = line[0];
        if (c == '#') continue;
        if (c == '{') {
            if (!section.empty()) throw; // already in a section
            section = SubStr(line.substr(1)).str; continue;
        }
        if (c == '}') {
            if (section.empty()) throw; // not in a section
            section = ""; continue;
        }

        if (section == "header") {
            SubStr key(line);
            if (key.str == "skinID") { skinID = SubStr(line, key).str; continue; }
            if (key.str == "gCount") { gCount = SubStr(line, key); continue; }
            if (key.str == "pCount") { pCount = SubStr(line, key); continue; }
            if (key.str == "norm") {
                SubStr sub = SubStr(line, key); nSize.x = int(sub);
                sub = SubStr(line, sub); nSize.y = int(sub);
                norm = true; continue;
            }
            if (key.str == "diff") {
                SubStr sub = SubStr(line, key); dSize.x = int(sub);
                sub = SubStr(line, sub); dSize.y = int(sub);
                diff = true; continue;
            }
            if (key.str == "spec") {
                SubStr sub = SubStr(line, key); sSize.x = int(sub);
                sub = SubStr(line, sub); sSize.y = int(sub);
                spec = true; continue;
            }
            throw; // invalid key
        }

        if (section == "poses") {
            if (line == "pose") { pNum++; continue; }

            SubStr key(line);
            SubStr sub = key;

            if (key.str == "norm")
                for (uint i = 0; i < gCount; i++) {
                    sub = SubStr(line, sub);
                    normVec.emplace_back(sub.str);
                }

            if (key.str == "diff")
                for (uint i = 0; i < gCount; i++) {
                    sub = SubStr(line, sub);
                    diffVec.emplace_back(sub.str);
                }

            if (key.str == "spec")
                for (uint i = 0; i < gCount; i++) {
                    sub = SubStr(line, sub);
                    specVec.emplace_back(sub.str);
                }

            continue;
        }

        if (section == "timeline") {
            // todo
            continue;
        }
    }

#ifdef SQEE_DEBUG
    if (!norm && normVec.size())
        std::cout << "WARNING: Got unexpected Normal textures when loading skin" << std::endl;
    if (!diff && diffVec.size())
        std::cout << "WARNING: Got unexpected Diffuse textures when loading skin" << std::endl;
    if (!spec && specVec.size())
        std::cout << "WARNING: Got unexpected Specular textures when loading skin" << std::endl;
    // need to add more
#endif

    bool needLoad = (!(texNorm = resBank().texH.get(skinID+"/norm")) && norm)
                 || (!(texDiff = resBank().texH.get(skinID+"/diff")) && diff)
                 || (!(texSpec = resBank().texH.get(skinID+"/spec")) && spec);

    nSize.z = dSize.z = sSize.z = gCount;

    if (needLoad) {
        if (norm) {
            texNorm = resBank().texH.add(skinID+"/norm");
            texNorm->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_C);
            texNorm->resize(nSize);
        } else texNorm = &resBank().tex2DA_8080FF;

        if (diff) {
            texDiff = resBank().texH.add(skinID+"/diff");
            texDiff->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_C);
            texDiff->resize(dSize);
        } else texDiff = &resBank().tex2DA_FFFFFF;

        if (spec) {
            texSpec = resBank().texH.add(skinID+"/spec");
            texSpec->create(gl::TEXTURE_2D_ARRAY, gl::RGB, gl::RGB8, sq::TexPreset::L_C);
            texSpec->resize(sSize);
        } else texSpec = &resBank().tex2DA_000000;
    }

    if (needLoad) {
        for (uint i = 0; i < gCount; i++) {
            if (norm) texNorm->buffer_file("models/norm/"+normVec[i], i);
            if (diff) texDiff->buffer_file("models/diff/"+diffVec[i], i);
            if (spec) texSpec->buffer_file("models/spec/"+specVec[i], i);
        }
    }


    sq::Animation* anim;

    if (!(anim = resBank().animH.get(_animPath))) {
        anim = resBank().animH.add(_animPath);
        anim->load(_animPath);
    }

    skeleton.setup(anim);
}

void Model::bind_textures() {
    texNorm->bind(gl::TEXTURE0);
    texDiff->bind(gl::TEXTURE1);
    texSpec->bind(gl::TEXTURE2);
}
