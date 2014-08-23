#include <gl/gl.hpp>
#include <models/mesh.hpp>
#include <misc/files.hpp>

namespace sq {

bool Mesh::load_from_dir(std::string _dirPath, std::string _skin) {
    Json::Value rootModel = get_json_from_file(_dirPath+"/model.json");
    Json::Value rootSkin = get_json_from_file(_dirPath+"/skins.json")[_skin];

    vCount = rootModel["vCount"].asInt();
    hasTex[0] = rootSkin["textures"][0].asBool();
    hasTex[1] = rootSkin["textures"][1].asBool();
    hasTex[2] = rootSkin["textures"][2].asBool();
    hasTex[3] = rootSkin["textures"][3].asBool();
    texWidth = rootSkin["width"].asInt();
    texHeight = rootSkin["height"].asInt();

    std::vector<glm::vec3> pointsVec;
    std::vector<glm::vec3> normalsVec;
    std::vector<glm::vec2> texcoordsVec;
    for (int i = 0; i < vCount; i++) {
        pointsVec.push_back(glm::vec3(
            rootModel["points"][i*3 +0].asFloat(),
            rootModel["points"][i*3 +1].asFloat(),
            rootModel["points"][i*3 +2].asFloat()
        ));

        normalsVec.push_back(glm::vec3(
            rootModel["normals"][i*3 +0].asFloat(),
            rootModel["normals"][i*3 +1].asFloat(),
            rootModel["normals"][i*3 +2].asFloat()
        ));

        texcoordsVec.push_back(glm::vec2(
            rootModel["texcoords"][i*2 +0].asFloat(),
            rootModel["texcoords"][i*2 +1].asFloat()
        ));
    }

    /// MODEL ///
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);

    GLfloat points[vCount * 3];
    GLfloat normals[vCount * 3];
    GLfloat texcoords[vCount * 2];
    GLfloat tangents[vCount * 3];

    for (int i = 0; i < vCount; i++) {
        points[i*3 +0] = pointsVec[i].x;
        points[i*3 +1] = pointsVec[i].y;
        points[i*3 +2] = pointsVec[i].z;

        normals[i*3 +0] = normalsVec[i].x;
        normals[i*3 +1] = normalsVec[i].y;
        normals[i*3 +2] = normalsVec[i].z;

        texcoords[i*2 +0] = texcoordsVec[i].x;
        texcoords[i*2 +1] = texcoordsVec[i].y;

        glm::vec3 tangent = sq::get_tangent(normalsVec[i]);
        tangents[i*3 +0] = tangent.x;
        tangents[i*3 +1] = tangent.y;
        tangents[i*3 +2] = tangent.z;
    }

    // Buffers
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


    /// Textures ///
    // Normals
    if (hasTex[0]) {
        texNorm.load_from_file("res/textures/models/norm/"
                               + rootSkin["textures"][0].asString() + ".png", gl::RGB16F);
        texNorm.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texNorm.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
    }

    // Diffuse
    if (hasTex[1]) {
        texDiff.load_from_file("res/textures/models/diff/"
                               + rootSkin["textures"][1].asString() + ".png", gl::RGBA16F);
        texDiff.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texDiff.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
    }

    // Ambient
    if (hasTex[2]) {
        texAmbi.load_from_file("res/textures/models/ambi/"
                               + rootSkin["textures"][2].asString() + ".png", gl::R16F);
        texAmbi.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texAmbi.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
    }

    // Specular
    if (hasTex[3]) {
        texSpec.load_from_file("res/textures/models/spec/"
                               + rootSkin["textures"][3].asString() + ".png", gl::RGB16F);
        texSpec.set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
        texSpec.set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
    }

    return true;
}

}
