#include "app/logging.hpp"
#include "gl/gl_ext_3_3.hpp"
#include "gl/maths.hpp"
#include "misc/files.hpp"
#include "misc/strtonum.hpp"

#include "models/mesh.hpp"

using namespace sq;

void Mesh::load(const string& _filePath) {
    mtrlVec.resize(1);
    uint countedFaces = 0;

    string filePath = "res/models/meshes/" + _filePath + ".sqm";
    std::vector<std::vector<string>> fileVec(sq::get_words_from_file(filePath));

    string section = "";
    for (const std::vector<string>& line : fileVec) {
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
            if      (key == "vCount") vCount = stoi(line[1]);
            else if (key == "fCount") fCount = stoi(line[1]);
            else if (key == "mCount") mtrlVec.resize(stoi(line[1]));
            else if (key == "hasNM") hasNM = true;
            else if (key == "hasUV") hasUV = true;
            else if (key == "hasBW") hasBW = true;
            else if (key == "hasMT") hasMT = true;
            else throw; // invalid key
            continue;
        }

        if (section == "vertices") {
            int ind = 0;
            Vertex vert;

            vert.x = stof(line[ind++]),
            vert.y = stof(line[ind++]),
            vert.z = stof(line[ind++]);

            if (hasNM) vert.nx = stof(line[ind++]),
                       vert.ny = stof(line[ind++]),
                       vert.nz = stof(line[ind++]);

            if (hasUV) vert.u = stof(line[ind++]),
                       vert.v = stof(line[ind++]);

            if (hasBW) vert.b[0] = stoi(line[ind++]), vert.b[1] = stoi(line[ind++]),
                       vert.b[2] = stoi(line[ind++]), vert.b[3] = stoi(line[ind++]),
                       vert.b[4] = stoi(line[ind++]), vert.b[5] = stoi(line[ind++]),
                       vert.b[6] = stoi(line[ind++]), vert.b[7] = stoi(line[ind++]),
                       vert.w[0] = stof(line[ind++]), vert.w[1] = stof(line[ind++]),
                       vert.w[2] = stof(line[ind++]), vert.w[3] = stof(line[ind++]),
                       vert.w[4] = stof(line[ind++]), vert.w[5] = stof(line[ind++]),
                       vert.w[6] = stof(line[ind++]), vert.w[7] = stof(line[ind++]);

            vertVec.push_back(vert);
            continue;
        }

        if (section == "faces") {
            countedFaces++;
            std::array<uint, 3> face = {stou(line[0]), stou(line[1]), stou(line[2])};
            mtrlVec[hasMT ? stoi(line[3]) : 0].push_back(face);
            continue;
        }
    }

    #ifdef SQEE_DEBUG
    if (vCount != vertVec.size())
        log_warning("vCount mismatch when loading mesh from $0", filePath);
    if (fCount != countedFaces)
        log_warning("fCount mismatch when loading mesh from $0", filePath);
    #endif

    for (uint i = 0; i < mtrlVec.size(); i++) {
        std::vector<std::array<uint, 3>>& faceVec = mtrlVec[i];
        iboVec.emplace_back(0, faceVec.size() * 3);
        std::pair<GLuint, uint>& ibo = iboVec.back();
        gl::GenBuffers(1, &ibo.first);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo.first);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, ibo.second * sizeof(int),
                       faceVec.data(), gl::STATIC_DRAW);
    }

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    if (hasNM) {
        gl::EnableVertexAttribArray(1);
        gl::EnableVertexAttribArray(2);
    }
    if (hasUV) {
        gl::EnableVertexAttribArray(3);
    }
    if (hasBW) {
        gl::EnableVertexAttribArray(4);
        gl::EnableVertexAttribArray(5);
        gl::EnableVertexAttribArray(6);
        gl::EnableVertexAttribArray(7);
    }

    float points[vCount * 3];
    float normals[vCount * 3];
    float tangents[vCount * 3];
    float texcoords[vCount * 2];
    int bones[vCount * 8];
    float weights[vCount * 8];

    for (uint i = 0; i < vCount; i++) {
        const Vertex& vert = vertVec[i];
        points[i*3 +0] = vert.x;
        points[i*3 +1] = vert.y;
        points[i*3 +2] = vert.z;
        if (hasNM) {
            normals[i*3 +0] = vert.nx;
            normals[i*3 +1] = vert.ny;
            normals[i*3 +2] = vert.nz;
            glm::vec3 norm(vert.nx, vert.ny, vert.nz);
            glm::vec3 t1(glm::cross(norm, {0, 1, 0})), t2(glm::cross(norm, {0, 0, 1}));
            glm::vec3 tangent = glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
            tangents[i*3 +0] = tangent.x;
            tangents[i*3 +1] = tangent.y;
            tangents[i*3 +2] = tangent.z;
        }
        if (hasUV) {
            texcoords[i*2 +0] = vert.u;
            texcoords[i*2 +1] = vert.v;
        }
        if (hasBW) {
            for (uint j = 0; j < 8; j++) {
                bones[i*8 +j] = vert.b[j];
                weights[i*8 +j] = vert.w[j];
            }
        }
    }

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(float), points, gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

    if (hasNM) {
        gl::GenBuffers(1, &vboN);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboN);
        gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(float), normals, gl::STATIC_DRAW);
        gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, nullptr);

        gl::GenBuffers(1, &vboT);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboT);
        gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(float), tangents, gl::STATIC_DRAW);
        gl::VertexAttribPointer(2, 3, gl::FLOAT, false, 0, nullptr);
    }

    if (hasUV) {
        gl::GenBuffers(1, &vboTc);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
        gl::BufferData(gl::ARRAY_BUFFER, 2 * vCount * sizeof(float), texcoords, gl::STATIC_DRAW);
        gl::VertexAttribPointer(3, 2, gl::FLOAT, false, 0, nullptr);
    }

    if (hasBW) {
        gl::GenBuffers(1, &vboB);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboB);
        gl::BufferData(gl::ARRAY_BUFFER, vCount * 8 * sizeof(int), bones, gl::STATIC_DRAW);
        gl::VertexAttribIPointer(4, 4, gl::INT, 8*sizeof(int), nullptr);
        gl::VertexAttribIPointer(5, 4, gl::INT, 8*sizeof(int), (void*)(4*sizeof(int)));

        gl::GenBuffers(1, &vboW);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboW);
        gl::BufferData(gl::ARRAY_BUFFER, vCount * 8 * sizeof(float), weights, gl::STATIC_DRAW);
        gl::VertexAttribPointer(6, 4, gl::FLOAT, false, 8*sizeof(float), nullptr);
        gl::VertexAttribPointer(7, 4, gl::FLOAT, false, 8*sizeof(float), (void*)(4*sizeof(float)));
    }
}

Mesh::~Mesh() {
    for (std::pair<GLuint, uint>& iboPair : iboVec) {
        gl::DeleteBuffers(1, &iboPair.first);
    }
    gl::DeleteVertexArrays(1, &vao);
    gl::DeleteBuffers(1, &vboP);
    gl::DeleteBuffers(1, &vboN);
    gl::DeleteBuffers(1, &vboT);
    if (hasUV) {
        gl::DeleteBuffers(1, &vboTc);
    }
    if (hasBW) {
        gl::DeleteBuffers(1, &vboB);
        gl::DeleteBuffers(1, &vboW);
    }
}

void Mesh::bind_vao() {
    gl::BindVertexArray(vao);
}

void Mesh::draw_ibo(uint _mtrl) {
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, iboVec[_mtrl].first);
    gl::DrawElements(gl::TRIANGLES, iboVec[_mtrl].second, gl::UNSIGNED_INT, 0);
}
