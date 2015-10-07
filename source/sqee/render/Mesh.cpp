#include <cstring>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/redist/tinyformat.hpp>
#include <sqee/gl/FixedBuffer.hpp>
#include <sqee/gl/VertexArray.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

template<typename... Args>
void throw_error(const string& _path, int _lnum, const string& _msg, const Args&... _args) {
    string message = tfm::format("Parsing SQM \"%s\"\nline %d: ", _path, _lnum);
    throw runtime_error(message + tfm::format(_msg.c_str(), _args...));
}

Mesh::~Mesh() = default;

Mesh::Mesh(const string& _path) {
    create(_path);
}

void Mesh::create(const string& _path) {
    string path = res::path() + "meshes/" + _path + ".sqm";
    if (get_file_first_byte(path) == '#') load_ascii(path);
    else load_binary(path);
}

void Mesh::bind_vao() const {
    vertArr->bind();
}

void Mesh::draw_complete() const {
    gl::DrawElements(gl::TRIANGLES, elementTotal, gl::UNSIGNED_INT, nullptr);
}

void Mesh::draw_material(uint _mtrl) const {
    size_t start = mtrlVec[_mtrl].first; uint count = mtrlVec[_mtrl].second;
    gl::DrawElements(gl::TRIANGLES, count, gl::UNSIGNED_INT, (void*)start);
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
            if (key == "sphere") origin = svtofv3(line, 1), radius = stof(line[4]);
            else if (key == "bbsize") bbsize = svtofv3(line, 1);
            else if (key == "vertCount") vertCount = stou(line[1]);
            else if (key == "faceCount") faceCount = stou(line[1]);
            else if (key == "mtrlCount") mtrlCount = stou(line[1]), fData.resize(mtrlCount);
            else if (key == "hasNorm") hasNorm = true;
            else if (key == "hasTang") hasTang = true;
            else if (key == "hasTcrd") hasTcrd = true;
            else if (key == "hasColr") hasColr = true;
            else if (key == "hasBone") hasBone = true;
            else if (key == "hasMtrl") hasMtrl = true;
            else throw_error(path, lnum, "Invalid property \"%s\"", key);
        }

        else if (section == "vertices") {
            uint n = 0u;
            vData.points.emplace_back(svtofv3(line, n)); n+=3u;
            if (hasNorm == true) {
                vData.normals.emplace_back(svtofv3(line, n)); n+=3u;
            }
            if (hasTang == true) {
                vData.tangents.emplace_back(svtofv4(line, n)); n+=4u;
            }
            if (hasTcrd == true) {
                vData.texcrds.emplace_back(svtofv2(line, n)); n+=2u;
            }
            if (hasColr == true) {
                vData.colours.emplace_back(svtofv3(line, n)); n+=3u;
            }
            if (hasBone == true) {
                vData.bonesA.emplace_back(svtoiv4(line, n)); n+=4u;
                vData.bonesB.emplace_back(svtoiv4(line, n)); n+=4u;
                vData.weightsA.emplace_back(svtofv4(line, n)); n+=4u;
                vData.weightsB.emplace_back(svtofv4(line, n)); n+=4u;
            }
        }

        else if (section == "faces") {
            if (hasMtrl == true) {
                fData[stou(line[0])].emplace_back(svtouv3(line, 1));
            } else fData.front().emplace_back(svtouv3(line, 0));
        }

        else throw_error(path, lnum, "Invalid section \"%s\"", section);
    }

    load_final(vData, fData);
}

void Mesh::load_binary(const string& _path) {
//    vector<char> data = get_bytes_from_file(_path);
//    char* ptr = data.data();

//    vertCount = *(uint*)ptr; ptr += 4;
//    faceCount = *(uint*)ptr; ptr += 4;
//    mtrlCount = *(uint*)ptr; ptr += 4;
//    hasNorm = bool(*ptr); ptr += 1;
//    hasTcrd = bool(*ptr); ptr += 1;
//    hasBone = bool(*ptr); ptr += 1;
//    hasMtrl = bool(*ptr); ptr += 1;

//    VertData vdat;
//    FaceData fdat(mtrlCount);

//    float* fPtr = (float*)ptr;
//    for (uint i = 0; i < vertCount; i++) {
//        points.emplace_back(fPtr[0], fPtr[1], fPtr[2]); fPtr += 3;
//        if (hasNorm == true) {
//            normals.emplace_back(fPtr[0], fPtr[1], fPtr[2]); fPtr += 3;
//            tangents.emplace_back(make_tangent(normals.back()));
//        }
//        if (hasTcrd == true) {
//            texcrds.emplace_back(fPtr[0], fPtr[1]); fPtr += 2;
//        }
//        if (hasBone == true) {
//            int* iPtr = (int*)fPtr; fPtr += 8;
//            bonesA.emplace_back(iPtr[0], iPtr[1], iPtr[2], iPtr[3]);
//            bonesB.emplace_back(iPtr[4], iPtr[5], iPtr[6], iPtr[7]);
//            weightsA.emplace_back(fPtr[0], fPtr[1], fPtr[2], fPtr[3]); fPtr += 4;
//            weightsB.emplace_back(fPtr[0], fPtr[1], fPtr[2], fPtr[3]); fPtr += 4;
//        }
//    }

//    uint* uPtr = (uint*)fPtr;
//    for (uint i = 0; i < faceCount; i++) {
//        uint mtrlInd = 0u; if (hasMtrl) { mtrlInd = *uPtr; uPtr++; }
//        faceVec[mtrlInd].emplace_back(uPtr[0], uPtr[1], uPtr[2]); uPtr += 3;
//    }

//    load_final(points, normals, tangents, texcrds, bonesA, bonesB, weightsA, weightsB, faceVec);
}

void Mesh::load_final(const VertData& _v, const FaceData& _f) {
    vertArr.reset(new VertexArray());
    vertBuf.reset(new FixedBuffer(gl::ARRAY_BUFFER));
    elemBuf.reset(new FixedBuffer(gl::ELEMENT_ARRAY_BUFFER));
    vertArr->set_element_buffer(*elemBuf);

    size_t ptOffs = 0u;
    size_t nmOffs = vertCount * 12u;
    size_t tnOffs = vertCount * 24u;
    size_t tcOffs = ptOffs + hasNorm * vertCount * 40u;
    size_t clOffs = tcOffs + hasTcrd * vertCount * 8u;
    size_t bwOffs = clOffs + hasColr * vertCount * 12u;

    size_t vSize = 12 + hasNorm*28 + hasTcrd*8 + hasColr*12 + hasBone*64;

    vertBuf->allocate_editable(vertCount*vSize, nullptr);
    vertBuf->update(ptOffs, vertCount * 12u, _v.points.data());
    vertArr->add_attribute(*vertBuf, 0u, ptOffs, 12u, 3u, gl::FLOAT, false);

    if (hasNorm == true) {
        vertBuf->update(nmOffs, vertCount * 12u, _v.normals.data());
        vertBuf->update(tnOffs, vertCount * 16u, _v.tangents.data());
        vertArr->add_attribute(*vertBuf, 1u, nmOffs, 12u, 3u, gl::FLOAT, false);
        vertArr->add_attribute(*vertBuf, 2u, tnOffs, 16u, 4u, gl::FLOAT, false);
    }

    if (hasTcrd == true) {
        vertBuf->update(tcOffs, vertCount * 8u, _v.texcrds.data());
        vertArr->add_attribute(*vertBuf, 3u, tcOffs, 8u, 2u, gl::FLOAT, false);
    }

    if (hasColr == true) {
        vertBuf->update(clOffs, vertCount * 12u, _v.colours.data());
        vertArr->add_attribute(*vertBuf, 4u, clOffs, 12u, 3u, gl::FLOAT, false);
    }

    if (hasBone == true) {
        uchar* data = new uchar[vertCount*64];
        for (uint i = 0u; i < vertCount; ++i) {
            std::memcpy(data + i * 64 + 00, &_v.bonesA.at(i), 16u);
            std::memcpy(data + i * 64 + 16, &_v.bonesB.at(i), 16u);
            std::memcpy(data + i * 64 + 32, &_v.weightsA.at(i), 16u);
            std::memcpy(data + i * 64 + 48, &_v.weightsB.at(i), 16u);
        } vertBuf->update(bwOffs, vertCount * 64u, data); delete[] data;
        vertArr->add_attribute_I(*vertBuf, 5u, bwOffs + 00u, 64u, 4u, gl::INT);
        vertArr->add_attribute_I(*vertBuf, 6u, bwOffs + 16u, 64u, 4u, gl::INT);
        vertArr->add_attribute(*vertBuf, 7u, bwOffs + 32u, 64u, 4u, gl::FLOAT, false);
        vertArr->add_attribute(*vertBuf, 8u, bwOffs + 48u, 64u, 4u, gl::FLOAT, false);
    }

    vector<uvec3> faceData; uint start = 0u;
    for (uint i = 0u; i < mtrlCount; ++i) {
        mtrlVec.emplace_back(start, _f[i].size() * 3u);
        start += mtrlVec.back().second * 4u;
        faceData.insert(faceData.end(), _f[i].begin(), _f[i].end());
    } elemBuf->allocate_constant(faceData.size() * 12u, faceData.data());
    elementTotal = faceData.size() * 3u;
}

ResHolder<Mesh>& sq::res::mesh() {
    static ResHolder<Mesh> holder;
    return holder;
}
