#include <libsqee/gl/gl.hpp>
#include <libsqee/misc/files.hpp>

#include "models.hpp"

namespace sqt {

bool Model::load_from_dir(std::string _mesh, std::string _skin, sq::TexHolderBasic* _texHolder) {
    std::string mPath = "res/models/meshes/" + _mesh+".ply";
    std::string sPath = "res/models/skins/" + _skin+".json";

    std::vector<sq::Vertex> vertVec;
    std::vector<glm::uvec3> faceVec;

    mesh = MeshPtr(new Mesh(mPath));

    skin.load(sPath, _texHolder);

    load_ply_from_file(mPath, vertVec, faceVec);


    ushort vCount = vertVec.size();
    mesh->iCount = faceVec.size() * 3;

    GLfloat points[vCount * 3];
    GLfloat normals[vCount * 3];
    GLfloat texcoords[vCount * 2];
    GLfloat tangents[vCount * 3];
    for (int i = 0; i < vCount; i++) {
        points[i*3 +0] = vertVec[i].x;
        points[i*3 +1] = vertVec[i].y;
        points[i*3 +2] = vertVec[i].z;
        normals[i*3 +0] = vertVec[i].nx;
        normals[i*3 +1] = vertVec[i].ny;
        normals[i*3 +2] = vertVec[i].nz;
        texcoords[i*2 +0] = vertVec[i].s;
        texcoords[i*2 +1] = vertVec[i].t;
        tangents[i*3 +0] = vertVec[i].tx;
        tangents[i*3 +1] = vertVec[i].ty;
        tangents[i*3 +2] = vertVec[i].tz;
    }

    GLushort indices[mesh->iCount];
    for (int i = 0; i < mesh->iCount / 3; i++) {
        indices[i*3 +0] = faceVec[i].x;
        indices[i*3 +1] = faceVec[i].y;
        indices[i*3 +2] = faceVec[i].z;
    }

    // Buffers
    gl::GenVertexArrays(1, &mesh->vao);
    gl::BindVertexArray(mesh->vao);

    GLuint vboP, vboN, vboTc, vboT;

    gl::GenBuffers(1, &vboP);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboP);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), points, gl::STATIC_DRAW);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, NULL);
    gl::EnableVertexAttribArray(0);

    gl::GenBuffers(1, &vboN);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboN);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), normals, gl::STATIC_DRAW);
    gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, NULL);
    gl::EnableVertexAttribArray(1);

    gl::GenBuffers(1, &vboTc);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTc);
    gl::BufferData(gl::ARRAY_BUFFER, 2 * vCount * sizeof(GLfloat), texcoords, gl::STATIC_DRAW);
    gl::VertexAttribPointer(2, 2, gl::FLOAT, false, 0, NULL);
    gl::EnableVertexAttribArray(2);

    gl::GenBuffers(1, &vboT);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboT);
    gl::BufferData(gl::ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), tangents, gl::STATIC_DRAW);
    gl::VertexAttribPointer(3, 3, gl::FLOAT, false, 0, NULL);
    gl::EnableVertexAttribArray(3);

    gl::GenBuffers(1, &mesh->ibo);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, mesh->ibo);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, mesh->iCount * sizeof(GLushort), indices, gl::STATIC_DRAW);



    return true;
}

}
