#include <cstring>
#include <stdexcept>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/redist/tinyformat.hpp>
#include <sqee/app/Resources.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/misc/Files.hpp>
#include <sqee/render/Mesh.hpp>

using namespace sq;

template<typename... Args>
void throw_error(const string& _path, int _lnum, const string& _msg, const Args&... _args) {
    string message = tfm::format("Parsing SQM \"%s\"\nline %d: ", _path, _lnum);
    throw std::runtime_error(message + tfm::format(_msg.c_str(), _args...));
}


Mesh::Mesh(const string& _path) :
    vertBuf(gl::ARRAY_BUFFER), elemBuf(gl::ELEMENT_ARRAY_BUFFER) {
    vertArr.set_element_buffer(elemBuf);

    string path = static_path() + "meshes/" + _path + ".sqm";
    if (get_file_first_char(path) == '#') load_ascii(path);
    //else load_binary(path);
}

void Mesh::draw_complete() const {
    gl::DrawElements(gl::TRIANGLES, elementTotal, gl::UNSIGNED_INT, nullptr);
}

void Mesh::draw_material(uint _index) const {
    size_t start = mtrlVec[_index].first; uint count = mtrlVec[_index].second;
    gl::DrawElements(gl::TRIANGLES, count, gl::UNSIGNED_INT, (void*)start);
}

BoundBox Mesh::create_BoundBox(const Mat4F& _modelMat) const {
    return make_BoundBox(_modelMat, origin, radius, bbsize);
}

void Mesh::load_ascii(const string& _path) {
    const auto fileVec = tokenise_file(_path);
    const string& path = _path;

    VertData vData;
    FaceData fData;

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

        if (section == "header") {
            if      (key == "radius") { radius = stof(line[1]); }
            else if (key == "origin") { origin = svtofv3(line, 1); }
            else if (key == "sphere") origin = svtofv3(line, 1), radius = stof(line[4]);
            else if (key == "bbsize") { bbsize = svtofv3(line, 1); }
            else if (key == "vertCount") { vertCount = stou(line[1]); }
            else if (key == "faceCount") { faceCount = stou(line[1]); }
            else if (key == "mtrlCount") { mtrlCount = stou(line[1]); fData.resize(mtrlCount); }
            else if (key == "hasNorm") { optionsBits = optionsBits | 0b10000; }
            else if (key == "hasTang") { optionsBits = optionsBits | 0b01000; }
            else if (key == "hasTcrd") { optionsBits = optionsBits | 0b00100; }
            else if (key == "hasColr") { optionsBits = optionsBits | 0b00010; }
            else if (key == "hasBone") { optionsBits = optionsBits | 0b00001; }
            else throw_error(path, lnum, "Invalid property \"%s\"", key);
        }

        else if (section == "vertices") {
            uint n = 0u;
            vData.points.emplace_back(svtofv3(line, n)); n+=3u;
            if (has_NORM() == true) {
                vData.normals.emplace_back(svtofv3(line, n)); n+=3u;
            }
            if (has_TANG() == true) {
                vData.tangents.emplace_back(svtofv4(line, n)); n+=4u;
            }
            if (has_TCRD() == true) {
                vData.texcrds.emplace_back(svtofv2(line, n)); n+=2u;
            }
            if (has_COLR() == true) {
                vData.colours.emplace_back(svtofv3(line, n)); n+=3u;
            }
            if (has_BONE() == true) {
                vData.bones.emplace_back(svtoiv4(line, n)); n+=4u;
                vData.weights.emplace_back(svtofv4(line, n)); n+=4u;
            }
        }

        else if (section == "faces") {
            if (mtrlCount > 1u) {
                fData[stou(line[0])].emplace_back(svtouv3(line, 1));
            } else fData.front().emplace_back(svtouv3(line, 0));
        }

        else throw_error(path, lnum, "Invalid section \"%s\"", section);
    }

    load_final(vData, fData);
}


void Mesh::load_final(const VertData& _v, const FaceData& _f) {
    size_t ptOffs = 0u;
    size_t nmOffs = vertCount * 12u;
    size_t tnOffs = vertCount * 24u;
    size_t tcOffs = ptOffs + has_NORM() * vertCount * 40u;
    size_t clOffs = tcOffs + has_TCRD() * vertCount * 8u;
    size_t bwOffs = clOffs + has_COLR() * vertCount * 12u;

    size_t vSize = 12 + has_NORM()*28 + has_TCRD()*8 + has_COLR()*12 + has_BONE()*32;

    vertBuf.allocate_editable(vertCount*vSize, nullptr);
    vertBuf.update(ptOffs, vertCount * 12u, _v.points.data());
    vertArr.add_attribute(vertBuf, 0u, ptOffs, 12u, 3u, gl::FLOAT, false);

    if (has_NORM() == true) {
        vertBuf.update(nmOffs, vertCount * 12u, _v.normals.data());
        vertBuf.update(tnOffs, vertCount * 16u, _v.tangents.data());
        vertArr.add_attribute(vertBuf, 1u, nmOffs, 12u, 3u, gl::FLOAT, false);
        vertArr.add_attribute(vertBuf, 2u, tnOffs, 16u, 4u, gl::FLOAT, false);
    }

    if (has_TCRD() == true) {
        vertBuf.update(tcOffs, vertCount * 8u, _v.texcrds.data());
        vertArr.add_attribute(vertBuf, 3u, tcOffs, 8u, 2u, gl::FLOAT, false);
    }

    if (has_COLR() == true) {
        vertBuf.update(clOffs, vertCount * 12u, _v.colours.data());
        vertArr.add_attribute(vertBuf, 4u, clOffs, 12u, 3u, gl::FLOAT, false);
    }

    if (has_BONE() == true) {
        uchar* data = new uchar[vertCount*32];
        for (uint i = 0u; i < vertCount; ++i) {
            std::memcpy(data + i * 32 + 00, &_v.bones.at(i), 16u);
            std::memcpy(data + i * 32 + 16, &_v.weights.at(i), 16u);
        } vertBuf.update(bwOffs, vertCount * 32u, data); delete[] data;
        vertArr.add_attribute_I(vertBuf, 5u, bwOffs + 00u, 32u, 4u, gl::INT);
        vertArr.add_attribute(vertBuf, 6u, bwOffs + 16u, 32u, 4u, gl::FLOAT, false);
    }

    vector<Vec3U> faceData; uint start = 0u;
    for (uint i = 0u; i < mtrlCount; ++i) {
        mtrlVec.emplace_back(start, _f[i].size() * 3u);
        start += mtrlVec.back().second * 4u;
        faceData.insert(faceData.end(), _f[i].begin(), _f[i].end());
    } elemBuf.allocate_constant(faceData.size() * 12u, faceData.data());
    elementTotal = faceData.size() * 3u;
}
