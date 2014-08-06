#include <fstream>

#include <gl/gl.hpp>
#include <gl/mesh.hpp>
#include <extra/helpers.hpp>

namespace sq {

bool Mesh::load_from_dir(std::string dirPath) {
    Json::Value root = load_json_file(dirPath+"/model.json");

    vCount = root["vCount"].asInt();
    texWidth = root["texWidth"].asInt();
    texHeight = root["texHeight"].asInt();

    shadN = root["shadN"].asFloat();
    shadE = root["shadE"].asFloat();
    shadS = root["shadS"].asFloat();
    shadW = root["shadW"].asFloat();

    for (int i = 0; i < vCount; i++) {
        positionsVec.push_back(glm::vec3(
            root["positions"][i*3 +0].asFloat(),
            root["positions"][i*3 +1].asFloat(),
            root["positions"][i*3 +2].asFloat()
        ));

        normalsVec.push_back(glm::vec3(
            root["normals"][i*3 +0].asFloat(),
            root["normals"][i*3 +1].asFloat(),
            root["normals"][i*3 +2].asFloat()
        ));

        texcoordsVec.push_back(glm::vec2(
            root["texcoords"][i*2 +0].asFloat(),
            root["texcoords"][i*2 +1].asFloat()
        ));
    }

    glActiveTexture(GL_TEXTURE0); // used for all textures
    diffImg.loadFromFile(dirPath+"/diff.png");
    specImg.loadFromFile(dirPath+"/spec.png");
    normImg.loadFromFile(dirPath+"/norm.png");
    shadImg.loadFromFile(dirPath+"/shad.png");

    /// MODEL ///
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat positions[vCount * 3];
    GLfloat normals[vCount * 3];
    GLfloat texcoords[vCount * 2];
    GLfloat tangents[vCount * 4];

    for (int i = 0; i < vCount; i++) {
        positions[i*3 +0] = positionsVec[i].x;
        positions[i*3 +1] = positionsVec[i].y;
        positions[i*3 +2] = positionsVec[i].z;

        normals[i*3 +0] = normalsVec[i].x;
        normals[i*3 +1] = normalsVec[i].y;
        normals[i*3 +2] = normalsVec[i].z;

        texcoords[i*2 +0] = texcoordsVec[i].x;
        texcoords[i*2 +1] = texcoordsVec[i].y;

        glm::vec4 tangent = sq::get_tangent(normalsVec[i]);
        tangents[i*4 +0] = tangent.x;
        tangents[i*4 +1] = tangent.y;
        tangents[i*4 +2] = tangent.z;
        tangents[i*4 +3] = tangent.w;
    }

    // Buffers
    GLuint vboP, vboN, vboUV, vboT;

    glGenBuffers(1, &vboP);
    glBindBuffer(GL_ARRAY_BUFFER, vboP);
    glBufferData(GL_ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboN);
    glBindBuffer(GL_ARRAY_BUFFER, vboN);
    glBufferData(GL_ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboUV);
    glBindBuffer(GL_ARRAY_BUFFER, vboUV);
    glBufferData(GL_ARRAY_BUFFER, 2 * vCount * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &vboT);
    glBindBuffer(GL_ARRAY_BUFFER, vboT);
    glBufferData(GL_ARRAY_BUFFER, 4 * vCount * sizeof(GLfloat), tangents, GL_STATIC_DRAW);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(3);

    // Textures
    glGenTextures(1, &texArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, texWidth, texHeight, 3, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, texWidth, texHeight, 1,
                    GL_RGBA, GL_UNSIGNED_BYTE, diffImg.getPixelsPtr());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 1, texWidth, texHeight, 1,
                    GL_RGBA, GL_UNSIGNED_BYTE, specImg.getPixelsPtr());
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 2, texWidth, texHeight, 1,
                    GL_RGBA, GL_UNSIGNED_BYTE, normImg.getPixelsPtr());

    /// SHADOW ///
    GLfloat shadPositions[18] = {
        shadW, shadS, 0.001f,
        shadE, shadN, 0.001f,
        shadW, shadN, 0.001f,
        shadW, shadS, 0.001f,
        shadE, shadN, 0.001f,
        shadE, shadS, 0.001f
    };

    GLfloat shadTexcoords[12] = {
        0.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0
    };

    glGenVertexArrays(1, &shadVao);
    glBindVertexArray(shadVao);
    GLuint vboShadP, vboShadUV;

    glGenBuffers(1, &vboShadP);
    glBindBuffer(GL_ARRAY_BUFFER, vboShadP);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), shadPositions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboShadUV);
    glBindBuffer(GL_ARRAY_BUFFER, vboShadUV);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), shadTexcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glGenTextures(1, &shadTex);
    glBindTexture(GL_TEXTURE_2D, shadTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadImg.getSize().x, shadImg.getSize().y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, shadImg.getPixelsPtr());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return true;
}

}
