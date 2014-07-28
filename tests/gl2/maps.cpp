#include "maps.hpp"

#include <bitset>

#include <libsqee/extra/helpers.hpp>

#define CHECK_BIT(var, pos) bool((var) & (1<<(pos)))
#define UNUSED {0,0}

namespace sqt {

ModelInstance::ModelInstance(glm::vec3 _pos, glm::vec3 _scale, int _index) {
    pos = _pos;
    scale = _scale;
    index = _index;
    modelMatrix = glm::translate(glm::mat4(), pos) * glm::scale(glm::mat4(), scale);
}

bool LevelMap::load_map(std::string dirPath) {
    mapDirPath = dirPath;
    // Put some kind of checks here
    return false;
}

bool LevelMap::load_ground() {
    Json::Value root = sqe::load_json_file(mapDirPath+"/ground.json");

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

    ground.pCount = width * height * 6;
    float points[width * height * 18];
    float normals[width * height * 18];
    int texcoords[width * height * 18];

    float angleNormals[9][2][2][2] = {
        {{UNUSED,        {0.f, 0.25f}},  {UNUSED,        {0.f, 0.25f}}},   // 0
        {{{0.f, 0.25f},  {0.25f, 0.f}},  {{0.25f, 0.f},  {0.f, 0.25f}}},   // 1
        {{UNUSED,        {0.25f, 0.f}},  {UNUSED,        {0.25f, 0.f}}},   // 2
        {{{0.25f, 0.f},  {0.f, -0.25f}}, {{0.f, -0.25f}, {0.25f, 0.f}}},   // 3
        {{UNUSED,        {0.f, -0.25f}}, {UNUSED,        {0.f, -0.25f}}},  // 4
        {{{0.f, -0.25f}, {-0.25f, 0.f}}, {{-0.25f, 0.f}, {0.f, -0.25f}}},  // 5
        {{UNUSED,        {-0.25f, 0.f}}, {UNUSED,        {-0.25f, 0.f}}},  // 6
        {{{-0.25f, 0.f}, {0.f, 0.25f}},  {{0.f, 0.25f},  {-0.25f, 0.f}}},  // 7
        {{{0.f, 0.f},    {0.f, 0.f}},    {{0.f, 0.f},    {0.f, 0.f}}}      // 8
    };

    bool nAngleInd[9] = {0, 0, 0, 0, 0, 1, 0, 1, 0};
    bool eAngleInd[9] = {0, 1, 0, 0, 0, 0, 0, 1, 0};
    bool sAngleInd[9] = {0, 1, 0, 1, 0, 0, 0, 0, 0};
    bool wAngleInd[9] = {0, 0, 0, 1, 0, 1, 0, 0, 0};

    // Working Arrays
    int zBases[width * height];
    float zNormals[width * height];
    bool corners[width * height];
    bool v0s[width * height];
    bool v1s[width * height];
    bool v2s[width * height];
    bool inclines[width * height];
    bool smooths[width * height];
    int angles[width * height];

    // Fill Working Arrays and set textures
    for (int y_ = 0; y_ < height; y_++) {
        int y = height - y_ - 1;
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            int val = tilesGeometry[width*y_ + x];

            zBases[pos]      = (val >> 8) % 128;
            corners[pos]     = CHECK_BIT(val, 5);
            v0s[pos]         = CHECK_BIT(val, 4);
            v1s[pos]         = CHECK_BIT(val, 3);
            v2s[pos]         = CHECK_BIT(val, 2);
            inclines[pos]    = CHECK_BIT(val, 1);
            smooths[pos]     = CHECK_BIT(val, 0);

            if (corners[pos]) {
                if      (!v1s[pos] && !v2s[pos]) angles[pos] = 1;
                else if (!v1s[pos] &&  v2s[pos]) angles[pos] = 3;
                else if ( v1s[pos] &&  v2s[pos]) angles[pos] = 5;
                else if ( v1s[pos] && !v2s[pos]) angles[pos] = 7;
            } else if (v0s[pos]) {
                if      ( v1s[pos] && !v2s[pos]) angles[pos] = 0;
                else if (!v1s[pos] && !v2s[pos]) angles[pos] = 2;
                else if ( v1s[pos] &&  v2s[pos]) angles[pos] = 4;
                else if (!v1s[pos] &&  v2s[pos]) angles[pos] = 6;
            } else angles[pos] = 8;

            texcoords[pos*18+2] = tilesTextures[width*y_ + x];
            texcoords[pos*18+5] = tilesTextures[width*y_ + x];
            texcoords[pos*18+8] = tilesTextures[width*y_ + x];
            texcoords[pos*18+11] = tilesTextures[width*y_ + x];
            texcoords[pos*18+14] = tilesTextures[width*y_ + x];
            texcoords[pos*18+17] = tilesTextures[width*y_ + x];
        }
    }

    // Calculate Points and Base Z Normals
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            float left   = x;
            float right  = x+1;
            float top    = y+1;
            float bottom = y;

            bool corner = corners[pos];
            bool incline = inclines[pos];
            int angle = angles[pos];

            // Set X and Y coords, and set texcoords
            if (angle == 1) {
                points[pos*18+0] = left;
                points[pos*18+1] = bottom;
                points[pos*18+3] = left;
                points[pos*18+4] = top;
                points[pos*18+6] = right;
                points[pos*18+7] = top;

                points[pos*18+9] = left;
                points[pos*18+10] = bottom;
                points[pos*18+12] = right;
                points[pos*18+13] = bottom;
                points[pos*18+15] = right;
                points[pos*18+16] = top;

                texcoords[pos*18+0] = 0;
                texcoords[pos*18+1] = 1;
                texcoords[pos*18+3] = 0;
                texcoords[pos*18+4] = 0;
                texcoords[pos*18+6] = 1;
                texcoords[pos*18+7] = 0;

                texcoords[pos*18+9] = 0;
                texcoords[pos*18+10] = 1;
                texcoords[pos*18+12] = 1;
                texcoords[pos*18+13] = 1;
                texcoords[pos*18+15] = 1;
                texcoords[pos*18+16] = 0;
            } else
            if (angle == 3) {
                points[pos*18+0] = left;
                points[pos*18+1] = top;
                points[pos*18+3] = right;
                points[pos*18+4] = top;
                points[pos*18+6] = right;
                points[pos*18+7] = bottom;

                points[pos*18+9] = left;
                points[pos*18+10] = top;
                points[pos*18+12] = left;
                points[pos*18+13] = bottom;
                points[pos*18+15] = right;
                points[pos*18+16] = bottom;

                texcoords[pos*18+0] = 0;
                texcoords[pos*18+1] = 0;
                texcoords[pos*18+3] = 1;
                texcoords[pos*18+4] = 0;
                texcoords[pos*18+6] = 1;
                texcoords[pos*18+7] = 1;

                texcoords[pos*18+9] = 0;
                texcoords[pos*18+10] = 0;
                texcoords[pos*18+12] = 0;
                texcoords[pos*18+13] = 1;
                texcoords[pos*18+15] = 1;
                texcoords[pos*18+16] = 1;
            } else
            if (angle == 5) {
                points[pos*18+0] = right;
                points[pos*18+1] = top;
                points[pos*18+3] = right;
                points[pos*18+4] = bottom;
                points[pos*18+6] = left;
                points[pos*18+7] = bottom;

                points[pos*18+9] = right;
                points[pos*18+10] = top;
                points[pos*18+12] = left;
                points[pos*18+13] = top;
                points[pos*18+15] = left;
                points[pos*18+16] = bottom;

                texcoords[pos*18+0] = 1;
                texcoords[pos*18+1] = 0;
                texcoords[pos*18+3] = 1;
                texcoords[pos*18+4] = 1;
                texcoords[pos*18+6] = 0;
                texcoords[pos*18+7] = 1;

                texcoords[pos*18+9] = 1;
                texcoords[pos*18+10] = 0;
                texcoords[pos*18+12] = 0;
                texcoords[pos*18+13] = 0;
                texcoords[pos*18+15] = 0;
                texcoords[pos*18+16] = 1;
            } else
            if (angle == 7) {
                points[pos*18+0] = right;
                points[pos*18+1] = bottom;
                points[pos*18+3] = left;
                points[pos*18+4] = bottom;
                points[pos*18+6] = left;
                points[pos*18+7] = top;

                points[pos*18+9] = right;
                points[pos*18+10] = bottom;
                points[pos*18+12] = right;
                points[pos*18+13] = top;
                points[pos*18+15] = left;
                points[pos*18+16] = top;

                texcoords[pos*18+0] = 1;
                texcoords[pos*18+1] = 1;
                texcoords[pos*18+3] = 0;
                texcoords[pos*18+4] = 1;
                texcoords[pos*18+6] = 0;
                texcoords[pos*18+7] = 0;

                texcoords[pos*18+9] = 1;
                texcoords[pos*18+10] = 1;
                texcoords[pos*18+12] = 1;
                texcoords[pos*18+13] = 0;
                texcoords[pos*18+15] = 0;
                texcoords[pos*18+16] = 0;
            } else {
                points[pos*18+0] = left;
                points[pos*18+1] = bottom;
                points[pos*18+3] = right;
                points[pos*18+4] = bottom;
                points[pos*18+6] = right;
                points[pos*18+7] = top;

                points[pos*18+9] = left;
                points[pos*18+10] = bottom;
                points[pos*18+12] = left;
                points[pos*18+13] = top;
                points[pos*18+15] = right;
                points[pos*18+16] = top;

                texcoords[pos*18+0] = 0;
                texcoords[pos*18+1] = 1;
                texcoords[pos*18+3] = 1;
                texcoords[pos*18+4] = 1;
                texcoords[pos*18+6] = 1;
                texcoords[pos*18+7] = 0;

                texcoords[pos*18+9] = 0;
                texcoords[pos*18+10] = 1;
                texcoords[pos*18+12] = 0;
                texcoords[pos*18+13] = 0;
                texcoords[pos*18+15] = 1;
                texcoords[pos*18+16] = 0;
            };

            // Set Z coords
            for (int p = 0; p < 6; p++) {
                int zBase = zBases[pos];
                if (corner) {
                    if (p==0 || p==3 || (v0s[pos] && (p==1 || p==4))) {
                        zBase += 1 + incline;
                    }
                } else if ((angle == 0 && (p==0 || p==1 || p==3)) ||
                           (angle == 2 && (p==0 || p==3 || p==4)) ||
                           (angle == 4 && (p==2 || p==4 || p==5)) ||
                           (angle == 6 && (p==1 || p==2 || p==5))) {
                    zBase += 1 + incline;
                }
                points[pos*18 + p*3 +2] = float(zBase) / 2.f;
            }

            if (corners[pos] || (!corners[pos] && v0s[pos])) {
                if (incline) zNormals[pos] = 0.5f;
                else         zNormals[pos] = 0.75f;
            } else zNormals[pos] = 1.f;
        }
    }

    // Calculate Normals
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;

            bool v0 = v0s[pos];
            bool incline = inclines[pos];
            bool smooth = smooths[pos];
            int angle = angles[pos];

            // Copy Flat Normals
            for (int p = 0; p < 6; p++) {
                normals[pos*18 + p*3 +0] = angleNormals [angle] [v0s[pos]] [1*(p>2)] [0] * (1+incline);
                normals[pos*18 + p*3 +1] = angleNormals [angle] [v0s[pos]] [1*(p>2)] [1] * (1+incline);
                normals[pos*18 + p*3 +2] = zNormals[pos];
            }

            // No Smoothing
            if (!smooth) continue;

            // Set up just a few variables
            int nPos = pos + width;
            int ePos = pos + 1;
            int sPos = pos - width;
            int wPos = pos - 1;
            int nePos = pos + width + 1;
            int esPos = pos + 1 - width;
            int swPos = pos - width - 1;
            int wnPos = pos - 1 + width;
            bool nV0 = v0s[nPos];
            bool eV0 = v0s[ePos];
            bool sV0 = v0s[sPos];
            bool wV0 = v0s[wPos];
            bool neV0 = v0s[nePos];
            bool esV0 = v0s[esPos];
            bool swV0 = v0s[swPos];
            bool wnV0 = v0s[wnPos];
            int nAngle = angles[nPos];
            int eAngle = angles[ePos];
            int sAngle = angles[sPos];
            int wAngle = angles[wPos];
            int neAngle = angles[nePos];
            int esAngle = angles[esPos];
            int swAngle = angles[swPos];
            int wnAngle = angles[wnPos];
            bool nIncline = inclines[nPos];
            bool eIncline = inclines[ePos];
            bool sIncline = inclines[sPos];
            bool wIncline = inclines[wPos];
            bool neIncline = inclines[nePos];
            bool esIncline = inclines[esPos];
            bool swIncline = inclines[swPos];
            bool wnIncline = inclines[wnPos];
            bool nSmooth, eSmooth, sSmooth, wSmooth;
            glm::vec2 nNorms;
            glm::vec2 eNorms;
            glm::vec2 sNorms;
            glm::vec2 wNorms;
            std::vector<glm::vec3> neNorms; neNorms.resize(8);
            std::vector<glm::vec3> esNorms; esNorms.resize(8);
            std::vector<glm::vec3> swNorms; swNorms.resize(8);
            std::vector<glm::vec3> wnNorms; wnNorms.resize(8);

            // check edges
            if (y == height-1) nSmooth = false;
            else               nSmooth = true;
            if (x == width-1)  eSmooth = false;
            else               eSmooth = true;
            if (y == 0)        sSmooth = false;
            else               sSmooth = true;
            if (x == 0)        wSmooth = false;
            else               wSmooth = true;

            if (!(nSmooth || eSmooth || sSmooth || wSmooth)) continue;

            if (nSmooth) {
                nNorms.x = angleNormals [nAngle] [nV0] [sAngleInd[nAngle]] [0] * (1+nIncline);
                nNorms.y = angleNormals [nAngle] [nV0] [sAngleInd[nAngle]] [1] * (1+nIncline);
            }
            if (eSmooth) {
                eNorms.x = angleNormals [eAngle] [eV0] [wAngleInd[eAngle]] [0] * (1+eIncline);
                eNorms.y = angleNormals [eAngle] [eV0] [wAngleInd[eAngle]] [1] * (1+eIncline);
            }
            if (sSmooth) {
                sNorms.x = angleNormals [sAngle] [sV0] [nAngleInd[sAngle]] [0] * (1+sIncline);
                sNorms.y = angleNormals [sAngle] [sV0] [nAngleInd[sAngle]] [1] * (1+sIncline);
            }
            if (wSmooth) {
                wNorms.x = angleNormals [wAngle] [wV0] [eAngleInd[wAngle]] [0] * (1+wIncline);
                wNorms.y = angleNormals [wAngle] [wV0] [eAngleInd[wAngle]] [1] * (1+wIncline);
            }

            if (nSmooth && eSmooth) {
                neNorms[0] = glm::vec3(angleNormals [angle] [v0] [0] [0] * (1+incline),
                                       angleNormals [angle] [v0] [0] [1] * (1+incline),
                                       zNormals [pos]);
                neNorms[1] = glm::vec3(angleNormals [angle] [v0] [1] [0] * (1+incline),
                                       angleNormals [angle] [v0] [1] [1] * (1+incline),
                                       zNormals [pos]);
                neNorms[2] = glm::vec3(angleNormals [nAngle] [nV0] [eAngleInd[nAngle]] [0] * (1+nIncline),
                                       angleNormals [nAngle] [nV0] [eAngleInd[nAngle]] [1] * (1+nIncline),
                                       zNormals [nPos]);
                neNorms[3] = glm::vec3(angleNormals [nAngle] [nV0] [sAngleInd[nAngle]] [0] * (1+nIncline),
                                       angleNormals [nAngle] [nV0] [sAngleInd[nAngle]] [1] * (1+nIncline),
                                       zNormals [nPos]);
                neNorms[4] = glm::vec3(angleNormals [neAngle] [neV0] [sAngleInd[neAngle]] [0] * (1+neIncline),
                                       angleNormals [neAngle] [neV0] [sAngleInd[neAngle]] [1] * (1+neIncline),
                                       zNormals [nePos]);
                neNorms[5] = glm::vec3(angleNormals [neAngle] [neV0] [wAngleInd[neAngle]] [0] * (1+neIncline),
                                       angleNormals [neAngle] [neV0] [wAngleInd[neAngle]] [1] * (1+neIncline),
                                       zNormals [nePos]);
                neNorms[6] = glm::vec3(angleNormals [eAngle] [eV0] [wAngleInd[eAngle]] [0] * (1+eIncline),
                                       angleNormals [eAngle] [eV0] [wAngleInd[eAngle]] [1] * (1+eIncline),
                                       zNormals [ePos]);
                neNorms[7] = glm::vec3(angleNormals [eAngle] [eV0] [nAngleInd[eAngle]] [0] * (1+eIncline),
                                       angleNormals [eAngle] [eV0] [nAngleInd[eAngle]] [1] * (1+eIncline),
                                       zNormals [ePos]);
            }

            if (eSmooth && sSmooth) {
                esNorms[0] = glm::vec3(angleNormals [angle] [v0] [0] [0] * (1+incline),
                                       angleNormals [angle] [v0] [0] [1] * (1+incline),
                                       zNormals [pos]);
                esNorms[1] = glm::vec3(angleNormals [angle] [v0] [1] [0] * (1+incline),
                                       angleNormals [angle] [v0] [1] [1] * (1+incline),
                                       zNormals [pos]);
                esNorms[2] = glm::vec3(angleNormals [eAngle] [eV0] [sAngleInd[eAngle]] [0] * (1+eIncline),
                                       angleNormals [eAngle] [eV0] [sAngleInd[eAngle]] [1] * (1+eIncline),
                                       zNormals [ePos]);
                esNorms[3] = glm::vec3(angleNormals [eAngle] [eV0] [wAngleInd[eAngle]] [0] * (1+eIncline),
                                       angleNormals [eAngle] [eV0] [wAngleInd[eAngle]] [1] * (1+eIncline),
                                       zNormals [ePos]);
                esNorms[4] = glm::vec3(angleNormals [esAngle] [esV0] [wAngleInd[esAngle]] [0] * (1+esIncline),
                                       angleNormals [esAngle] [esV0] [wAngleInd[esAngle]] [1] * (1+esIncline),
                                       zNormals [esPos]);
                esNorms[5] = glm::vec3(angleNormals [esAngle] [esV0] [nAngleInd[esAngle]] [0] * (1+esIncline),
                                       angleNormals [esAngle] [esV0] [nAngleInd[esAngle]] [1] * (1+esIncline),
                                       zNormals [esPos]);
                esNorms[6] = glm::vec3(angleNormals [sAngle] [sV0] [nAngleInd[sAngle]] [0] * (1+sIncline),
                                       angleNormals [sAngle] [sV0] [nAngleInd[sAngle]] [1] * (1+sIncline),
                                       zNormals [sPos]);
                esNorms[7] = glm::vec3(angleNormals [sAngle] [sV0] [eAngleInd[sAngle]] [0] * (1+sIncline),
                                       angleNormals [sAngle] [sV0] [eAngleInd[sAngle]] [1] * (1+sIncline),
                                       zNormals [sPos]);
            }

            if (sSmooth && wSmooth) {
                swNorms[0] = glm::vec3(angleNormals [angle] [v0] [0] [0] * (1+incline),
                                       angleNormals [angle] [v0] [0] [1] * (1+incline),
                                       zNormals [pos]);
                swNorms[1] = glm::vec3(angleNormals [angle] [v0] [1] [0] * (1+incline),
                                       angleNormals [angle] [v0] [1] [1] * (1+incline),
                                       zNormals [pos]);
                swNorms[2] = glm::vec3(angleNormals [sAngle] [sV0] [wAngleInd[sAngle]] [0] * (1+sIncline),
                                       angleNormals [sAngle] [sV0] [wAngleInd[sAngle]] [1] * (1+sIncline),
                                       zNormals [sPos]);
                swNorms[3] = glm::vec3(angleNormals [sAngle] [sV0] [nAngleInd[sAngle]] [0] * (1+sIncline),
                                       angleNormals [sAngle] [sV0] [nAngleInd[sAngle]] [1] * (1+sIncline),
                                       zNormals [sPos]);
                swNorms[4] = glm::vec3(angleNormals [swAngle] [swV0] [nAngleInd[swAngle]] [0] * (1+swIncline),
                                       angleNormals [swAngle] [swV0] [nAngleInd[swAngle]] [1] * (1+swIncline),
                                       zNormals [swPos]);
                swNorms[5] = glm::vec3(angleNormals [swAngle] [swV0] [eAngleInd[swAngle]] [0] * (1+swIncline),
                                       angleNormals [swAngle] [swV0] [eAngleInd[swAngle]] [1] * (1+swIncline),
                                       zNormals [swPos]);
                swNorms[6] = glm::vec3(angleNormals [wAngle] [wV0] [eAngleInd[wAngle]] [0] * (1+wIncline),
                                       angleNormals [wAngle] [wV0] [eAngleInd[wAngle]] [1] * (1+wIncline),
                                       zNormals [wPos]);
                swNorms[7] = glm::vec3(angleNormals [wAngle] [wV0] [sAngleInd[wAngle]] [0] * (1+wIncline),
                                       angleNormals [wAngle] [wV0] [sAngleInd[wAngle]] [1] * (1+wIncline),
                                       zNormals [wPos]);
            }

            if (wSmooth && nSmooth) {
                wnNorms[0] = glm::vec3(angleNormals [angle] [v0] [0] [0] * (1+incline),
                                       angleNormals [angle] [v0] [0] [1] * (1+incline),
                                       zNormals [pos]);
                wnNorms[1] = glm::vec3(angleNormals [angle] [v0] [1] [0] * (1+incline),
                                       angleNormals [angle] [v0] [1] [1] * (1+incline),
                                       zNormals [pos]);
                wnNorms[2] = glm::vec3(angleNormals [wAngle] [wV0] [nAngleInd[wAngle]] [0] * (1+wIncline),
                                       angleNormals [wAngle] [wV0] [nAngleInd[wAngle]] [1] * (1+wIncline),
                                       zNormals [wPos]);
                wnNorms[3] = glm::vec3(angleNormals [wAngle] [wV0] [eAngleInd[wAngle]] [0] * (1+wIncline),
                                       angleNormals [wAngle] [wV0] [eAngleInd[wAngle]] [1] * (1+wIncline),
                                       zNormals [wPos]);
                wnNorms[4] = glm::vec3(angleNormals [wnAngle] [wnV0] [eAngleInd[wnAngle]] [0] * (1+wnIncline),
                                       angleNormals [wnAngle] [wnV0] [eAngleInd[wnAngle]] [1] * (1+wnIncline),
                                       zNormals [wnPos]);
                wnNorms[5] = glm::vec3(angleNormals [wnAngle] [wnV0] [sAngleInd[wnAngle]] [0] * (1+wnIncline),
                                       angleNormals [wnAngle] [wnV0] [sAngleInd[wnAngle]] [1] * (1+wnIncline),
                                       zNormals [wnPos]);
                wnNorms[6] = glm::vec3(angleNormals [nAngle] [nV0] [sAngleInd[nAngle]] [0] * (1+nIncline),
                                       angleNormals [nAngle] [nV0] [sAngleInd[nAngle]] [1] * (1+nIncline),
                                       zNormals [nPos]);
                wnNorms[7] = glm::vec3(angleNormals [nAngle] [nV0] [wAngleInd[nAngle]] [0] * (1+nIncline),
                                       angleNormals [nAngle] [nV0] [wAngleInd[nAngle]] [1] * (1+nIncline),
                                       zNormals [nPos]);
            }

            glm::vec3 neSum;
            for (glm::vec3& norm : neNorms) {
                neSum += norm;
            }
            neSum = glm::normalize(neSum);

            glm::vec3 esSum;
            for (glm::vec3& norm : esNorms) {
                esSum += norm;
            }
            esSum = glm::normalize(esSum);

            glm::vec3 swSum;
            for (glm::vec3& norm : swNorms) {
                swSum += norm;
            }
            swSum = glm::normalize(swSum);

            glm::vec3 wnSum;
            for (glm::vec3& norm : wnNorms) {
                wnSum += norm;
            }
            wnSum = glm::normalize(wnSum);

            if (nSmooth && eSmooth) {
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    normals[pos*18+6+0] = neSum.x;
                    normals[pos*18+6+1] = neSum.y;
                    normals[pos*18+6+2] = neSum.z;
                    normals[pos*18+15+0] = neSum.x;
                    normals[pos*18+15+1] = neSum.y;
                    normals[pos*18+15+2] = neSum.z;
                } else
                if (angle == 1) {
                    normals[pos*18+6+0] = neSum.x;
                    normals[pos*18+6+1] = neSum.y;
                    normals[pos*18+6+2] = neSum.z;
                    normals[pos*18+15+0] = neSum.x;
                    normals[pos*18+15+1] = neSum.y;
                    normals[pos*18+15+2] = neSum.z;
                } else
                if (angle == 3) {
                    normals[pos*18+3+0] = neSum.x;
                    normals[pos*18+3+1] = neSum.y;
                    normals[pos*18+3+2] = neSum.z;
                } else
                if (angle == 5) {
                    normals[pos*18+0+0] = neSum.x;
                    normals[pos*18+0+1] = neSum.y;
                    normals[pos*18+0+2] = neSum.z;
                    normals[pos*18+9+0] = neSum.x;
                    normals[pos*18+9+1] = neSum.y;
                    normals[pos*18+9+2] = neSum.z;
                } else
                if (angle == 7) {
                    normals[pos*18+12+0] = neSum.x;
                    normals[pos*18+12+1] = neSum.y;
                    normals[pos*18+12+2] = neSum.z;
                }
            }

            if (eSmooth && sSmooth) {
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    normals[pos*18+3+0] = esSum.x;
                    normals[pos*18+3+1] = esSum.y;
                    normals[pos*18+3+2] = esSum.z;
                } else
                if (angle == 1) {
                    normals[pos*18+12+0] = esSum.x;
                    normals[pos*18+12+1] = esSum.y;
                    normals[pos*18+12+2] = esSum.z;
                } else
                if (angle == 3) {
                    normals[pos*18+6+0] = esSum.x;
                    normals[pos*18+6+1] = esSum.y;
                    normals[pos*18+6+2] = esSum.z;
                    normals[pos*18+15+0] = esSum.x;
                    normals[pos*18+15+1] = esSum.y;
                    normals[pos*18+15+2] = esSum.z;
                } else
                if (angle == 5) {
                    normals[pos*18+3+0] = esSum.x;
                    normals[pos*18+3+1] = esSum.y;
                    normals[pos*18+3+2] = esSum.z;
                } else
                if (angle == 7) {
                    normals[pos*18+0+0] = esSum.x;
                    normals[pos*18+0+1] = esSum.y;
                    normals[pos*18+0+2] = esSum.z;
                    normals[pos*18+9+0] = esSum.x;
                    normals[pos*18+9+1] = esSum.y;
                    normals[pos*18+9+2] = esSum.z;
                }
            }

            if (sSmooth && wSmooth) {
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    normals[pos*18+0+0] = swSum.x;
                    normals[pos*18+0+1] = swSum.y;
                    normals[pos*18+0+2] = swSum.z;
                    normals[pos*18+9+0] = swSum.x;
                    normals[pos*18+9+1] = swSum.y;
                    normals[pos*18+9+2] = swSum.z;
                } else
                if (angle == 1) {
                    normals[pos*18+0+0] = swSum.x;
                    normals[pos*18+0+1] = swSum.y;
                    normals[pos*18+0+2] = swSum.z;
                    normals[pos*18+9+0] = swSum.x;
                    normals[pos*18+9+1] = swSum.y;
                    normals[pos*18+9+2] = swSum.z;
                } else
                if (angle == 3) {
                    normals[pos*18+12+0] = swSum.x;
                    normals[pos*18+12+1] = swSum.y;
                    normals[pos*18+12+2] = swSum.z;
                } else
                if (angle == 5) {
                    normals[pos*18+6+0] = swSum.x;
                    normals[pos*18+6+1] = swSum.y;
                    normals[pos*18+6+2] = swSum.z;
                    normals[pos*18+15+0] = swSum.x;
                    normals[pos*18+15+1] = swSum.y;
                    normals[pos*18+15+2] = swSum.z;
                } else
                if (angle == 7) {
                    normals[pos*18+3+0] = swSum.x;
                    normals[pos*18+3+1] = swSum.y;
                    normals[pos*18+3+2] = swSum.z;
                }
            }

            if (wSmooth && nSmooth) {
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    normals[pos*18+12+0] = wnSum.x;
                    normals[pos*18+12+1] = wnSum.y;
                    normals[pos*18+12+2] = wnSum.z;
                } else
                if (angle == 1) {
                    normals[pos*18+3+0] = wnSum.x;
                    normals[pos*18+3+1] = wnSum.y;
                    normals[pos*18+3+2] = wnSum.z;
                } else
                if (angle == 3) {
                    normals[pos*18+0+0] = wnSum.x;
                    normals[pos*18+0+1] = wnSum.y;
                    normals[pos*18+0+2] = wnSum.z;
                    normals[pos*18+9+0] = wnSum.x;
                    normals[pos*18+9+1] = wnSum.y;
                    normals[pos*18+9+2] = wnSum.z;
                } else
                if (angle == 5) {
                    normals[pos*18+12+0] = wnSum.x;
                    normals[pos*18+12+1] = wnSum.y;
                    normals[pos*18+12+2] = wnSum.z;
                } else
                if (angle == 7) {
                    normals[pos*18+6+0] = wnSum.x;
                    normals[pos*18+6+1] = wnSum.y;
                    normals[pos*18+6+2] = wnSum.z;
                    normals[pos*18+15+0] = wnSum.x;
                    normals[pos*18+15+1] = wnSum.y;
                    normals[pos*18+15+2] = wnSum.z;
                }
            }
        }
    }

    GLuint vboPoints = 0;
    GLuint vboNormals = 0;
    GLuint vboTexcoords = 0;

    glGenBuffers(1, &vboPoints);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    glGenBuffers(1, &vboTexcoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    glGenVertexArrays(1, &ground.vao);
    glBindVertexArray(ground.vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glVertexAttribPointer(2, 3, GL_INT, GL_FALSE, 0, NULL);

    // Load Textures
    sf::Image image;
    glGenTextures(1, &ground.texArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ground.texArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 64, 64, texCount, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < texCount ; i++) {
        image.loadFromFile("res/textures/ground/"+texPaths[i]);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 64, 64, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    }

    // Load Normal Maps
    glGenTextures(1, &ground.nMapArray);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ground.nMapArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 64, 64, texCount, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    for (int i = 0; i < texCount ; i++) {
        if (!image.loadFromFile("res/normalmaps/ground/"+texPaths[i]))
            image.loadFromFile("res/normalmaps/ground/blank.png");
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 64, 64, 1,
                        GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
    }

    return false;
}

bool LevelMap::load_models() {
    Json::Value root = sqe::load_json_file(mapDirPath+"/models.json");

    int modelCount = root["models"].size();

    for (int i = 0; i < modelCount; i++) {
        modelVec.push_back(StaticModel());
        sqe::load_mesh("res/models/"+root["models"][i].asString()+"/model.dae", modelVec.back().vao, modelVec.back().pCount);
    }

    for (Json::Value& valA : root["mapModels"]) {
        for (Json::Value& valB : valA["locations"]) {
            mapModelVec.push_back(
                ModelInstance(glm::vec3(valB[0].asFloat(),
                                        valB[1].asFloat(),
                                        valB[2].asFloat()),
                              glm::vec3(valA["scale"][0].asFloat(),
                                        valA["scale"][1].asFloat(),
                                        valA["scale"][2].asFloat()),
                              valA["index"].asInt()));
        }
    }

    return false;
}

}
