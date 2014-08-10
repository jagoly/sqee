#include <fstream>

#include <gl/gl.hpp>
#include <gl/mesh.hpp>
#include <extra/helpers.hpp>

namespace sq {

bool Mesh::load_from_dir(std::string dirPath) {
    Json::Value root = load_json_file(dirPath+"/model.json");

    vCount = root["vCount"].asInt();
    hasNorm = root["hasNorm"].asBool();
    hasAmbi = root["hasAmbi"].asBool();
    hasSpec = root["hasSpec"].asBool();
    hasShadow = root["castShadow"].asBool();
    texWidth = root["texWidth"].asInt();
    texHeight = root["texHeight"].asInt();

    for (int i = 0; i < vCount; i++) {
        pointsVec.push_back(glm::vec3(
            root["points"][i*3 +0].asFloat(),
            root["points"][i*3 +1].asFloat(),
            root["points"][i*3 +2].asFloat()
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

    float shadowPoints[18];
    if (hasShadow) {
        shadowVec.push_back(glm::vec3(
            root["P0"][0].asFloat(), root["P0"][1].asFloat(), root["P0"][2].asFloat()));
        shadowVec.push_back(glm::vec3(
            root["P1"][0].asFloat(), root["P1"][1].asFloat(), root["P1"][2].asFloat()));
        shadowVec.push_back(glm::vec3(
            root["P2"][0].asFloat(), root["P2"][1].asFloat(), root["P2"][2].asFloat()));
        shadowVec.push_back(glm::vec3(
            root["P3"][0].asFloat(), root["P3"][1].asFloat(), root["P3"][2].asFloat()));

        shadowPoints[0] = shadowPoints[9]  = shadowVec[2].x;
        shadowPoints[1] = shadowPoints[10] = shadowVec[2].y;
        shadowPoints[2] = shadowPoints[11] = shadowVec[2].z;
        shadowPoints[3] = shadowPoints[12] = shadowVec[0].x;
        shadowPoints[4] = shadowPoints[13] = shadowVec[0].y;
        shadowPoints[5] = shadowPoints[14] = shadowVec[0].z;
        shadowPoints[6]                    = shadowVec[3].x;
        shadowPoints[7]                    = shadowVec[3].y;
        shadowPoints[8]                    = shadowVec[3].z;
                          shadowPoints[15] = shadowVec[1].x;
                          shadowPoints[16] = shadowVec[1].y;
                          shadowPoints[17] = shadowVec[1].z;
    }



    glActiveTexture(GL_TEXTURE0); // used for all textures

    /// MODEL ///
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat points[vCount * 3];
    GLfloat normals[vCount * 3];
    GLfloat texcoords[vCount * 2];
    GLfloat tangents[vCount * 4];

    for (int i = 0; i < vCount; i++) {
        points[i*3 +0] = pointsVec[i].x;
        points[i*3 +1] = pointsVec[i].y;
        points[i*3 +2] = pointsVec[i].z;

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
    GLuint vboP, vboN, vboTc, vboT;

    glGenBuffers(1, &vboP);
    glBindBuffer(GL_ARRAY_BUFFER, vboP);
    glBufferData(GL_ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboN);
    glBindBuffer(GL_ARRAY_BUFFER, vboN);
    glBufferData(GL_ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboTc);
    glBindBuffer(GL_ARRAY_BUFFER, vboTc);
    glBufferData(GL_ARRAY_BUFFER, 2 * vCount * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &vboT);
    glBindBuffer(GL_ARRAY_BUFFER, vboT);
    glBufferData(GL_ARRAY_BUFFER, 4 * vCount * sizeof(GLfloat), tangents, GL_STATIC_DRAW);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(3);


    /// Textures ///

    // Normals
    if (hasNorm) {
        normImg.loadFromFile(dirPath+"/norm.png");
        glGenTextures(1, &texNorm);
        glBindTexture(GL_TEXTURE_2D, texNorm);
        glTexImage2D(texNorm, 0, GL_RGB16F, texWidth, texHeight, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, normImg.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Diffuse
    {
        diffImg.loadFromFile(dirPath+"/diff.png");
        glGenTextures(1, &texDiff);
        glBindTexture(GL_TEXTURE_2D, texDiff);
        glTexImage2D(texNorm, 0, GL_RGBA16F, texWidth, texHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, diffImg.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Ambient
    if (hasAmbi) {
        ambiImg.loadFromFile(dirPath+"/ambi.png");
        glGenTextures(1, &texAmbi);
        glBindTexture(GL_TEXTURE_2D, texAmbi);
        glTexImage2D(texNorm, 0, GL_RGBA16F, texWidth, texHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, ambiImg.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Specular
    if (hasSpec) {
        specImg.loadFromFile(dirPath+"/spec.png");
        glGenTextures(1, &texSpec);
        glBindTexture(GL_TEXTURE_2D, texSpec);
        glTexImage2D(texNorm, 0, GL_RGBA16F, texWidth, texHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, specImg.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }


    /// SHADOW ///

    if (hasShadow) {
        glGenVertexArrays(1, &vaoShadow);
        glBindVertexArray(vaoShadow);
        GLuint vboShadowPoints, vboShadowTexcoords;

        glGenBuffers(1, &vboShadowPoints);
        glBindBuffer(GL_ARRAY_BUFFER, vboShadowPoints);
        glBufferData(GL_ARRAY_BUFFER, 3 * vCount * sizeof(GLfloat), shadowPoints, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &vboShadowTexcoords);
        glBindBuffer(GL_ARRAY_BUFFER, vboShadowTexcoords);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), shadowTexcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        shadowImg.loadFromFile(dirPath+"/shadow.png");
        glGenTextures(1, &texShadow);
        glBindTexture(GL_TEXTURE_2D, texShadow);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shadowImg.getSize().x, shadowImg.getSize().y, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, shadowImg.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    return true;
}

}
