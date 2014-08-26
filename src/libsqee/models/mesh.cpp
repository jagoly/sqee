#include <gl/gl.hpp>
#include <models/mesh.hpp>
#include <misc/files.hpp>

namespace sq {


bool Mesh::load_from_dir(std::string _dirPath, std::string _skin) {
    std::vector<Vertex> vertVec;
    std::vector<glm::uvec3> faceVec;
    load_ply_from_file(_dirPath + "/model.ply", vertVec, faceVec);
    Json::Value rootSkin = get_json_from_file(_dirPath+"/skins.json")[_skin];

    std::string str = rootSkin["ambi"].asString();
    if      (str == "none") ambi = 1;
    else if (str == "full") ambi = 2;
    else                    ambi = 0;

    ushort vCount = vertVec.size();
    iCount = faceVec.size() * 3;

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

    GLushort indices[iCount];
    for (int i = 0; i < iCount / 3; i++) {
        indices[i*3 +0] = faceVec[i].x;
        indices[i*3 +1] = faceVec[i].y;
        indices[i*3 +2] = faceVec[i].z;
    }

    // Buffers
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);

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

    gl::GenBuffers(1, &ibo);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ibo);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, iCount * sizeof(GLushort), indices, gl::STATIC_DRAW);

    /// Textures ///
    // Normals
    texNorm.load_from_file("res/textures/models/norm/"
                           + rootSkin["textures"][0].asString() + ".png", gl::RGB16F);
    texNorm.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texNorm.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);

    // Diffuse
    texDiff.load_from_file("res/textures/models/diff/"
                           + rootSkin["textures"][1].asString() + ".png", gl::RGB16F);
    texDiff.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texDiff.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);

    // Specular
    texSpec.load_from_file("res/textures/models/spec/"
                           + rootSkin["textures"][2].asString() + ".png", gl::RGB16F);
    texSpec.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texSpec.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);

    return true;
}

}
