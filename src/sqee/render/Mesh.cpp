#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/misc/Files.hpp>

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
    if (hasNorm == true) {
        gl::DeleteBuffers(1, &vboN);
        gl::DeleteBuffers(1, &vboT);
    }
    if (hasTcrd == true) {
        gl::DeleteBuffers(1, &vboTc);
    }
    if (hasBone == true) {
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
    vector<vector<string>> fileVec(get_words_from_file(_path));

    vector<fvec3> points;
    vector<fvec3> normals;
    vector<fvec3> tangents;
    vector<fvec2> texcrds;
    vector<ivec4> bonesA;
    vector<ivec4> bonesB;
    vector<fvec4> weightsA;
    vector<fvec4> weightsB;
    vector<vector<uvec3>> faceVec;

    string section = "";
    for (const auto& line : fileVec) {
        const string& key = line[0]; if (key[0] == '#') continue;
        if (key == "{") { if (!section.empty()) throw; section = line[1]; continue; }
        if (key == "}") { if (section.empty()) throw; section.clear(); continue; }

        if (section == "header") {
            if (key == "sphere") origin = svtofv3(line, 1), radius = stof(line[4]);
            else if (key == "bbsize") bbsize = svtofv3(line, 1);
            else if (key == "vertCount") vertCount = stou(line[1]);
            else if (key == "faceCount") faceCount = stou(line[1]);
            else if (key == "mtrlCount") mtrlCount = stou(line[1]), faceVec.resize(mtrlCount);
            else if (key == "hasNorm") hasNorm = true;
            else if (key == "hasTcrd") hasTcrd = true;
            else if (key == "hasBone") hasBone = true;
            else if (key == "hasMtrl") hasMtrl = true;
            else throw;
        }

        else if (section == "vertices") {
            uint n = 0u;
            points.emplace_back(svtofv3(line, n)); n+=3u;
            if (hasNorm == true) {
                normals.emplace_back(svtofv3(line, n)); n+=3u;
                tangents.emplace_back(make_tangent(normals.back()));
            }
            if (hasTcrd == true) {
                texcrds.emplace_back(svtofv2(line, n)); n+=2u;
            }
            if (hasBone == true) {
                bonesA.emplace_back(svtoiv4(line, n)); n+=4u;
                bonesB.emplace_back(svtoiv4(line, n)); n+=4u;
                weightsA.emplace_back(svtofv4(line, n)); n+=4u;
                weightsB.emplace_back(svtofv4(line, n)); n+=4u;
            }
        }

        else if (section == "faces") {
            if (hasMtrl == true) {
                faceVec[stou(line[0])].emplace_back(svtouv3(line, 1));
            } else faceVec[0].emplace_back(svtouv3(line, 0));
        }

        else throw;
    }

    load_final(points, normals, tangents, texcrds, bonesA, bonesB, weightsA, weightsB, faceVec);
}

void Mesh::load_binary(const string& _path) {
    vector<char> data = get_bytes_from_file(_path);
    char* ptr = data.data();

    vertCount = *(uint*)ptr; ptr += 4;
    faceCount = *(uint*)ptr; ptr += 4;
    mtrlCount = *(uint*)ptr; ptr += 4;
    hasNorm = bool(*ptr); ptr += 1;
    hasTcrd = bool(*ptr); ptr += 1;
    hasBone = bool(*ptr); ptr += 1;
    hasMtrl = bool(*ptr); ptr += 1;

    vector<fvec3> points;
    vector<fvec3> normals;
    vector<fvec3> tangents;
    vector<fvec2> texcrds;
    vector<ivec4> bonesA;
    vector<ivec4> bonesB;
    vector<fvec4> weightsA;
    vector<fvec4> weightsB;
    vector<vector<uvec3>> faceVec(mtrlCount);

    float* fPtr = (float*)ptr;
    for (uint i = 0; i < vertCount; i++) {
        points.emplace_back(fPtr[0], fPtr[1], fPtr[2]); fPtr += 3;
        if (hasNorm == true) {
            normals.emplace_back(fPtr[0], fPtr[1], fPtr[2]); fPtr += 3;
            tangents.emplace_back(make_tangent(normals.back()));
        }
        if (hasTcrd == true) {
            texcrds.emplace_back(fPtr[0], fPtr[1]); fPtr += 2;
        }
        if (hasBone == true) {
            int* iPtr = (int*)fPtr; fPtr += 8;
            bonesA.emplace_back(iPtr[0], iPtr[1], iPtr[2], iPtr[3]);
            bonesB.emplace_back(iPtr[4], iPtr[5], iPtr[6], iPtr[7]);
            weightsA.emplace_back(fPtr[0], fPtr[1], fPtr[2], fPtr[3]); fPtr += 4;
            weightsB.emplace_back(fPtr[0], fPtr[1], fPtr[2], fPtr[3]); fPtr += 4;
        }
    }

    uint* uPtr = (uint*)fPtr;
    for (uint i = 0; i < faceCount; i++) {
        uint mtrlInd = 0u; if (hasMtrl) { mtrlInd = *uPtr; uPtr++; }
        faceVec[mtrlInd].emplace_back(uPtr[0], uPtr[1], uPtr[2]); uPtr += 3;
    }

    load_final(points, normals, tangents, texcrds, bonesA, bonesB, weightsA, weightsB, faceVec);
}

void Mesh::load_final(const vector<fvec3>& points,   const vector<fvec3>& normals,
                      const vector<fvec3>& tangents, const vector<fvec2>& texcrds,
                      const vector<ivec4>& bonesA,   const vector<ivec4>& bonesB,
                      const vector<fvec4>& weightsA, const vector<fvec4>& weightsB,
                      const vector<vector<uvec3>>& faceVec) {
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, vertCount*3*4, points.data(), gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);
    gl::EnableVertexAttribArray(0);

    if (hasNorm == true) {
        gl::GenBuffers(1, &vboN);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboN);
        gl::BufferData(gl::ARRAY_BUFFER, vertCount*3*4, normals.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(1);

        gl::GenBuffers(1, &vboT);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboT);
        gl::BufferData(gl::ARRAY_BUFFER, vertCount*3*4, tangents.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(2, 3, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(2);
    }

    if (hasTcrd == true) {
        gl::GenBuffers(1, &vboTc);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
        gl::BufferData(gl::ARRAY_BUFFER, vertCount*2*4, texcrds.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(3, 2, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(3);
    }

    if (hasBone == true) {
        gl::GenBuffers(1, &vboBa);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboBa);
        gl::BufferData(gl::ARRAY_BUFFER, vertCount*4*4, bonesA.data(), gl::STATIC_DRAW);
        gl::VertexAttribIPointer(4, 4, gl::INT, 0, nullptr);
        gl::EnableVertexAttribArray(4);

        gl::GenBuffers(1, &vboBb);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboBb);
        gl::BufferData(gl::ARRAY_BUFFER, vertCount*4*4, bonesB.data(), gl::STATIC_DRAW);
        gl::VertexAttribIPointer(5, 4, gl::INT, 0, nullptr);
        gl::EnableVertexAttribArray(5);

        gl::GenBuffers(1, &vboWa);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboWa);
        gl::BufferData(gl::ARRAY_BUFFER, vertCount*4*4, weightsA.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(6, 4, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(6);

        gl::GenBuffers(1, &vboWb);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboWb);
        gl::BufferData(gl::ARRAY_BUFFER, vertCount*4*4, weightsB.data(), gl::STATIC_DRAW);
        gl::VertexAttribPointer(7, 4, gl::FLOAT, false, 0, nullptr);
        gl::EnableVertexAttribArray(7);
    }

    for (uint i = 0u; i < mtrlCount; i++) {
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
