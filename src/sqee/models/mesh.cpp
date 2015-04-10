#include <glm/geometric.hpp>

#include "sqee/redist/gl_ext_3_3.hpp"
#include "sqee/gl/maths.hpp"
#include "sqee/app/logging.hpp"
#include "sqee/misc/files.hpp"
#include "sqee/models/mesh.hpp"

using namespace sq;

void Mesh::create(const string& _path) {
    string path = res::mesh_path() + _path + ".sqm";
    if (get_file_first_byte(path) == '#') load_ascii(path);
    else load_binary(path);
}

Mesh::~Mesh() {
    for (auto& iboPair : iboVec)
        gl::DeleteBuffers(1, &iboPair.first);
    gl::DeleteVertexArrays(1, &vao);

    gl::DeleteBuffers(1, &vboP);
    if (hasNM) {
        gl::DeleteBuffers(1, &vboN);
        gl::DeleteBuffers(1, &vboT);
    }
    if (hasUV) {
        gl::DeleteBuffers(1, &vboTc);
    }
    if (hasBW) {
        gl::DeleteBuffers(1, &vboBa);
        gl::DeleteBuffers(1, &vboBb);
        gl::DeleteBuffers(1, &vboWa);
        gl::DeleteBuffers(1, &vboWb);
    }
}

void Mesh::bind_vao() {
    gl::BindVertexArray(vao);
}

void Mesh::draw_ibo(uint _mtrl) {
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, iboVec[_mtrl].first);
    gl::DrawElements(gl::TRIANGLES, iboVec[_mtrl].second, gl::UNSIGNED_INT, 0);
}

void Mesh::load_ascii(const string& _path) {
    std::vector<std::vector<string>> fileVec(get_words_from_file(_path));

    std::vector<vec3> points;
    std::vector<vec3> normals;
    std::vector<vec3> tangents;
    std::vector<vec2> texcrds;
    std::vector<ivec4> bonesA;
    std::vector<ivec4> bonesB;
    std::vector<vec4> weightsA;
    std::vector<vec4> weightsB;
    std::vector<std::vector<uvec3>> faceVec;

    string section = "";
    for (const auto& line : fileVec) {
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
            if      (key == "sphere") sphere = {stof(line[1]), stof(line[2]), stof(line[3]), stof(line[4])};
            else if (key == "vCount") vCount = stou(line[1]);
            else if (key == "fCount") fCount = stou(line[1]);
            else if (key == "mCount") mCount = stou(line[1]), faceVec.resize(mCount);
            else if (key == "hasNM") hasNM = true;
            else if (key == "hasUV") hasUV = true;
            else if (key == "hasBW") hasBW = true;
            else if (key == "hasMT") hasMT = true;
            else throw;
            continue;
        }

        if (section == "vertices") {
            int n = 0;
            points.emplace_back(stof(line[n+0]), stof(line[n+1]), stof(line[n+2]));n+=3;
            if (hasNM) {
                normals.emplace_back(stof(line[n+0]), stof(line[n+1]), stof(line[n+2]));n+=3;
                tangents.emplace_back(make_tangent(normals.back()));
            }
            if (hasUV) {
                texcrds.emplace_back(stof(line[n+0]), stof(line[n+1]));n+=2;
            }
            if (hasBW) {
                bonesA.emplace_back(stoi(line[n+0]), stoi(line[n+1]), stoi(line[n+2]), stoi(line[n+3]));n+=4;
                bonesB.emplace_back(stoi(line[n+0]), stoi(line[n+1]), stoi(line[n+2]), stoi(line[n+3]));n+=4;
                weightsA.emplace_back(stof(line[n+0]), stof(line[n+1]), stof(line[n+2]), stof(line[n+3]));n+=4;
                weightsB.emplace_back(stof(line[n+0]), stof(line[n+1]), stof(line[n+2]), stof(line[n+3]));n+=4;
            }
            continue;
        }

        if (section == "faces") {
            if (hasMT) faceVec[stou(line[0])].emplace_back(stou(line[1]), stou(line[2]), stou(line[3]));
            else faceVec[0].emplace_back(stou(line[0]), stou(line[1]), stou(line[2]));
            continue;
        }
    }

    load_final(points, normals, tangents, texcrds, bonesA, bonesB, weightsA, weightsB, faceVec);
}

void Mesh::load_binary(const string& _path) {
    std::vector<char> data = get_bytes_from_file(_path);
    char* ptr = data.data();

    vCount = *(uint*)ptr; ptr += 4;
    fCount = *(uint*)ptr; ptr += 4;
    mCount = *(uint*)ptr; ptr += 4;
    hasNM = bool(*ptr); ptr += 1;
    hasUV = bool(*ptr); ptr += 1;
    hasBW = bool(*ptr); ptr += 1;
    hasMT = bool(*ptr); ptr += 1;

    std::vector<vec3> points;
    std::vector<vec3> normals;
    std::vector<vec3> tangents;
    std::vector<vec2> texcrds;
    std::vector<ivec4> bonesA;
    std::vector<ivec4> bonesB;
    std::vector<vec4> weightsA;
    std::vector<vec4> weightsB;
    std::vector<std::vector<uvec3>> faceVec(mCount);

    float* fPtr = (float*)ptr;
    for (uint i = 0; i < vCount; i++) {
        points.emplace_back(fPtr[0], fPtr[1], fPtr[2]); fPtr += 3;
        if (hasNM) {
            normals.emplace_back(fPtr[0], fPtr[1], fPtr[2]); fPtr += 3;
            tangents.emplace_back(make_tangent(normals.back()));
        }
        if (hasUV) {
            texcrds.emplace_back(fPtr[0], fPtr[1]); fPtr += 2;
        }
        if (hasBW) {
            int* iPtr = (int*)fPtr; fPtr += 8;
            bonesA.emplace_back(iPtr[0], iPtr[1], iPtr[2], iPtr[3]);
            bonesB.emplace_back(iPtr[4], iPtr[5], iPtr[6], iPtr[7]);
            weightsA.emplace_back(fPtr[0], fPtr[1], fPtr[2], fPtr[3]); fPtr += 4;
            weightsB.emplace_back(fPtr[0], fPtr[1], fPtr[2], fPtr[3]); fPtr += 4;
        }
    }

    uint* uPtr = (uint*)fPtr;
    for (uint i = 0; i < fCount; i++) {
        uint mtrlInd = 0; if (hasMT) { mtrlInd = *uPtr; uPtr++; }
        faceVec[mtrlInd].emplace_back(uPtr[0], uPtr[1], uPtr[2]); uPtr += 3;
    }

    load_final(points, normals, tangents, texcrds, bonesA, bonesB, weightsA, weightsB, faceVec);
}

void Mesh::load_final(const std::vector<vec3>& points, const std::vector<vec3>& normals,
                      const std::vector<vec3>& tangents, const std::vector<vec2>& texcrds,
                      const std::vector<ivec4>& bonesA, const std::vector<ivec4>& bonesB,
                      const std::vector<vec4>& weightsA, const std::vector<vec4>& weightsB,
                      const std::vector<std::vector<uvec3>>& faceVec) {
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, vCount*3*4, points.data(), gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);
    gl::EnableVertexAttribArray(0);

    if (hasNM) {
        gl::GenBuffers(1, &vboN);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboN);
        gl::BufferData(gl::ARRAY_BUFFER, vCount*3*4, normals.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(1);

        gl::GenBuffers(1, &vboT);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboT);
        gl::BufferData(gl::ARRAY_BUFFER, vCount*3*4, tangents.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(2, 3, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(2);
    }

    if (hasUV) {
        gl::GenBuffers(1, &vboTc);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
        gl::BufferData(gl::ARRAY_BUFFER, vCount*2*4, texcrds.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(3, 2, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(3);
    }

    if (hasBW) {
        gl::GenBuffers(1, &vboBa);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboBa);
        gl::BufferData(gl::ARRAY_BUFFER, vCount*4*4, bonesA.data(), gl::STATIC_DRAW);
        gl::VertexAttribIPointer(4, 4, gl::INT, 0, nullptr);
        gl::EnableVertexAttribArray(4);

        gl::GenBuffers(1, &vboBb);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboBb);
        gl::BufferData(gl::ARRAY_BUFFER, vCount*4*4, bonesB.data(), gl::STATIC_DRAW);
        gl::VertexAttribIPointer(5, 4, gl::INT, 0, nullptr);
        gl::EnableVertexAttribArray(5);

        gl::GenBuffers(1, &vboWa);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboWa);
        gl::BufferData(gl::ARRAY_BUFFER, vCount*4*4, weightsA.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(6, 4, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(6);

        gl::GenBuffers(1, &vboWb);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboWb);
        gl::BufferData(gl::ARRAY_BUFFER, vCount*4*4, weightsB.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(7, 4, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(7);
    }

    for (uint i = 0; i < mCount; i++) {
        iboVec.emplace_back(0, faceVec[i].size() * 3);
        auto& ibo = iboVec.back();
        gl::GenBuffers(1, &ibo.first);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo.first);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, ibo.second*4, faceVec[i].data(), gl::STATIC_DRAW);
    }
}

ResHolder<Mesh>& sq::res::mesh() {
    static ResHolder<Mesh> holder;
    return holder;
}
string& sq::res::mesh_path() {
    static string path;
    return path;
}
