#include "maps.hpp"

#include <bitset>

#include <libsqee/extra/helpers.hpp>

#define CHECK_BIT(var, pos) bool((var) & (1<<(pos)))

namespace sqt {

Ground::Ground() {

}

bool Ground::load_map(std::string filePath, sq::TextureHolder& texHolder) {
    Json::Value root = sqe::load_json_file(filePath);

    width = root["width"].asInt();
    height = root["height"].asInt();

    for (int i = 0; i < width*height; i++) {
        tilesModels.push_back(root["tilesModels"][i].asInt());
    }

    for (int i = 0; i < int(root["textures"].size()); i++) {
        texHolder.add_texture("ground_"+std::to_string(i),
                              "res/textures/ground/" + root["textures"][i].asString());
    }

    for (int i = 0; i < width*height; i++) {
        tilesTextures.push_back(root["tilesTextures"][i].asInt());
    }

    return false;
}

void Ground::get_models(GLuint& vao, int& pCount) {
    pCount = width * height * 6;
    float points[width * height * 18];
    float normals[width * height * 18];
    float texcoords[width * height * 12];

    float angleNormals[9][2][2] = {
        {{0.f, 0.25f},  {0.f, 0.25f}},  // 0
        {{0.f, 0.25f},  {0.25f, 0.f}},  // 1
        {{0.25f, 0.f},  {0.25f, 0.f}},  // 2
        {{0.25f, 0.f},  {0.f, -0.25f}}, // 3
        {{0.f, -0.25f}, {0.f, -0.25f}}, // 4
        {{0.f, -0.25f}, {-0.25f, 0.f}}, // 5
        {{-0.25f, 0.f}, {-0.25f, 0.f}}, // 6
        {{-0.25f, 0.f}, {0.f, 0.25f}},  // 7
        {{0.f, 0.f}, {0.f, 0.f}}        // 8
    };

    bool nAngleInd[9] = {0, 0, 0, 0, 0, 1, 0, 1, 0};
    bool eAngleInd[9] = {0, 1, 0, 0, 0, 0, 0, 1, 0};
    bool sAngleInd[9] = {0, 1, 0, 1, 0, 0, 0, 0, 0};
    bool wAngleInd[9] = {0, 0, 0, 1, 0, 1, 0, 0, 0};

    // Working Arrays
    int zBases[width * height];
    bool slopeds[width * height];
    bool corners[width * height];
    bool v1s[width * height];
    bool v2s[width * height];
    bool inclines[width * height];
    bool smooths[width * height];
    int angles[width * height];

    // Fill Working Arrays
    for (int y_ = 0; y_ < height; y_++) {
        int y = height - y_ - 1;
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            int val = tilesModels[width*y_ + x];

            zBases[pos]      = (val >> 7) % 64;
            slopeds[pos]     = CHECK_BIT(val, 6);
            corners[pos]     = CHECK_BIT(val, 5);
            v1s[pos]         = CHECK_BIT(val, 4);
            v2s[pos]         = CHECK_BIT(val, 3);
            inclines[pos]    = CHECK_BIT(val, 2);
            smooths[pos]     = CHECK_BIT(val, 1);
            angles[pos]      = 8;

            if (slopeds[pos]) {
                if (corners[pos]) {
                    if      (!v1s[pos] && !v2s[pos]) angles[pos] = 1;
                    else if (!v1s[pos] &&  v2s[pos]) angles[pos] = 3;
                    else if ( v1s[pos] &&  v2s[pos]) angles[pos] = 5;
                    else if ( v1s[pos] && !v2s[pos]) angles[pos] = 7;
                } else {
                    if      ( v1s[pos] && !v2s[pos]) angles[pos] = 0;
                    else if (!v1s[pos] && !v2s[pos]) angles[pos] = 2;
                    else if ( v1s[pos] &&  v2s[pos]) angles[pos] = 4;
                    else if (!v1s[pos] &&  v2s[pos]) angles[pos] = 6;
                }
            }
        }
    }

    // Points and Flat Normals
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            int start = pos * 18;
            float left   = x;
            float right  = x+1;
            float top    = y+1;
            float bottom = y;

            bool sloped = slopeds[pos];
            bool corner = corners[pos];
            bool incline = inclines[pos];

            int angle = angles[pos];

            if (corner) {
                if (angle == 1) {
                    points[start+0] = left;
                    points[start+1] = bottom;
                    points[start+3] = left;
                    points[start+4] = top;
                    points[start+6] = right;
                    points[start+7] = top;

                    points[start+9] = left;
                    points[start+10] = bottom;
                    points[start+12] = right;
                    points[start+13] = bottom;
                    points[start+15] = right;
                    points[start+16] = top;

                    texcoords[pos*12+0] = 0.f;
                    texcoords[pos*12+1] = 0.f;
                    texcoords[pos*12+2] = 0.f;
                    texcoords[pos*12+3] = 1.f;
                    texcoords[pos*12+4] = 1.f;
                    texcoords[pos*12+5] = 1.f;

                    texcoords[pos*12+6] = 0.f;
                    texcoords[pos*12+7] = 0.f;
                    texcoords[pos*12+8] = 1.f;
                    texcoords[pos*12+9] = 0.f;
                    texcoords[pos*12+10] = 1.f;
                    texcoords[pos*12+11] = 1.f;
                } else
                if (angle == 3) {
                    points[start+0] = left;
                    points[start+1] = top;
                    points[start+3] = right;
                    points[start+4] = top;
                    points[start+6] = right;
                    points[start+7] = bottom;

                    points[start+9] = left;
                    points[start+10] = top;
                    points[start+12] = left;
                    points[start+13] = bottom;
                    points[start+15] = right;
                    points[start+16] = bottom;

                    texcoords[pos*12+0] = 0.f;
                    texcoords[pos*12+1] = 1.f;
                    texcoords[pos*12+2] = 1.f;
                    texcoords[pos*12+3] = 1.f;
                    texcoords[pos*12+4] = 1.f;
                    texcoords[pos*12+5] = 0.f;

                    texcoords[pos*12+6] = 0.f;
                    texcoords[pos*12+7] = 1.f;
                    texcoords[pos*12+8] = 0.f;
                    texcoords[pos*12+9] = 0.f;
                    texcoords[pos*12+10] = 1.f;
                    texcoords[pos*12+11] = 0.f;
                } else
                if (angle == 5) {
                    points[start+0] = right;
                    points[start+1] = top;
                    points[start+3] = right;
                    points[start+4] = bottom;
                    points[start+6] = left;
                    points[start+7] = bottom;

                    points[start+9] = right;
                    points[start+10] = top;
                    points[start+12] = left;
                    points[start+13] = top;
                    points[start+15] = left;
                    points[start+16] = bottom;

                    texcoords[pos*12+0] = 1.f;
                    texcoords[pos*12+1] = 1.f;
                    texcoords[pos*12+2] = 1.f;
                    texcoords[pos*12+3] = 0.f;
                    texcoords[pos*12+4] = 0.f;
                    texcoords[pos*12+5] = 0.f;

                    texcoords[pos*12+6] = 1.f;
                    texcoords[pos*12+7] = 1.f;
                    texcoords[pos*12+8] = 0.f;
                    texcoords[pos*12+9] = 1.f;
                    texcoords[pos*12+10] = 0.f;
                    texcoords[pos*12+11] = 0.f;
                } else
                if (angle == 7) {
                    points[start+0] = right;
                    points[start+1] = bottom;
                    points[start+3] = left;
                    points[start+4] = bottom;
                    points[start+6] = left;
                    points[start+7] = top;

                    points[start+9] = right;
                    points[start+10] = bottom;
                    points[start+12] = right;
                    points[start+13] = top;
                    points[start+15] = left;
                    points[start+16] = top;

                    texcoords[pos*12+0] = 1.f;
                    texcoords[pos*12+1] = 0.f;
                    texcoords[pos*12+2] = 0.f;
                    texcoords[pos*12+3] = 0.f;
                    texcoords[pos*12+4] = 0.f;
                    texcoords[pos*12+5] = 1.f;

                    texcoords[pos*12+6] = 1.f;
                    texcoords[pos*12+7] = 0.f;
                    texcoords[pos*12+8] = 1.f;
                    texcoords[pos*12+9] = 1.f;
                    texcoords[pos*12+10] = 0.f;
                    texcoords[pos*12+11] = 1.f;
                }
            } else {
                points[start+0] = left;
                points[start+1] = bottom;
                points[start+3] = right;
                points[start+4] = bottom;
                points[start+6] = right;
                points[start+7] = top;

                points[start+9] = left;
                points[start+10] = bottom;
                points[start+12] = left;
                points[start+13] = top;
                points[start+15] = right;
                points[start+16] = top;

                texcoords[pos*12+0] = 0.f;
                texcoords[pos*12+1] = 0.f;
                texcoords[pos*12+2] = 1.f;
                texcoords[pos*12+3] = 0.f;
                texcoords[pos*12+4] = 1.f;
                texcoords[pos*12+5] = 1.f;

                texcoords[pos*12+6] = 0.f;
                texcoords[pos*12+7] = 0.f;
                texcoords[pos*12+8] = 0.f;
                texcoords[pos*12+9] = 1.f;
                texcoords[pos*12+10] = 1.f;
                texcoords[pos*12+11] = 1.f;
            }

            for (int p = 0; p < 6; p++) {
                int zBase = zBases[pos];
                if (sloped) {
                    if (corner) {
                        if (p==0 || p==3) {
                            zBase += 1 + incline;
                        }
                    } else {
                        if ((angle == 0 && (p==0 || p==1 || p==3)) ||
                            (angle == 2 && (p==0 || p==3 || p==4)) ||
                            (angle == 4 && (p==2 || p==4 || p==5)) ||
                            (angle == 6 && (p==1 || p==2 || p==5))) {
                            zBase += 1 + incline;
                        }
                    }
                }
                points[start + p*3 +2] = float(zBase) / 2.f;
            }
        }
    }

    // Calculate Normals
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pos = y * width + x;
            int start = pos * 18;

            bool incline = inclines[pos];
            bool smooth = smooths[pos];
            int angle = angles[pos];

            // Copy Flat Normals
            for (int p = 0; p < 6; p++) {
                normals[start + p*3 +0] = angleNormals[angle][1*(p>2)][0] * (1+incline);
                normals[start + p*3 +1] = angleNormals[angle][1*(p>2)][1] * (1+incline);
                normals[start + p*3 +2] = 1.f;
            }

            // No Smoothing
            if (!smooth) continue;

            // Set up some variables
            int nPos = pos + width;
            int ePos = pos + 1;
            int sPos = pos - width;
            int wPos = pos - 1;
            int nePos = pos + width + 1;
            int esPos = pos + 1 - width;
            int swPos = pos - width - 1;
            int wnPos = pos - 1 + width;
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
            std::vector<glm::vec2> neNorms; neNorms.resize(8);
            std::vector<glm::vec2> esNorms; esNorms.resize(8);
            std::vector<glm::vec2> swNorms; swNorms.resize(8);
            std::vector<glm::vec2> wnNorms; wnNorms.resize(8);

            // check edges
            if (y == height-1) nSmooth = false;
            else               nSmooth = smooths[nPos];
            if (x == width-1)  eSmooth = false;
            else               eSmooth = smooths[ePos];
            if (y == 0)        sSmooth = false;
            else               sSmooth = smooths[sPos];
            if (x == 0)        wSmooth = false;
            else               wSmooth = smooths[wPos];

            if (!(nSmooth || eSmooth || sSmooth || wSmooth)) continue;

            if (nSmooth) {
                nNorms.x = angleNormals [nAngle] [sAngleInd[nAngle]] [0] * (1+nIncline);
                nNorms.y = angleNormals [nAngle] [sAngleInd[nAngle]] [1] * (1+nIncline);
            }
            if (eSmooth) {
                eNorms.x = angleNormals [eAngle] [wAngleInd[eAngle]] [0] * (1+eIncline);
                eNorms.y = angleNormals [eAngle] [wAngleInd[eAngle]] [1] * (1+eIncline);
            }
            if (sSmooth) {
                sNorms.x = angleNormals [sAngle] [nAngleInd[sAngle]] [0] * (1+sIncline);
                sNorms.y = angleNormals [sAngle] [nAngleInd[sAngle]] [1] * (1+sIncline);
            }
            if (wSmooth) {
                wNorms.x = angleNormals [wAngle] [eAngleInd[wAngle]] [0] * (1+wIncline);
                wNorms.y = angleNormals [wAngle] [eAngleInd[wAngle]] [1] * (1+wIncline);
            }

            if (nSmooth && eSmooth) {
                neNorms[0] = glm::vec2(angleNormals [angle] [0] [0] * (1+incline),
                                       angleNormals [angle] [0] [1] * (1+incline));
                neNorms[1] = glm::vec2(angleNormals [angle] [1] [0] * (1+incline),
                                       angleNormals [angle] [1] [1] * (1+incline));
                neNorms[2] = glm::vec2(angleNormals [nAngle] [eAngleInd[nAngle]] [0] * (1+nIncline),
                                       angleNormals [nAngle] [eAngleInd[nAngle]] [1] * (1+nIncline));
                neNorms[3] = glm::vec2(angleNormals [nAngle] [sAngleInd[nAngle]] [0] * (1+nIncline),
                                       angleNormals [nAngle] [sAngleInd[nAngle]] [1] * (1+nIncline));
                neNorms[4] = glm::vec2(angleNormals [neAngle] [sAngleInd[neAngle]] [0] * (1+neIncline),
                                       angleNormals [neAngle] [sAngleInd[neAngle]] [1] * (1+neIncline));
                neNorms[5] = glm::vec2(angleNormals [neAngle] [wAngleInd[neAngle]] [0] * (1+neIncline),
                                       angleNormals [neAngle] [wAngleInd[neAngle]] [1] * (1+neIncline));
                neNorms[6] = glm::vec2(angleNormals [eAngle] [wAngleInd[eAngle]] [0] * (1+eIncline),
                                       angleNormals [eAngle] [wAngleInd[eAngle]] [1] * (1+eIncline));
                neNorms[7] = glm::vec2(angleNormals [eAngle] [nAngleInd[eAngle]] [0] * (1+eIncline),
                                       angleNormals [eAngle] [nAngleInd[eAngle]] [1] * (1+eIncline));
            }

            if (eSmooth && sSmooth) {
                esNorms[0] = glm::vec2(angleNormals [angle] [0] [0] * (1+incline),
                                       angleNormals [angle] [0] [1] * (1+incline));
                esNorms[1] = glm::vec2(angleNormals [angle] [1] [0] * (1+incline),
                                       angleNormals [angle] [1] [1] * (1+incline));
                esNorms[2] = glm::vec2(angleNormals [eAngle] [sAngleInd[eAngle]] [0] * (1+eIncline),
                                       angleNormals [eAngle] [sAngleInd[eAngle]] [1] * (1+eIncline));
                esNorms[3] = glm::vec2(angleNormals [eAngle] [wAngleInd[eAngle]] [0] * (1+eIncline),
                                       angleNormals [eAngle] [wAngleInd[eAngle]] [1] * (1+eIncline));
                esNorms[4] = glm::vec2(angleNormals [esAngle] [wAngleInd[esAngle]] [0] * (1+esIncline),
                                       angleNormals [esAngle] [wAngleInd[esAngle]] [1] * (1+esIncline));
                esNorms[5] = glm::vec2(angleNormals [esAngle] [nAngleInd[esAngle]] [0] * (1+esIncline),
                                       angleNormals [esAngle] [nAngleInd[esAngle]] [1] * (1+esIncline));
                esNorms[6] = glm::vec2(angleNormals [sAngle] [nAngleInd[sAngle]] [0] * (1+sIncline),
                                       angleNormals [sAngle] [nAngleInd[sAngle]] [1] * (1+sIncline));
                esNorms[7] = glm::vec2(angleNormals [sAngle] [eAngleInd[sAngle]] [0] * (1+sIncline),
                                       angleNormals [sAngle] [eAngleInd[sAngle]] [1] * (1+sIncline));
            }

            if (sSmooth && wSmooth) {
                swNorms[0] = glm::vec2(angleNormals [angle] [0] [0] * (1+incline),
                                       angleNormals [angle] [0] [1] * (1+incline));
                swNorms[1] = glm::vec2(angleNormals [angle] [1] [0] * (1+incline),
                                       angleNormals [angle] [1] [1] * (1+incline));
                swNorms[2] = glm::vec2(angleNormals [sAngle] [wAngleInd[sAngle]] [0] * (1+sIncline),
                                       angleNormals [sAngle] [wAngleInd[sAngle]] [1] * (1+sIncline));
                swNorms[3] = glm::vec2(angleNormals [sAngle] [nAngleInd[sAngle]] [0] * (1+sIncline),
                                       angleNormals [sAngle] [nAngleInd[sAngle]] [1] * (1+sIncline));
                swNorms[4] = glm::vec2(angleNormals [swAngle] [nAngleInd[swAngle]] [0] * (1+swIncline),
                                       angleNormals [swAngle] [nAngleInd[swAngle]] [1] * (1+swIncline));
                swNorms[5] = glm::vec2(angleNormals [swAngle] [eAngleInd[swAngle]] [0] * (1+swIncline),
                                       angleNormals [swAngle] [eAngleInd[swAngle]] [1] * (1+swIncline));
                swNorms[6] = glm::vec2(angleNormals [wAngle] [eAngleInd[wAngle]] [0] * (1+wIncline),
                                       angleNormals [wAngle] [eAngleInd[wAngle]] [1] * (1+wIncline));
                swNorms[7] = glm::vec2(angleNormals [wAngle] [sAngleInd[wAngle]] [0] * (1+wIncline),
                                       angleNormals [wAngle] [sAngleInd[wAngle]] [1] * (1+wIncline));
            }

            if (wSmooth && nSmooth) {
                wnNorms[0] = glm::vec2(angleNormals [angle] [0] [0] * (1+incline),
                                       angleNormals [angle] [0] [1] * (1+incline));
                wnNorms[1] = glm::vec2(angleNormals [angle] [1] [0] * (1+incline),
                                       angleNormals [angle] [1] [1] * (1+incline));
                wnNorms[2] = glm::vec2(angleNormals [wAngle] [nAngleInd[wAngle]] [0] * (1+wIncline),
                                       angleNormals [wAngle] [nAngleInd[wAngle]] [1] * (1+wIncline));
                wnNorms[3] = glm::vec2(angleNormals [wAngle] [eAngleInd[wAngle]] [0] * (1+wIncline),
                                       angleNormals [wAngle] [eAngleInd[wAngle]] [1] * (1+wIncline));
                wnNorms[4] = glm::vec2(angleNormals [wnAngle] [eAngleInd[wnAngle]] [0] * (1+wnIncline),
                                       angleNormals [wnAngle] [eAngleInd[wnAngle]] [1] * (1+wnIncline));
                wnNorms[5] = glm::vec2(angleNormals [wnAngle] [sAngleInd[wnAngle]] [0] * (1+wnIncline),
                                       angleNormals [wnAngle] [sAngleInd[wnAngle]] [1] * (1+wnIncline));
                wnNorms[6] = glm::vec2(angleNormals [nAngle] [sAngleInd[nAngle]] [0] * (1+nIncline),
                                       angleNormals [nAngle] [sAngleInd[nAngle]] [1] * (1+nIncline));
                wnNorms[7] = glm::vec2(angleNormals [nAngle] [wAngleInd[nAngle]] [0] * (1+nIncline),
                                       angleNormals [nAngle] [wAngleInd[nAngle]] [1] * (1+nIncline));
            }

            glm::vec2 neSum;
            for (glm::vec2& norm : neNorms) {
                neSum.x += norm.x;
                neSum.y += norm.y;
            }

            neSum.x /= 8.f;
            neSum.y /= 8.f;

            glm::vec2 esSum;
            for (glm::vec2& norm : esNorms) {
                esSum.x += norm.x;
                esSum.y += norm.y;
            }
            esSum.x /= 8.f;
            esSum.y /= 8.f;

            glm::vec2 swSum;
            for (glm::vec2& norm : swNorms) {
                swSum.x += norm.x;
                swSum.y += norm.y;
            }
            swSum.x /= 8.f;
            swSum.y /= 8.f;

            glm::vec2 wnSum;
            for (glm::vec2& norm : wnNorms) {
                wnSum.x += norm.x;
                wnSum.y += norm.y;
            }
            wnSum.x /= 8.f;
            wnSum.y /= 8.f;

            if (nSmooth && eSmooth) {
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    normals[start+6+0] = neSum.x;
                    normals[start+6+1] = neSum.y;
                    normals[start+15+0] = neSum.x;
                    normals[start+15+1] = neSum.y;
                } else
                if (angle == 1) {
                    normals[start+6+0] = neSum.x;
                    normals[start+6+1] = neSum.y;
                    normals[start+15+0] = neSum.x;
                    normals[start+15+1] = neSum.y;
                } else
                if (angle == 3) {
                    normals[start+3+0] = neSum.x;
                    normals[start+3+1] = neSum.y;
                } else
                if (angle == 5) {
                    normals[start+0+0] = neSum.x;
                    normals[start+0+1] = neSum.y;
                    normals[start+9+0] = neSum.x;
                    normals[start+9+1] = neSum.y;
                } else
                if (angle == 7) {
                    normals[start+12+0] = neSum.x;
                    normals[start+12+1] = neSum.y;
                }
            }

            if (eSmooth && sSmooth) {
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    normals[start+3+0] = esSum.x;
                    normals[start+3+1] = esSum.y;
                } else
                if (angle == 1) {
                    normals[start+12+0] = esSum.x;
                    normals[start+12+1] = esSum.y;
                } else
                if (angle == 3) {
                    normals[start+6+0] = esSum.x;
                    normals[start+6+1] = esSum.y;
                    normals[start+15+0] = esSum.x;
                    normals[start+15+1] = esSum.y;
                } else
                if (angle == 5) {
                    normals[start+3+0] = esSum.x;
                    normals[start+3+1] = esSum.y;
                } else
                if (angle == 7) {
                    normals[start+0+0] = esSum.x;
                    normals[start+0+1] = esSum.y;
                    normals[start+9+0] = esSum.x;
                    normals[start+9+1] = esSum.y;
                }
            }

            if (sSmooth && wSmooth) {
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    normals[start+0+0] = swSum.x;
                    normals[start+0+1] = swSum.y;
                    normals[start+9+0] = swSum.x;
                    normals[start+9+1] = swSum.y;
                } else
                if (angle == 1) {
                    normals[start+0+0] = swSum.x;
                    normals[start+0+1] = swSum.y;
                    normals[start+9+0] = swSum.x;
                    normals[start+9+1] = swSum.y;
                } else
                if (angle == 3) {
                    normals[start+12+0] = swSum.x;
                    normals[start+12+1] = swSum.y;
                } else
                if (angle == 5) {
                    normals[start+6+0] = swSum.x;
                    normals[start+6+1] = swSum.y;
                    normals[start+15+0] = swSum.x;
                    normals[start+15+1] = swSum.y;
                } else
                if (angle == 7) {
                    normals[start+3+0] = swSum.x;
                    normals[start+3+1] = swSum.y;
                }
            }

            if (wSmooth && nSmooth) {
                if (angle==0 || angle==2 || angle==4 || angle==6 || angle==8) {
                    normals[start+12+0] = wnSum.x;
                    normals[start+12+1] = wnSum.y;
                } else
                if (angle == 1) {
                    normals[start+3+0] = wnSum.x;
                    normals[start+3+1] = wnSum.y;
                } else
                if (angle == 3) {
                    normals[start+0+0] = wnSum.x;
                    normals[start+0+1] = wnSum.y;
                    normals[start+9+0] = wnSum.x;
                    normals[start+9+1] = wnSum.y;
                } else
                if (angle == 5) {
                    normals[start+12+0] = wnSum.x;
                    normals[start+12+1] = wnSum.y;
                } else
                if (angle == 7) {
                    normals[start+6+0] = wnSum.x;
                    normals[start+6+1] = wnSum.y;
                    normals[start+15+0] = wnSum.x;
                    normals[start+15+1] = wnSum.y;
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

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vboPoints);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexcoords);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

}
