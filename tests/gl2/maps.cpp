#include "maps.hpp"

#include <libsqee/extra/helpers.hpp>

#define CHECK_BIT(_v, _p) bool((_v) & (1<<(_p)))

namespace sqt {

const float xyTiles[24] = {
    0.f, 1.f,  1.f, 1.f,  0.5f, 0.5f,
    1.f, 1.f,  1.f, 0.f,  0.5f, 0.5f,
    1.f, 0.f,  0.f, 0.f,  0.5f, 0.5f,
    0.f, 0.f,  0.f, 1.f,  0.5f, 0.5f
};

const float typeNormals[13][4][2] = {
    {{0.f, 0.f},     {0.f, 0.f},     {0.f, 0.f},     {0.f, 0.f}},    // 0

    {{0.f, 0.25f},   {0.f, 0.25f},   {0.f, 0.25f},   {0.f, 0.25f}},  // 1
    {{0.25f, 0.f},   {0.25f, 0.f},   {0.25f, 0.f},   {0.25f, 0.f}},  // 2
    {{0.f, -0.25f},  {0.f, -0.25f},  {0.f, -0.25f},  {0.f, -0.25f}}, // 3
    {{-0.25f, 0.f},  {-0.25f, 0.f},  {-0.25f, 0.f},  {-0.25f, 0.f}}, // 4

    {{0.f, 0.25f},   {0.25f, 0.f},   {0.25f, 0.f},   {0.f, 0.25f}},  // 5
    {{0.25f, 0.f},   {0.25f, 0.f},   {0.f, -0.25f},  {0.f, -0.25f}}, // 6
    {{-0.25f, 0.f},  {0.f, -0.25f},  {0.f, -0.25f},  {-0.25f, 0.f}}, // 7
    {{0.f, 0.25f},   {0.f, 0.25f},   {-0.25f, 0.f},  {-0.25f, 0.f}}, // 8

    {{0.f, -0.25f},  {-0.25f, 0.f},  {-0.25f, 0.f},  {0.f, -0.25f}}, // 9
    {{-0.25f, 0.f},  {-0.25f, 0.f},  {0.f, 0.25f},   {0.f, 0.25f}},  // 10
    {{0.25f, 0.f},   {0.f, 0.25f},   {0.f, 0.25f},   {0.25f, 0.f}},  // 11
    {{0.f, -0.25f},  {0.f, -0.25f},  {0.25f, 0.f},   {0.25f, 0.f}}   // 12
};

const float typeZCoords[13][5] = {
    {0.f,   0.f,  0.f,  0.f,  0.f},

    {0.25f, 0.f,  0.5f, 0.5f, 0.f},
    {0.25f, 0.f,  0.f,  0.5f, 0.5f},
    {0.25f, 0.5f, 0.f,  0.f,  0.5f},
    {0.25f, 0.5f, 0.5f, 0.f,  0.f},

    {0.25f, 0.f,  0.f,  0.5f, 0.f},
    {0.25f, 0.f,  0.f,  0.f,  0.5f},
    {0.25f, 0.5f, 0.f,  0.f,  0.f},
    {0.25f, 0.f,  0.5f, 0.f,  0.f},

    {0.25f, 0.5f, 0.5f, 0.f,  0.5f},
    {0.25f, 0.5f, 0.5f, 0.5f, 0.f},
    {0.25f, 0.f,  0.5f, 0.5f, 0.5f},
    {0.25f, 0.5f, 0.f,  0.5f, 0.5f}
};

ModelInstance::ModelInstance(glm::vec3 _pos, glm::vec3 _scale, bool _hasShadow, int _index) {
    pos = _pos;
    scale = _scale;
    hasShadow = _hasShadow;
    index = _index;
    modelMatrix = glm::translate(glm::mat4(), pos) * glm::scale(glm::mat4(), scale);
}

bool LevelMap::load_map(std::string dirPath) {
    mapDirPath = dirPath;
    // Put some kind of checks here
    return false;
}

bool LevelMap::load_ground() {
    Json::Value root = sq::load_json_file(mapDirPath+"/ground.json");

    std::vector<int> tilesGeometry;
    std::vector<int> tilesTextures;
    std::vector<std::string> texPaths;

    ground.width = root["width"].asInt();
    ground.height = root["height"].asInt();
    ground.texCount = root["textures"].size();

    int width = ground.width;
    int height = ground.height;
    int texCount = ground.texCount;

    for (int i = 0; i < width*height; i++) {
        tilesGeometry.push_back(root["tilesGeometry"][i].asInt());
        tilesTextures.push_back(root["tilesTextures"][i].asInt());
    }

    for (int i = 0; i < texCount; i++) {
        texPaths.push_back(root["textures"][i].asString());
    }

    ground.pCount = width * height * 12;

    float points[width * height * 36];
    float normalsFlat[width * height * 36];
    float normals[width * height * 36];
    float texcoords[width * height * 36];
    float tangents[width * height * 48];

    // Working Arrays
    int zBases[width * height];
    bool invisibles[width * height];
    int types[width * height];
    bool inclines[width * height];
    bool smooths[width * height];

    // Fill working arrays and set textures (Z texcoord)
    for (int y_ = 0; y_ < height; y_++) {
        int y = height - y_ - 1;
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            int val = tilesGeometry[width*y_ + x];

            // Get all properties from tile's 16bit int
            zBases[pos]      = (val >> 8) % 256;
            invisibles[pos]  = CHECK_BIT(val, 6);
            types[pos]       = (val >> 2) % 16;
            inclines[pos]    = CHECK_BIT(val, 1);
            smooths[pos]     = CHECK_BIT(val, 0);


            // Set Textures
            for (int p = 0; p < 12; p++) {
                texcoords[pos*36 + p*3 +2] = tilesTextures[width*y_ + x];
            }
        }
    }

    // Calculate points and base Z normals
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;

            // Set X and Y Points and Texcoords
            for (int p = 0; p < 12; p++) {
                points[pos*36 + p*3 +0] = float(x) + xyTiles[p*2 +0];
                points[pos*36 + p*3 +1] = float(y) + xyTiles[p*2 +1];
                texcoords[pos*36 + p*3 +0] = xyTiles[p*2 +0];
                texcoords[pos*36 + p*3 +1] = xyTiles[p*2 +1];
            }

            // Set Z coords
            points[pos*36 + 0*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][4] * (1 + inclines[pos]);
            points[pos*36 + 1*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][1] * (1 + inclines[pos]);
            points[pos*36 + 2*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][0] * (1 + inclines[pos]);
            points[pos*36 + 3*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][1] * (1 + inclines[pos]);
            points[pos*36 + 4*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][2] * (1 + inclines[pos]);
            points[pos*36 + 5*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][0] * (1 + inclines[pos]);
            points[pos*36 + 6*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][2] * (1 + inclines[pos]);
            points[pos*36 + 7*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][3] * (1 + inclines[pos]);
            points[pos*36 + 8*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][0] * (1 + inclines[pos]);
            points[pos*36 + 9*3  +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][3] * (1 + inclines[pos]);
            points[pos*36 + 10*3 +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][4] * (1 + inclines[pos]);
            points[pos*36 + 11*3 +2] = 0.5f * zBases[pos] + typeZCoords[types[pos]][0] * (1 + inclines[pos]);

            // Set flat normals and tangents
            for (int f = 0; f < 4; f++) {
                for (int p = 0; p < 3; p++) {
                    normalsFlat[pos*36 + f*9 + p*3 +0] = typeNormals[types[pos]][f][0] * (1 + inclines[pos]);
                    normalsFlat[pos*36 + f*9 + p*3 +1] = typeNormals[types[pos]][f][1] * (1 + inclines[pos]);
                    if (types[pos]) normalsFlat[pos*36 + f*9 + p*3 +2] = 1.f - 0.25f * (1 + inclines[pos]);
                    else normalsFlat[pos*36 + f*9 + p*3 +2] = 1.f;

                    glm::vec3 normal(normalsFlat[pos*36 + f*9 + p*3 +0],
                                     normalsFlat[pos*36 + f*9 + p*3 +1],
                                     normalsFlat[pos*36 + f*9 + p*3 +2]);

                    glm::vec4 tangent = sq::get_tangent(normal);

                    tangents[pos*48 + f*12 + p*4 +0] = tangent.x;
                    tangents[pos*48 + f*12 + p*4 +1] = tangent.y;
                    tangents[pos*48 + f*12 + p*4 +2] = tangent.z;
                    tangents[pos*48 + f*12 + p*4 +3] = tangent.w;
                }
            }
        }
    }

    for (int i = 0; i < width * height * 36; i++) {
        normals[i] = normalsFlat[i];
    }

    // Calculate smooth normals and set normals
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;

            // No smoothing
            if (!smooths[pos]) continue;

            // Set up just a few variables
            int nPos = pos + width;
            int ePos = pos + 1;
            int sPos = pos - width;
            int wPos = pos - 1;
            int nePos = pos + width + 1;
            int esPos = pos + 1 - width;
            int swPos = pos - width - 1;
            int wnPos = pos - 1 + width;

            std::vector<glm::vec3> neNorms; neNorms.resize(8);
            std::vector<glm::vec3> esNorms; esNorms.resize(8);
            std::vector<glm::vec3> swNorms; swNorms.resize(8);
            std::vector<glm::vec3> wnNorms; wnNorms.resize(8);

            // Check map edges
            bool nEdge = (y == height-1);
            bool eEdge = (x == width-1);
            bool sEdge = (y == 0);
            bool wEdge = (x == 0);


            // North East
            neNorms[0] = glm::vec3(normalsFlat[pos*36 + 1*3 +0],
                                   normalsFlat[pos*36 + 1*3 +1],
                                   normalsFlat[pos*36 + 1*3 +2]);
            neNorms[1] = glm::vec3(normalsFlat[pos*36 + 3*3 +0],
                                   normalsFlat[pos*36 + 3*3 +1],
                                   normalsFlat[pos*36 + 3*3 +2]);

            if (nEdge) {
                neNorms[2] = neNorms[0]; neNorms[3] = neNorms[1];
            } else {
                neNorms[2] = glm::vec3(normalsFlat[nPos*36 + 4*3 +0],
                                       normalsFlat[nPos*36 + 4*3 +1],
                                       normalsFlat[nPos*36 + 4*3 +2]);
                neNorms[3] = glm::vec3(normalsFlat[nPos*36 + 6*3 +0],
                                       normalsFlat[nPos*36 + 6*3 +1],
                                       normalsFlat[nPos*36 + 6*3 +2]);
            }

            if (eEdge) {
                neNorms[4] = neNorms[0]; neNorms[5] = neNorms[1];
            } else {
                neNorms[4] = glm::vec3(normalsFlat[ePos*36 + 10*3 +0],
                                       normalsFlat[ePos*36 + 10*3 +1],
                                       normalsFlat[ePos*36 + 10*3 +2]);
                neNorms[5] = glm::vec3(normalsFlat[ePos*36 + 0*3  +0],
                                       normalsFlat[ePos*36 + 0*3  +1],
                                       normalsFlat[ePos*36 + 0*3  +2]);
            }

            if (nEdge | eEdge) {
                neNorms[6] = neNorms[0]; neNorms[7] = neNorms[1];
            } else {
                neNorms[6] = glm::vec3(normalsFlat[nePos*36 + 7*3 +0],
                                       normalsFlat[nePos*36 + 7*3 +1],
                                       normalsFlat[nePos*36 + 7*3 +2]);
                neNorms[7] = glm::vec3(normalsFlat[nePos*36 + 9*3 +0],
                                       normalsFlat[nePos*36 + 9*3 +1],
                                       normalsFlat[nePos*36 + 9*3 +2]);
            }

            // East South
            esNorms[0] = glm::vec3(normalsFlat[pos*36 + 4*3 +0],
                                   normalsFlat[pos*36 + 4*3 +1],
                                   normalsFlat[pos*36 + 4*3 +2]);
            esNorms[1] = glm::vec3(normalsFlat[pos*36 + 6*3 +0],
                                   normalsFlat[pos*36 + 6*3 +1],
                                   normalsFlat[pos*36 + 6*3 +2]);

            if (eEdge) {
                esNorms[2] = esNorms[0]; esNorms[3] = esNorms[1];
            } else {
                esNorms[2] = glm::vec3(normalsFlat[ePos*36 + 7*3 +0],
                                       normalsFlat[ePos*36 + 7*3 +1],
                                       normalsFlat[ePos*36 + 7*3 +2]);
                esNorms[3] = glm::vec3(normalsFlat[ePos*36 + 9*3 +0],
                                       normalsFlat[ePos*36 + 9*3 +1],
                                       normalsFlat[ePos*36 + 9*3 +2]);
            }

            if (sEdge) {
                esNorms[4] = esNorms[0]; esNorms[5] = esNorms[1];
            } else {
                esNorms[4] = glm::vec3(normalsFlat[sPos*36 + 1*3 +0],
                                       normalsFlat[sPos*36 + 1*3 +1],
                                       normalsFlat[sPos*36 + 1*3 +2]);
                esNorms[5] = glm::vec3(normalsFlat[sPos*36 + 3*3 +0],
                                       normalsFlat[sPos*36 + 3*3 +1],
                                       normalsFlat[sPos*36 + 3*3 +2]);
            }

            if (eEdge | sEdge) {
                esNorms[6] = esNorms[0]; esNorms[7] = esNorms[1];
            } else {
                esNorms[6] = glm::vec3(normalsFlat[esPos*36 + 10*3 +0],
                                       normalsFlat[esPos*36 + 10*3 +1],
                                       normalsFlat[esPos*36 + 10*3 +2]);
                esNorms[7] = glm::vec3(normalsFlat[esPos*36 + 0*3  +0],
                                       normalsFlat[esPos*36 + 0*3  +1],
                                       normalsFlat[esPos*36 + 0*3  +2]);
            }


            // South West
            swNorms[0] = glm::vec3(normalsFlat[pos*36 + 7*3 +0],
                                   normalsFlat[pos*36 + 7*3 +1],
                                   normalsFlat[pos*36 + 7*3 +2]);
            swNorms[1] = glm::vec3(normalsFlat[pos*36 + 9*3 +0],
                                   normalsFlat[pos*36 + 9*3 +1],
                                   normalsFlat[pos*36 + 9*3 +2]);

            if (sEdge) {
                swNorms[2] = swNorms[0]; swNorms[3] = swNorms[1];
            } else {
                swNorms[2] = glm::vec3(normalsFlat[sPos*36 + 10*3 +0],
                                       normalsFlat[sPos*36 + 10*3 +1],
                                       normalsFlat[sPos*36 + 10*3 +2]);
                swNorms[3] = glm::vec3(normalsFlat[sPos*36 + 0*3  +0],
                                       normalsFlat[sPos*36 + 0*3  +1],
                                       normalsFlat[sPos*36 + 0*3  +2]);
            }

            if (wEdge) {
                swNorms[4] = swNorms[0]; swNorms[5] = swNorms[1];
            } else {
                swNorms[4] = glm::vec3(normalsFlat[wPos*36 + 4*3 +0],
                                       normalsFlat[wPos*36 + 4*3 +1],
                                       normalsFlat[wPos*36 + 4*3 +2]);
                swNorms[5] = glm::vec3(normalsFlat[wPos*36 + 6*3 +0],
                                       normalsFlat[wPos*36 + 6*3 +1],
                                       normalsFlat[wPos*36 + 6*3 +2]);
            }

            if (sEdge | wEdge) {
                swNorms[6] = swNorms[0]; swNorms[7] = swNorms[1];
            } else {
                swNorms[6] = glm::vec3(normalsFlat[swPos*36 + 1*3 +0],
                                       normalsFlat[swPos*36 + 1*3 +1],
                                       normalsFlat[swPos*36 + 1*3 +2]);
                swNorms[7] = glm::vec3(normalsFlat[swPos*36 + 3*3 +0],
                                       normalsFlat[swPos*36 + 3*3 +1],
                                       normalsFlat[swPos*36 + 3*3 +2]);
            }


            // West North
            wnNorms[0] = glm::vec3(normalsFlat[pos*36 + 10*3 +0],
                                   normalsFlat[pos*36 + 10*3 +1],
                                   normalsFlat[pos*36 + 10*3 +2]);
            wnNorms[1] = glm::vec3(normalsFlat[pos*36 + 0*3  +0],
                                   normalsFlat[pos*36 + 0*3  +1],
                                   normalsFlat[pos*36 + 0*3  +2]);

            if (wEdge) {
                wnNorms[2] = wnNorms[0]; wnNorms[3] = wnNorms[1];
            } else {
                wnNorms[2] = glm::vec3(normalsFlat[wPos*36 + 1*3 +0],
                                       normalsFlat[wPos*36 + 1*3 +1],
                                       normalsFlat[wPos*36 + 1*3 +2]);
                wnNorms[3] = glm::vec3(normalsFlat[wPos*36 + 3*3 +0],
                                       normalsFlat[wPos*36 + 3*3 +1],
                                       normalsFlat[wPos*36 + 3*3 +2]);
            }

            if (nEdge) {
                wnNorms[4] = wnNorms[0]; wnNorms[5] = wnNorms[1];
            } else {
                wnNorms[4] = glm::vec3(normalsFlat[nPos*36 + 7*3 +0],
                                       normalsFlat[nPos*36 + 7*3 +1],
                                       normalsFlat[nPos*36 + 7*3 +2]);
                wnNorms[5] = glm::vec3(normalsFlat[nPos*36 + 9*3 +0],
                                       normalsFlat[nPos*36 + 9*3 +1],
                                       normalsFlat[nPos*36 + 9*3 +2]);
            }

            if (wEdge | nEdge) {
                wnNorms[6] = wnNorms[0]; wnNorms[7] = wnNorms[1];
            } else {
                wnNorms[6] = glm::vec3(normalsFlat[wnPos*36 + 4*3 +0],
                                       normalsFlat[wnPos*36 + 4*3 +1],
                                       normalsFlat[wnPos*36 + 4*3 +2]);
                wnNorms[7] = glm::vec3(normalsFlat[wnPos*36 + 6*3 +0],
                                       normalsFlat[wnPos*36 + 6*3 +1],
                                       normalsFlat[wnPos*36 + 6*3 +2]);
            }

            // Work out smoothed vertex normals
            glm::vec3 neSum;
            glm::vec3 esSum;
            glm::vec3 swSum;
            glm::vec3 wnSum;
            for (glm::vec3& norm : neNorms) neSum += norm;
            for (glm::vec3& norm : esNorms) esSum += norm;
            for (glm::vec3& norm : swNorms) swSum += norm;
            for (glm::vec3& norm : wnNorms) wnSum += norm;

            neSum = glm::normalize(neSum);
            esSum = glm::normalize(esSum);
            swSum = glm::normalize(swSum);
            wnSum = glm::normalize(wnSum);

            normals[pos*36 + 1*3 +0] = neSum.x;
            normals[pos*36 + 1*3 +1] = neSum.y;
            normals[pos*36 + 1*3 +2] = neSum.z;
            normals[pos*36 + 3*3 +0] = neSum.x;
            normals[pos*36 + 3*3 +1] = neSum.y;
            normals[pos*36 + 3*3 +2] = neSum.z;

            normals[pos*36 + 4*3 +0] = esSum.x;
            normals[pos*36 + 4*3 +1] = esSum.y;
            normals[pos*36 + 4*3 +2] = esSum.z;
            normals[pos*36 + 6*3 +0] = esSum.x;
            normals[pos*36 + 6*3 +1] = esSum.y;
            normals[pos*36 + 6*3 +2] = esSum.z;

            normals[pos*36 + 7*3 +0] = swSum.x;
            normals[pos*36 + 7*3 +1] = swSum.y;
            normals[pos*36 + 7*3 +2] = swSum.z;
            normals[pos*36 + 9*3 +0] = swSum.x;
            normals[pos*36 + 9*3 +1] = swSum.y;
            normals[pos*36 + 9*3 +2] = swSum.z;

            normals[pos*36 + 10*3 +0] = wnSum.x;
            normals[pos*36 + 10*3 +1] = wnSum.y;
            normals[pos*36 + 10*3 +2] = wnSum.z;
            normals[pos*36 + 0*3 +0] = wnSum.x;
            normals[pos*36 + 0*3 +1] = wnSum.y;
            normals[pos*36 + 0*3 +2] = wnSum.z;
        }
    }


    // Remove non-visible tiles
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            if (invisibles[pos]) {
                for (int c = 0; c < 36; c++) {
                    points[pos*36 + c] = 0.f;
                }
            }
        }
    }

    GLuint vboPoints = 0;
    GLuint vboNormals = 0;
    GLuint vboTexcoords = 0;
    GLuint vboTangents = 0;

    glGenBuffers(1, &vboPoints);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glBufferData(GL_ARRAY_BUFFER, ground.pCount * 3 * sizeof(GLfloat), points, GL_STATIC_DRAW);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, ground.pCount * 3 * sizeof(GLfloat), normals, GL_STATIC_DRAW);

    glGenBuffers(1, &vboTexcoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glBufferData(GL_ARRAY_BUFFER, ground.pCount * 3 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);

    glGenBuffers(1, &vboTangents);
    glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
    glBufferData(GL_ARRAY_BUFFER, ground.pCount * 4 * sizeof(GLfloat), tangents, GL_STATIC_DRAW);

    glGenVertexArrays(1, &ground.vao);
    glBindVertexArray(ground.vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    /// Load Textures ///
    glActiveTexture(GL_TEXTURE0);
    sf::Image image;

    // Load Normal Textures
    glGenTextures(1, &ground.texNormArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ground.texNormArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, 64, 64, texCount, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < texCount ; i++) {
        if (!image.loadFromFile("res/textures/ground/"+texPaths[i]+"_norm.png"))
            image.loadFromFile("res/misc/blanknorm.png");
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 64, 64, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    }

    // Load Diffuse Textures
    glGenTextures(1, &ground.texDiffArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ground.texDiffArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, 64, 64, texCount, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < texCount ; i++) {
        if (!image.loadFromFile("res/textures/ground/"+texPaths[i]+"_diff.png"))
            image.loadFromFile("res/misc/blankdiff.png");
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 64, 64, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    }

    // Load Specular Textures
    glGenTextures(1, &ground.texSpecArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ground.texSpecArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, 64, 64, texCount, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < texCount ; i++) {
        if (!image.loadFromFile("res/textures/ground/"+texPaths[i]+"_spec.png"))
            image.loadFromFile("res/misc/blankspec.png");
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 64, 64, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    }

    return false;
}

bool LevelMap::load_models() {
    Json::Value root = sq::load_json_file(mapDirPath+"/models.json");

    int modelCount = root["models"].size();

    for (int i = 0; i < modelCount; i++) {
        modelVec.push_back(sq::Mesh());
        modelVec.back().load_from_dir("res/models/"+root["models"][i].asString());
    }

    for (Json::Value& val : root["mapModels"]) {
        for (Json::Value& loc : val["locations"]) {
            mapModelVec.push_back(
                ModelInstance(glm::vec3(loc[0].asFloat(),
                                        loc[1].asFloat(),
                                        loc[2].asFloat()),
                              glm::vec3(val["scale"][0].asFloat(),
                                        val["scale"][1].asFloat(),
                                        val["scale"][2].asFloat()),
                              val["shadow"].asBool(),
                              val["index"].asInt()));
        }
    }

    return false;
}

}

#undef CHECK_BIT
