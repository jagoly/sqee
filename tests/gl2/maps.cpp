#include "maps.hpp"

#include <libsqee/extra/helpers.hpp>

#define CHECK_BIT(_v, _p) bool((_v) & (1<<(_p)))
#define GET_NORM(_t, _n, _a) angleNormals [_t##Angle] [_t##V0] [_n##AngleInd[_t##Angle]] [_a] * (1+_t##Incline)
#define GET_OWN_NORM(_n, _a) angleNormals [angle] [v0] [_n##AngleInd[angle]] [_a] * (1+incline)
#define SET_NORMAL(_p, _v, _n) \
    normals[(_p) * 18 + ((_v) * 3) + 0] = _n.x; \
    normals[(_p) * 18 + ((_v) * 3) + 1] = _n.y; \
    normals[(_p) * 18 + ((_v) * 3) + 2] = _n.z;
#define SET_TANGENT(_p, _v, _t) \
    tangents[_p * 24 + (_v * 4) + 0] = _t.x;\
    tangents[_p * 24 + (_v * 4) + 1] = _t.y;\
    tangents[_p * 24 + (_v * 4) + 2] = _t.z;\
    tangents[_p * 24 + (_v * 4) + 3] = _t.w;

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

    ground.pCount = width * height * 6;
    float points[width * height * 18];
    float normals[width * height * 18];
    int texcoords[width * height * 18];
    float tangents[width * height * 24];

    float angleNormals[9][2][2][2] = {
        {{{0.f, 0.25f},  {0.f, 0.25f}},  {{0.f, 0.25f},  {0.f, 0.25f}}},   // 0
        {{{0.f, 0.25f},  {0.25f, 0.f}},  {{0.25f, 0.f},  {0.f, 0.25f}}},   // 1
        {{{0.25f, 0.f},  {0.25f, 0.f}},  {{0.25f, 0.f},  {0.25f, 0.f}}},   // 2
        {{{0.25f, 0.f},  {0.f, -0.25f}}, {{0.f, -0.25f}, {0.25f, 0.f}}},   // 3
        {{{0.f, -0.25f}, {0.f, -0.25f}}, {{0.f, -0.25f}, {0.f, -0.25f}}},  // 4
        {{{0.f, -0.25f}, {-0.25f, 0.f}}, {{-0.25f, 0.f}, {0.f, -0.25f}}},  // 5
        {{{-0.25f, 0.f}, {-0.25f, 0.f}}, {{-0.25f, 0.f}, {-0.25f, 0.f}}},  // 6
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
    bool invisibles[width * height];
    bool corners[width * height];
    bool v0s[width * height];
    bool v1s[width * height];
    bool v2s[width * height];
    bool inclines[width * height];
    bool smooths[width * height];
    int angles[width * height];

    // Fill working arrays and set textures (Z texcoord)
    for (int y_ = 0; y_ < height; y_++) {
        int y = height - y_ - 1;
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            int val = tilesGeometry[width*y_ + x];

            // Get all properties from tile's 16bit int
            zBases[pos]      = (val >> 8) % 128;
            invisibles[pos]  = CHECK_BIT(val, 6);
            corners[pos]     = CHECK_BIT(val, 5);
            v0s[pos]         = CHECK_BIT(val, 4);
            v1s[pos]         = CHECK_BIT(val, 3);
            v2s[pos]         = CHECK_BIT(val, 2);
            inclines[pos]    = CHECK_BIT(val, 1);
            smooths[pos]     = CHECK_BIT(val, 0);

            // Work out angles for each tile
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

    // Calculate points and base Z normals
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            float left   = x;
            float right  = x+1;
            float top    = y+1;
            float bottom = y;

            bool corner = corners[pos];
            bool v0 = v0s[pos];
            bool incline = inclines[pos];
            int angle = angles[pos];

            // Set X and Y coords and set texcoords
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
                    if (p==0 || p==3 || (v0 && (p==1 || p==4))) {
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

            // Set flat Z normals
            if (corner || (!corner && v0)) {
                if (incline) zNormals[pos] = 0.5f;
                else         zNormals[pos] = 0.75f;
            } else zNormals[pos] = 1.f;
        }
    }

    // Calculate smooth normals and set normals
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;

            bool v0 = v0s[pos];
            bool incline = inclines[pos];
            bool smooth = smooths[pos];
            int angle = angles[pos];

            // Copy flat normals and set tangents
            for (int p = 0; p < 6; p++) {
                normals[pos*18 + p*3 +0] = angleNormals [angle] [v0] [1*(p>2)] [0] * (1+incline);
                normals[pos*18 + p*3 +1] = angleNormals [angle] [v0] [1*(p>2)] [1] * (1+incline);
                normals[pos*18 + p*3 +2] = zNormals[pos];

                glm::vec3 normal(normals[pos*18 + p*3 +0],
                                 normals[pos*18 + p*3 +1],
                                 normals[pos*18 + p*3 +2]);
                glm::vec4 tangent = sq::get_tangent(normal);

                tangents[pos*24 + p*4 +0] = tangent.x;
                tangents[pos*24 + p*4 +1] = tangent.y;
                tangents[pos*24 + p*4 +2] = tangent.z;
                tangents[pos*24 + p*4 +3] = tangent.w;
            }

            // No smoothing
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
            std::vector<glm::vec3> neNorms; neNorms.resize(8);
            std::vector<glm::vec3> esNorms; esNorms.resize(8);
            std::vector<glm::vec3> swNorms; swNorms.resize(8);
            std::vector<glm::vec3> wnNorms; wnNorms.resize(8);

            // Check map edges
            bool nSmooth = (y != height-1);
            bool eSmooth = (x != width-1);
            bool sSmooth = (y != 0);
            bool wSmooth = (x != 0);
            if (!(nSmooth || eSmooth || sSmooth || wSmooth)) continue;

            // Fill Vectors of adjacent face normals
            if (nSmooth && eSmooth) {
                neNorms[0] = glm::vec3(GET_OWN_NORM(n, 0), GET_OWN_NORM(n, 1), zNormals [pos]);
                neNorms[1] = glm::vec3(GET_OWN_NORM(e, 0), GET_OWN_NORM(e, 1), zNormals [pos]);
                neNorms[2] = glm::vec3(GET_NORM(n, e, 0),  GET_NORM(n, e, 1),  zNormals [nPos]);
                neNorms[3] = glm::vec3(GET_NORM(n, s, 0),  GET_NORM(n, s, 1),  zNormals [nPos]);
                neNorms[4] = glm::vec3(GET_NORM(ne, s, 0), GET_NORM(ne, s, 1), zNormals [nePos]);
                neNorms[5] = glm::vec3(GET_NORM(ne, w, 0), GET_NORM(ne, w, 1), zNormals [nePos]);
                neNorms[6] = glm::vec3(GET_NORM(e, w, 0),  GET_NORM(e, w, 1),  zNormals [ePos]);
                neNorms[7] = glm::vec3(GET_NORM(e, n, 0),  GET_NORM(e, n, 1),  zNormals [ePos]);
            }
            if (eSmooth && sSmooth) {
                esNorms[0] = glm::vec3(GET_OWN_NORM(e, 0), GET_OWN_NORM(e, 1), zNormals [pos]);
                esNorms[1] = glm::vec3(GET_OWN_NORM(s, 0), GET_OWN_NORM(s, 1), zNormals [pos]);
                esNorms[2] = glm::vec3(GET_NORM(e, s, 0),  GET_NORM(e, s, 1),  zNormals [ePos]);
                esNorms[3] = glm::vec3(GET_NORM(e, w, 0),  GET_NORM(e, w, 1),  zNormals [ePos]);
                esNorms[4] = glm::vec3(GET_NORM(es, w, 0), GET_NORM(es, w, 1), zNormals [esPos]);
                esNorms[5] = glm::vec3(GET_NORM(es, n, 0), GET_NORM(es, n, 1), zNormals [esPos]);
                esNorms[6] = glm::vec3(GET_NORM(s, n, 0),  GET_NORM(s, n, 1),  zNormals [sPos]);
                esNorms[7] = glm::vec3(GET_NORM(s, e, 0),  GET_NORM(s, e, 1),  zNormals [sPos]);
            }
            if (sSmooth && wSmooth) {
                swNorms[0] = glm::vec3(GET_OWN_NORM(s, 0), GET_OWN_NORM(s, 1), zNormals [pos]);
                swNorms[1] = glm::vec3(GET_OWN_NORM(w, 0), GET_OWN_NORM(w, 1), zNormals [pos]);
                swNorms[2] = glm::vec3(GET_NORM(s, w, 0),  GET_NORM(s, w, 1),  zNormals [sPos]);
                swNorms[3] = glm::vec3(GET_NORM(s, n, 0),  GET_NORM(s, n, 1),  zNormals [sPos]);
                swNorms[4] = glm::vec3(GET_NORM(sw, n, 0), GET_NORM(sw, n, 1), zNormals [swPos]);
                swNorms[5] = glm::vec3(GET_NORM(sw, e, 0), GET_NORM(sw, e, 1), zNormals [swPos]);
                swNorms[6] = glm::vec3(GET_NORM(w, e, 0),  GET_NORM(w, e, 1),  zNormals [wPos]);
                swNorms[7] = glm::vec3(GET_NORM(w, s, 0),  GET_NORM(w, s, 1),  zNormals [wPos]);
            }
            if (wSmooth && nSmooth) {
                wnNorms[0] = glm::vec3(GET_OWN_NORM(w, 0), GET_OWN_NORM(w, 1), zNormals [pos]);
                wnNorms[1] = glm::vec3(GET_OWN_NORM(n, 0), GET_OWN_NORM(n, 1), zNormals [pos]);
                wnNorms[2] = glm::vec3(GET_NORM(w, n, 0),  GET_NORM(w, n, 1),  zNormals [wPos]);
                wnNorms[3] = glm::vec3(GET_NORM(w, e, 0),  GET_NORM(w, e, 1),  zNormals [wPos]);
                wnNorms[4] = glm::vec3(GET_NORM(wn, e, 0), GET_NORM(wn, e, 1), zNormals [wnPos]);
                wnNorms[5] = glm::vec3(GET_NORM(wn, s, 0), GET_NORM(wn, s, 1), zNormals [wnPos]);
                wnNorms[6] = glm::vec3(GET_NORM(n, s, 0),  GET_NORM(n, s, 1),  zNormals [nPos]);
                wnNorms[7] = glm::vec3(GET_NORM(n, w, 0),  GET_NORM(n, w, 1),  zNormals [nPos]);
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

            // Set north-east normals
            if (nSmooth && eSmooth) {
                glm::vec4 tangent = sq::get_tangent(neSum);
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    SET_NORMAL(pos, 2, neSum);
                    SET_NORMAL(pos, 5, neSum);
                    SET_TANGENT(pos, 2, tangent);
                    SET_TANGENT(pos, 5, tangent);
                } else
                if (angle == 1) {
                    SET_NORMAL(pos, 2, neSum);
                    SET_NORMAL(pos, 5, neSum);
                    SET_TANGENT(pos, 2, tangent);
                    SET_TANGENT(pos, 5, tangent);
                } else
                if (angle == 3) {
                    SET_NORMAL(pos, 1, neSum);
                    SET_TANGENT(pos, 1, tangent);
                } else
                if (angle == 5) {
                    SET_NORMAL(pos, 0, neSum);
                    SET_NORMAL(pos, 3, neSum);
                    SET_TANGENT(pos, 0, tangent);
                    SET_TANGENT(pos, 3, tangent);
                } else
                if (angle == 7) {
                    SET_NORMAL(pos, 4, neSum);
                    SET_TANGENT(pos, 4, tangent);
                }
            }

            // Set east-south normals
            if (eSmooth && sSmooth) {
                glm::vec4 tangent = sq::get_tangent(esSum);
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    SET_NORMAL(pos, 1, esSum);
                    SET_TANGENT(pos, 1, tangent);
                } else
                if (angle == 1) {
                    SET_NORMAL(pos, 4, esSum);
                    SET_TANGENT(pos, 4, tangent);
                } else
                if (angle == 3) {
                    SET_NORMAL(pos, 2, esSum);
                    SET_NORMAL(pos, 5, esSum);
                    SET_TANGENT(pos, 2, tangent);
                    SET_TANGENT(pos, 5, tangent);
                } else
                if (angle == 5) {
                    SET_NORMAL(pos, 1, esSum);
                    SET_TANGENT(pos, 1, tangent);
                } else
                if (angle == 7) {
                    SET_NORMAL(pos, 0, esSum);
                    SET_NORMAL(pos, 3, esSum);
                    SET_TANGENT(pos, 0, tangent);
                    SET_TANGENT(pos, 3, tangent);
                }
            }

            // Set south-west normals
            if (sSmooth && wSmooth) {
                glm::vec4 tangent = sq::get_tangent(swSum);
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    SET_NORMAL(pos, 0, swSum);
                    SET_NORMAL(pos, 3, swSum);
                    SET_TANGENT(pos, 0, tangent);
                    SET_TANGENT(pos, 3, tangent);
                } else
                if (angle == 1) {
                    SET_NORMAL(pos, 0, swSum);
                    SET_NORMAL(pos, 3, swSum);
                    SET_TANGENT(pos, 0, tangent);
                    SET_TANGENT(pos, 3, tangent);
                } else
                if (angle == 3) {
                    SET_NORMAL(pos, 4, swSum);
                    SET_TANGENT(pos, 4, tangent);
                } else
                if (angle == 5) {
                    SET_NORMAL(pos, 2, swSum);
                    SET_NORMAL(pos, 5, swSum);
                    SET_TANGENT(pos, 2, tangent);
                    SET_TANGENT(pos, 5, tangent);
                } else
                if (angle == 7) {
                    SET_NORMAL(pos, 1, swSum);
                    SET_TANGENT(pos, 1, tangent);
                }
            }

            // Set west-north normals
            if (wSmooth && nSmooth) {
                glm::vec4 tangent = sq::get_tangent(wnSum);
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    SET_NORMAL(pos, 4, wnSum);
                    SET_TANGENT(pos, 4, tangent);
                } else
                if (angle == 1) {
                    SET_NORMAL(pos, 1, wnSum);
                    SET_TANGENT(pos, 1, tangent);
                } else
                if (angle == 3) {
                    SET_NORMAL(pos, 0, wnSum);
                    SET_NORMAL(pos, 3, wnSum);
                    SET_TANGENT(pos, 0, tangent);
                    SET_TANGENT(pos, 3, tangent);
                } else
                if (angle == 5) {
                    SET_NORMAL(pos, 4, wnSum);
                    SET_TANGENT(pos, 4, tangent);
                } else
                if (angle == 7) {
                    SET_NORMAL(pos, 2, wnSum);
                    SET_NORMAL(pos, 5, wnSum);
                    SET_TANGENT(pos, 2, tangent);
                    SET_TANGENT(pos, 5, tangent);
                }
            }
        }
    }

    // Remove non-visible tiles
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            if (invisibles[pos]) {
                for (int i = 0; i < 18; i++) {
                    points[pos*18 + i] = 0.f;
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    glGenBuffers(1, &vboTexcoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    glGenBuffers(1, &vboTangents);
    glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangents), tangents, GL_STATIC_DRAW);

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
    glVertexAttribPointer(2, 3, GL_INT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    // Load Textures
    sf::Image image;
    glGenTextures(1, &ground.texArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, ground.texArray);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 64, 64, texCount, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
                              val["index"].asInt()));
        }
    }

    return false;
}

}

#undef CHECK_BIT
#undef GET_NORM
#undef GET_OWN_NORM
#undef SET_NORMAL
#undef SET_TANGENT
