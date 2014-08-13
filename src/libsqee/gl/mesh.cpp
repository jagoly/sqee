#include <fstream>

#include <gl/gl.hpp>
#include <gl/mesh.hpp>
#include <extra/helpers.hpp>

namespace sq {

bool Mesh::load_from_dir(std::string dirPath) {
    Json::Value root = load_json_file(dirPath+"/model.json");

    vCount = root["vCount"].asInt();
    hasNorm = root["hasNorm"].asBool();
    hasSpec = root["hasSpec"].asBool();
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texWidth, texHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, normImg.getPixelsPtr());
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texWidth, texHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, diffImg.getPixelsPtr());
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, texWidth, texHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, specImg.getPixelsPtr());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    return true;
}

}
