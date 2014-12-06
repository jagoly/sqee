#include <fstream>
#include <sstream>

#include <models/mesh.hpp>
#include <misc/files.hpp>

using namespace sq;

void Mesh::load(const string& _filePath) {
    faceVec.resize(1);
    uint countedFaces = 0;

    string filePath = SQ_MODELS "meshes/" + _filePath + ".sqm";
    std::ifstream src(filePath);
    if (!src.is_open()) cout << "ERROR: Couldn't open file \"" << filePath << "\"" << endl;

    string line, section = "";
    while (std::getline(src, line)) {
        vector<string> vec;
        {   std::stringstream stream(line); string val;
            while (stream >> val) vec.emplace_back(val);
        }

        if (vec.empty() || vec[0] == "#") continue;

        if (vec[0] == "{") {
            if (!section.empty()) throw; // already in a section
            section = vec[1]; continue;
        }
        if (vec[0] == "}") {
            if (section.empty()) throw; // not in a section
            section = ""; continue;
        }

        if (section == "header") {
            if      (vec[0] == "vCount") vCount = std::stoi(vec[1]);
            else if (vec[0] == "fCount") fCount = std::stoi(vec[1]);
            else if (vec[0] == "mCount") faceVec.resize(std::stoi(vec[1]));
            else if (vec[0] == "hasNM") hasNM = true;
            else if (vec[0] == "hasUV") hasUV = true;
            else if (vec[0] == "hasBW") hasBW = true;
            else if (vec[0] == "hasMT") hasMT = true;
            else throw; // invalid key
            continue;
        }

        if (section == "vertices") {
            float x = std::stof(vec[0]);
            float y = std::stof(vec[1]);
            float z = std::stof(vec[2]);
            float nx = hasNM ? std::stof(vec[3]) : 0.f;
            float ny = hasNM ? std::stof(vec[4]) : 0.f;
            float nz = hasNM ? std::stof(vec[5]) : 0.f;
            int o = 3 * !hasNM;
            float u = hasUV ? std::stof(vec[6-o]) : 0.f;
            float v = hasUV ? std::stof(vec[7-o]) : 0.f;
            o += 2 * !hasUV;
            int b0 = hasBW ? std::stoi(vec[8-o])  : -1;
            int b1 = hasBW ? std::stoi(vec[9-o])  : -1;
            int b2 = hasBW ? std::stoi(vec[10-o]) : -1;
            int b3 = hasBW ? std::stoi(vec[11-o]) : -1;
            int b4 = hasBW ? std::stoi(vec[12-o]) : -1;
            int b5 = hasBW ? std::stoi(vec[13-o]) : -1;
            int b6 = hasBW ? std::stoi(vec[14-o]) : -1;
            int b7 = hasBW ? std::stoi(vec[15-o]) : -1;
            float w0 = hasBW ? std::stof(vec[16-o]) : 0.f;
            float w1 = hasBW ? std::stof(vec[17-o]) : 0.f;
            float w2 = hasBW ? std::stof(vec[18-o]) : 0.f;
            float w3 = hasBW ? std::stof(vec[19-o]) : 0.f;
            float w4 = hasBW ? std::stof(vec[20-o]) : 0.f;
            float w5 = hasBW ? std::stof(vec[21-o]) : 0.f;
            float w6 = hasBW ? std::stof(vec[22-o]) : 0.f;
            float w7 = hasBW ? std::stof(vec[23-o]) : 0.f;

            vertVec.push_back({x, y, z, nx, ny, nz, u, v,
                               {b0, b1, b2, b3, b4, b5, b6, b7},
                               {w0, w1, w2, w3, w4, w5, w6, w7}});
            continue;
        }

        if (section == "faces") {
            countedFaces++;
            uint a = std::stoi(vec[0]);
            uint b = std::stoi(vec[1]);
            uint c = std::stoi(vec[2]);
            uchar m = hasMT ? std::stoi(vec[3]) : 0;

            faceVec[m].push_back({a, b, c});
            continue;
        }
    }

    #ifdef SQEE_DEBUG
    if (vCount != vertVec.size())
        cout << "WARNING: vCount mismatch when loading mesh from \""
             << filePath << "\"" << endl;
    if (fCount != countedFaces)
        cout << "WARNING: fCount mismatch when loading mesh from \""
             << filePath << "\"" << endl;
    #endif

    for (uint i = 0; i < faceVec.size(); i++) {
        iboVec.emplace_back(0, faceVec[i].size() * 3);
        gl::GenBuffers(1, &(iboVec[i].first));
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, iboVec[i].first);
        gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, iboVec[i].second * sizeof(GLuint), faceVec[i].data(), gl::STATIC_DRAW);
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
    for (pair<GLuint, uint>& iboPair : iboVec)
        gl::DeleteBuffers(1, &iboPair.first);
    gl::DeleteVertexArrays(1, &vao);
    gl::DeleteBuffers(1, &vboP);
    gl::DeleteBuffers(1, &vboN);
    gl::DeleteBuffers(1, &vboT);
    if (hasUV)
        gl::DeleteBuffers(1, &vboTc);
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
