#include <cstring>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/redist/tinyformat.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/misc/Files.hpp>

using namespace sq;

template<typename... Args>
void throw_error(const string& _path, int _lnum, const string& _msg, const Args&... _args) {
    string message = tfm::format("Parsing SQM \"%s\"\nline %d: ", _path, _lnum);
    throw runtime_error(message + tfm::format(_msg.c_str(), _args...));
}


Mesh::~Mesh() {
    for (auto& iboPair : iboVec)
        gl::DeleteBuffers(1, &iboPair.first);
    gl::DeleteVertexArrays(1, &vao);
    gl::DeleteBuffers(1, &vbo);
}

Mesh::Mesh(const string& _path) {
    create(_path);
}

void Mesh::create(const string& _path) {
    string path = res::path() + "meshes/" + _path + ".sqm";
    if (get_file_first_byte(path) == '#') load_ascii(path);
    else load_binary(path);
}

void Mesh::bind_vao() const {
    gl::BindVertexArray(vao);
}

void Mesh::draw_ibo(uint _mtrl) const {
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, iboVec[_mtrl].first);
    gl::DrawElements(gl::TRIANGLES, iboVec[_mtrl].second, gl::UNSIGNED_INT, 0);
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
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);

    size_t ptOffs = 0u;
    size_t nmOffs = vertCount * 12u;
    size_t tnOffs = vertCount * 24u;
    size_t tcOffs = ptOffs + hasNorm*vertCount*40u;
    size_t clOffs = tcOffs + hasTcrd*vertCount*8u;
    size_t bwOffs = clOffs + hasColr*vertCount*12u;

    gl::GenBuffers(1, &vbo); gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
    size_t vSize = 12 + hasNorm*28 + hasTcrd*8 + hasColr*12 + hasBone*64;
    gl::BufferData(gl::ARRAY_BUFFER, vertCount*vSize, nullptr, gl::STATIC_DRAW);

    gl::BufferSubData(gl::ARRAY_BUFFER, ptOffs, vertCount*12, _v.points.data());
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, (void*)ptOffs);
    gl::EnableVertexAttribArray(0u);

    if (hasNorm == true) {
        gl::BufferSubData(gl::ARRAY_BUFFER, nmOffs, vertCount*12, _v.normals.data());
        gl::BufferSubData(gl::ARRAY_BUFFER, tnOffs, vertCount*16, _v.tangents.data());
        gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, (void*)nmOffs);
        gl::VertexAttribPointer(2, 4, gl::FLOAT, false, 0, (void*)tnOffs);
        gl::EnableVertexAttribArray(1u); gl::EnableVertexAttribArray(2u);
    }

    if (hasTcrd == true) {
        gl::BufferSubData(gl::ARRAY_BUFFER, tcOffs, vertCount*8, _v.texcrds.data());
        gl::VertexAttribPointer(3, 2, gl::FLOAT, false, 0, (void*)tcOffs);
        gl::EnableVertexAttribArray(3u);
    }

    if (hasColr == true) {
        gl::BufferSubData(gl::ARRAY_BUFFER, clOffs, vertCount*12, _v.colours.data());
        gl::VertexAttribPointer(4, 3, gl::FLOAT, false, 0, (void*)clOffs);
        gl::EnableVertexAttribArray(4u);
    }

    if (hasBone == true) {
        uchar* data = new uchar[vertCount*64];
        for (uint i = 0u; i < vertCount; i++) {
            std::memcpy(data + i * 64 + 00, &_v.bonesA.at(i), 16u);
            std::memcpy(data + i * 64 + 16, &_v.bonesB.at(i), 16u);
            std::memcpy(data + i * 64 + 32, &_v.weightsA.at(i), 16u);
            std::memcpy(data + i * 64 + 48, &_v.weightsB.at(i), 16u);
        } gl::BufferSubData(gl::ARRAY_BUFFER, bwOffs, vertCount*64, data);
        delete[] data;

        gl::VertexAttribIPointer(5, 4, gl::INT, 64, (void*)(bwOffs+00));
        gl::VertexAttribIPointer(6, 4, gl::INT, 64, (void*)(bwOffs+16));
        gl::VertexAttribPointer(7, 4, gl::FLOAT, false, 64, (void*)(bwOffs+32));
        gl::VertexAttribPointer(8, 4, gl::FLOAT, false, 64, (void*)(bwOffs+48));
        gl::EnableVertexAttribArray(5u); gl::EnableVertexAttribArray(6u);
        gl::EnableVertexAttribArray(7u); gl::EnableVertexAttribArray(8u);
    }

    for (uint i = 0u; i < mtrlCount; i++) {
        iboVec.emplace_back(0u, _f[i].size()*3u);
        auto& ibo = iboVec.back();
        gl::GenBuffers(1, &ibo.first);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo.first);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, ibo.second*4, _f[i].data(), gl::STATIC_DRAW);
    }
}

ResHolder<Mesh>& sq::res::mesh() {
    static ResHolder<Mesh> holder;
    return holder;
}
