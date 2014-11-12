#include <iostream>
#include <fstream>

#include <models/mesh.hpp>
#include <misc/files.hpp>

using namespace sq;

void Mesh::load(const string& _filePath) {
    string filePath = SQ_MODELS "meshes/" + _filePath + ".sqm";

    std::ifstream src;
    src.open(filePath);

#ifdef SQEE_DEBUG
    if (!src.is_open())
        std::cout << "ERROR: Couldn't open file \"" << filePath << "\"" << std::endl;
#endif

    string line;
    string section = "";

    uint vCount, fCount;
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
            if (key.str == "vCount") { vCount = int(SubStr(line, key)); continue; }
            if (key.str == "fCount") { fCount = int(SubStr(line, key)); continue; }
            if (key.str == "hasUV")  { hasUV  = (SubStr(line, key).str == "yes"); continue; }
            if (key.str == "hasBW")  { hasBW  = (SubStr(line, key).str == "yes"); continue; }
            if (key.str == "hasMID") { hasMID = (SubStr(line, key).str == "yes"); continue; }
            throw; // invalid key
        }

        if (section == "vertices") {
            SubStr x(line), y(line, x), z(line, y);
            SubStr nx(line, z), ny(line, nx), nz(line, ny);
            SubStr u(line, nz), v(line, u);
            SubStr b0(line, v),  b1(line, b0), b2(line, b1), b3(line, b2);
            SubStr b4(line, b3), b5(line, b4), b6(line, b5), b7(line, b6);
            SubStr w0(line, b7), w1(line, w0), w2(line, w1), w3(line, w2);
            SubStr w4(line, w3), w5(line, w4), w6(line, w5), w7(line, w6);

            vertVec.push_back({x, y, z, nx, ny, nz, u, v,
                              {int(b0), int(b1), int(b2), int(b3),
                               int(b4), int(b5), int(b6), int(b7)},
                              {w0, w1, w2, w3, w4, w5, w6, w7}});
            continue;
        }

        if (section == "faces") {
            SubStr a(line), b(line, a), c(line, b), m(line, c);
            faceVec.emplace_back(int(a), int(b), int(c), int(m));
            continue;
        }
    }

#ifdef SQEE_DEBUG
    if (vCount != vertVec.size())
        std::cout << "WARNING: vCount and actual vertex count do not match when loading mesh from \""
                  << filePath << "\"" << std::endl;
    if (fCount != faceVec.size())
        std::cout << "WARNING: fCount and actual face count do not match when loading mesh from \""
                  << filePath << "\"" << std::endl;
#endif


    iCount = fCount * 3;

    uint indices[iCount];
    uchar texNums[fCount];
    for (uint i = 0; i < fCount; i++) {
        if (hasMID) texNums[i] = faceVec[i].m;
        indices[i*3 +0] = faceVec[i].a;
        indices[i*3 +1] = faceVec[i].b;
        indices[i*3 +2] = faceVec[i].c;
    }

    gl::GenBuffers(1, &ibo);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, iCount * sizeof(uint), indices, gl::STATIC_DRAW);

    if (hasMID) {
        gl::GenBuffers(1, &vboTn);
        gl::BindBuffer(gl::TEXTURE_BUFFER, vboTn);
        gl::BufferData(gl::TEXTURE_BUFFER, fCount * sizeof(uchar), texNums, gl::STATIC_DRAW);
        gl::GenTextures(1, &tboTn);
        gl::BindTexture(gl::TEXTURE_BUFFER, tboTn);
        gl::TexBuffer(gl::TEXTURE_BUFFER, gl::R8UI, vboTn);
    }

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);
    if (hasUV) gl::EnableVertexAttribArray(2);
    if (hasBW) {
        gl::EnableVertexAttribArray(3);
        gl::EnableVertexAttribArray(4);
        gl::EnableVertexAttribArray(5);
        gl::EnableVertexAttribArray(6);
    }

    float points[vCount * 3];
    float normals[vCount * 3];
    float texcoords[vCount * 2];
    int bones[vCount * 8];
    float weights[vCount * 8];

    for (uint i = 0; i < vCount; i++) {
        points[i*3 +0] = vertVec[i].x;
        points[i*3 +1] = vertVec[i].y;
        points[i*3 +2] = vertVec[i].z;
        normals[i*3 +0] = vertVec[i].nx;
        normals[i*3 +1] = vertVec[i].ny;
        normals[i*3 +2] = vertVec[i].nz;
        if (hasUV) {
            texcoords[i*2 +0] = vertVec[i].u;
            texcoords[i*2 +1] = vertVec[i].v;
        }
        if (hasBW) {
            for (uint j = 0; j < 8; j++) {
                bones[i*8 +j] = vertVec[i].b[j];
                weights[i*8 +j] = vertVec[i].w[j];
            }
        }
    }


    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(float), points, gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, nullptr);

    gl::GenBuffers(1, &vboN);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboN);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(float), normals, gl::STATIC_DRAW);
    gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, nullptr);

    if (hasUV) {
        gl::GenBuffers(1, &vboTc);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
        gl::BufferData(gl::ARRAY_BUFFER, 2 * vCount * sizeof(float), texcoords, gl::STATIC_DRAW);
        gl::VertexAttribPointer(2, 2, gl::FLOAT, false, 0, nullptr);
    }

    if (hasBW) {
        gl::GenBuffers(1, &vboB);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboB);
        gl::BufferData(gl::ARRAY_BUFFER, vCount * 8 * sizeof(int), bones, gl::STATIC_DRAW);
        gl::VertexAttribIPointer(3, 4, gl::INT, 8*sizeof(int), nullptr);
        gl::VertexAttribIPointer(4, 4, gl::INT, 8*sizeof(int), (void*)(4*sizeof(int)));

        gl::GenBuffers(1, &vboW);
        gl::BindBuffer(gl::ARRAY_BUFFER, vboW);
        gl::BufferData(gl::ARRAY_BUFFER, vCount * 8 * sizeof(float), weights, gl::STATIC_DRAW);
        gl::VertexAttribPointer(5, 4, gl::FLOAT, false, 8*sizeof(float), nullptr);
        gl::VertexAttribPointer(6, 4, gl::FLOAT, false, 8*sizeof(float), (void*)(4*sizeof(float)));
    }
}

Mesh::~Mesh() {
    gl::DeleteBuffers(1, &ibo);
    if (hasMID) {
        gl::DeleteBuffers(1, &vboTn);
        gl::DeleteTextures(1, &tboTn);
    }
    gl::DeleteVertexArrays(1, &vao);
    gl::DeleteBuffers(1, &vboP);
    gl::DeleteBuffers(1, &vboN);
    if (hasUV) gl::DeleteBuffers(1, &vboTc);
    if (hasBW) {
        gl::DeleteBuffers(1, &vboB);
        gl::DeleteBuffers(1, &vboW);
    }
}

void Mesh::bind_buffers(GLenum _texBufSlot) {
    gl::BindVertexArray(vao);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    if (hasMID && _texBufSlot != gl::NONE) {
        gl::ActiveTexture(_texBufSlot);
        gl::BindTexture(gl::TEXTURE_BUFFER, tboTn);
    }
}
