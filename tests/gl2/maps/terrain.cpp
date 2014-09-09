#include "terrain.hpp"

#define CHECK_BIT(_v, _p) bool((_v) & (1<<(_p)))

using namespace sqt;

glm::vec3 get_face_normal(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
    return glm::normalize(glm::cross(p1 - p0, p2 - p0));
}

const float xyTiles[24] = {
    0.f, 1.f,  1.f, 1.f,  0.5f, 0.5f,
    1.f, 1.f,  1.f, 0.f,  0.5f, 0.5f,
    1.f, 0.f,  0.f, 0.f,  0.5f, 0.5f,
    0.f, 0.f,  0.f, 1.f,  0.5f, 0.5f
};

bool Terrain::load(glm::uvec2 _size, uint _texCount,
                   const std::vector<std::string>& _texPaths,
                   const std::string& _map,
                   const int* _geometry, const bool* _smoothing,
                   const int* _textures, const bool* _visible) {
    int tileLen = _size.x * _size.y;
    vCount = tileLen * 12;

    float points[tileLen * 36];
    float normalsFlat[tileLen * 36];
    float normals[tileLen * 36];
    float texcoords[tileLen * 36];
    float tangents[tileLen * 36];

    // Fill working arrays and set textures (Z texcoord)
    for (uint y = 0; y < _size.y; y++) {
        int geoy = y * (_size.x+1);
        for (uint x = 0; x < _size.x; x++) {
            uint t = y * _size.x + x;

            points[t*36 + 10*3 +2] = float(_geometry[geoy+x + _size.x+1 +0]) / 4.f;
            points[t*36 + 0*3  +2] = float(_geometry[geoy+x + _size.x+1 +0]) / 4.f;
            points[t*36 + 1*3  +2] = float(_geometry[geoy+x + _size.x+1 +1]) / 4.f;
            points[t*36 + 3*3  +2] = float(_geometry[geoy+x + _size.x+1 +1]) / 4.f;
            points[t*36 + 4*3  +2] = float(_geometry[geoy+x + 0         +1]) / 4.f;
            points[t*36 + 6*3  +2] = float(_geometry[geoy+x + 0         +1]) / 4.f;
            points[t*36 + 7*3  +2] = float(_geometry[geoy+x + 0         +0]) / 4.f;
            points[t*36 + 9*3  +2] = float(_geometry[geoy+x + 0         +0]) / 4.f;

            for (int p = 0; p < 12; p++) {
                points[t*36 + p*3 +0] = float(x) + xyTiles[p*2 +0];
                points[t*36 + p*3 +1] = float(y) + xyTiles[p*2 +1];
                texcoords[t*36 + p*3 +0] = xyTiles[p*2 +0];
                texcoords[t*36 + p*3 +1] = xyTiles[p*2 +1];
                texcoords[t*36 + p*3 +2] = _textures[t];
            }

            float valH = 0.f;
            float valL = 64.f;
            for (int s = 0; s < 4; s++) {
                valH = std::max(valH, points[t*36 + s*9 + 2]);
                valL = std::min(valL, points[t*36 + s*9 + 2]);
            }
            float valC = (valH + valL) / 2.f;

            for (int s = 0; s < 4; s++) {
                points[t*36 + s*9 +8] = valC;
            }

            for (int s = 0; s < 4; s++) {
                glm::vec3 A = {points[t*36 + s*9 +0], points[t*36 + s*9 +1], points[t*36 + s*9 +2]};
                glm::vec3 B = {points[t*36 + s*9 +3], points[t*36 + s*9 +4], points[t*36 + s*9 +5]};
                glm::vec3 C = {points[t*36 + s*9 +6], points[t*36 + s*9 +7], points[t*36 + s*9 +8]};
                glm::vec3 norm = get_face_normal(C, B, A);
                for (int p = 0; p < 3; p++) {
                    normalsFlat[t*36 + s*9 + p*3 +0] = norm.x;
                    normalsFlat[t*36 + s*9 + p*3 +1] = norm.y;
                    normalsFlat[t*36 + s*9 + p*3 +2] = norm.z;
                }
            }
        }
    }


    for (int c = 0; c < tileLen * 36; c++) {
        normals[c] = normalsFlat[c];
    }

    // Calculate smooth normals and set normals
    for (uint y = 0; y < _size.y; y++) {
        for (uint x = 0; x < _size.x; x++) {
            uint t = y * _size.x + x;

            // No smoothing
            if (!_smoothing[t]) continue;

            // Set up just a few variables
            int nT = t + _size.x;
            int eT = t + 1;
            int sT = t - _size.x;
            int wT = t - 1;
            int neT = t + _size.x + 1;
            int esT = t + 1 - _size.x;
            int swT = t - _size.x - 1;
            int wnT = t - 1 + _size.x;

            std::vector<glm::vec3> neNorms; neNorms.resize(8);
            std::vector<glm::vec3> esNorms; esNorms.resize(8);
            std::vector<glm::vec3> swNorms; swNorms.resize(8);
            std::vector<glm::vec3> wnNorms; wnNorms.resize(8);

            // Check map edges
            bool nEdge = (y == _size.y -1);
            bool eEdge = (x == _size.x -1);
            bool sEdge = (y == 0);
            bool wEdge = (x == 0);

            glm::vec3 up = {0.f, 0.f, 1.f};

            // North East
            neNorms[0] = glm::vec3(normalsFlat[t*36 + 1*3 +0],
                                   normalsFlat[t*36 + 1*3 +1],
                                   normalsFlat[t*36 + 1*3 +2]);
            neNorms[1] = glm::vec3(normalsFlat[t*36 + 3*3 +0],
                                   normalsFlat[t*36 + 3*3 +1],
                                   normalsFlat[t*36 + 3*3 +2]);

            if (nEdge) {
                neNorms[2] = neNorms[3] = up;
            } else {
                neNorms[2] = glm::vec3(normalsFlat[nT*36 + 4*3 +0],
                                       normalsFlat[nT*36 + 4*3 +1],
                                       normalsFlat[nT*36 + 4*3 +2]);
                neNorms[3] = glm::vec3(normalsFlat[nT*36 + 6*3 +0],
                                       normalsFlat[nT*36 + 6*3 +1],
                                       normalsFlat[nT*36 + 6*3 +2]);
            }

            if (eEdge) {
                neNorms[4] = neNorms[5] = up;
            } else {
                neNorms[4] = glm::vec3(normalsFlat[eT*36 + 10*3 +0],
                                       normalsFlat[eT*36 + 10*3 +1],
                                       normalsFlat[eT*36 + 10*3 +2]);
                neNorms[5] = glm::vec3(normalsFlat[eT*36 + 0*3  +0],
                                       normalsFlat[eT*36 + 0*3  +1],
                                       normalsFlat[eT*36 + 0*3  +2]);
            }

            if (nEdge | eEdge) {
                neNorms[6] = neNorms[7] = up;
            } else {
                neNorms[6] = glm::vec3(normalsFlat[neT*36 + 7*3 +0],
                                       normalsFlat[neT*36 + 7*3 +1],
                                       normalsFlat[neT*36 + 7*3 +2]);
                neNorms[7] = glm::vec3(normalsFlat[neT*36 + 9*3 +0],
                                       normalsFlat[neT*36 + 9*3 +1],
                                       normalsFlat[neT*36 + 9*3 +2]);
            }

            // East South
            esNorms[0] = glm::vec3(normalsFlat[t*36 + 4*3 +0],
                                   normalsFlat[t*36 + 4*3 +1],
                                   normalsFlat[t*36 + 4*3 +2]);
            esNorms[1] = glm::vec3(normalsFlat[t*36 + 6*3 +0],
                                   normalsFlat[t*36 + 6*3 +1],
                                   normalsFlat[t*36 + 6*3 +2]);

            if (eEdge) {
                esNorms[2] = esNorms[3] = up;
            } else {
                esNorms[2] = glm::vec3(normalsFlat[eT*36 + 7*3 +0],
                                       normalsFlat[eT*36 + 7*3 +1],
                                       normalsFlat[eT*36 + 7*3 +2]);
                esNorms[3] = glm::vec3(normalsFlat[eT*36 + 9*3 +0],
                                       normalsFlat[eT*36 + 9*3 +1],
                                       normalsFlat[eT*36 + 9*3 +2]);
            }

            if (sEdge) {
                esNorms[4] = esNorms[5] = up;
            } else {
                esNorms[4] = glm::vec3(normalsFlat[sT*36 + 1*3 +0],
                                       normalsFlat[sT*36 + 1*3 +1],
                                       normalsFlat[sT*36 + 1*3 +2]);
                esNorms[5] = glm::vec3(normalsFlat[sT*36 + 3*3 +0],
                                       normalsFlat[sT*36 + 3*3 +1],
                                       normalsFlat[sT*36 + 3*3 +2]);
            }

            if (eEdge | sEdge) {
                esNorms[6] = esNorms[7] = up;
            } else {
                esNorms[6] = glm::vec3(normalsFlat[esT*36 + 10*3 +0],
                                       normalsFlat[esT*36 + 10*3 +1],
                                       normalsFlat[esT*36 + 10*3 +2]);
                esNorms[7] = glm::vec3(normalsFlat[esT*36 + 0*3  +0],
                                       normalsFlat[esT*36 + 0*3  +1],
                                       normalsFlat[esT*36 + 0*3  +2]);
            }


            // South West
            swNorms[0] = glm::vec3(normalsFlat[t*36 + 7*3 +0],
                                   normalsFlat[t*36 + 7*3 +1],
                                   normalsFlat[t*36 + 7*3 +2]);
            swNorms[1] = glm::vec3(normalsFlat[t*36 + 9*3 +0],
                                   normalsFlat[t*36 + 9*3 +1],
                                   normalsFlat[t*36 + 9*3 +2]);

            if (sEdge) {
                swNorms[2] = swNorms[3] = up;
            } else {
                swNorms[2] = glm::vec3(normalsFlat[sT*36 + 10*3 +0],
                                       normalsFlat[sT*36 + 10*3 +1],
                                       normalsFlat[sT*36 + 10*3 +2]);
                swNorms[3] = glm::vec3(normalsFlat[sT*36 + 0*3  +0],
                                       normalsFlat[sT*36 + 0*3  +1],
                                       normalsFlat[sT*36 + 0*3  +2]);
            }

            if (wEdge) {
                swNorms[4] = swNorms[5] = up;
            } else {
                swNorms[4] = glm::vec3(normalsFlat[wT*36 + 4*3 +0],
                                       normalsFlat[wT*36 + 4*3 +1],
                                       normalsFlat[wT*36 + 4*3 +2]);
                swNorms[5] = glm::vec3(normalsFlat[wT*36 + 6*3 +0],
                                       normalsFlat[wT*36 + 6*3 +1],
                                       normalsFlat[wT*36 + 6*3 +2]);
            }

            if (sEdge | wEdge) {
                swNorms[6] = swNorms[7] = up;
            } else {
                swNorms[6] = glm::vec3(normalsFlat[swT*36 + 1*3 +0],
                                       normalsFlat[swT*36 + 1*3 +1],
                                       normalsFlat[swT*36 + 1*3 +2]);
                swNorms[7] = glm::vec3(normalsFlat[swT*36 + 3*3 +0],
                                       normalsFlat[swT*36 + 3*3 +1],
                                       normalsFlat[swT*36 + 3*3 +2]);
            }


            // West North
            wnNorms[0] = glm::vec3(normalsFlat[t*36 + 10*3 +0],
                                   normalsFlat[t*36 + 10*3 +1],
                                   normalsFlat[t*36 + 10*3 +2]);
            wnNorms[1] = glm::vec3(normalsFlat[t*36 + 0*3  +0],
                                   normalsFlat[t*36 + 0*3  +1],
                                   normalsFlat[t*36 + 0*3  +2]);

            if (wEdge) {
                wnNorms[2] = wnNorms[3] = up;
            } else {
                wnNorms[2] = glm::vec3(normalsFlat[wT*36 + 1*3 +0],
                                       normalsFlat[wT*36 + 1*3 +1],
                                       normalsFlat[wT*36 + 1*3 +2]);
                wnNorms[3] = glm::vec3(normalsFlat[wT*36 + 3*3 +0],
                                       normalsFlat[wT*36 + 3*3 +1],
                                       normalsFlat[wT*36 + 3*3 +2]);
            }

            if (nEdge) {
                wnNorms[4] = wnNorms[5] = up;
            } else {
                wnNorms[4] = glm::vec3(normalsFlat[nT*36 + 7*3 +0],
                                       normalsFlat[nT*36 + 7*3 +1],
                                       normalsFlat[nT*36 + 7*3 +2]);
                wnNorms[5] = glm::vec3(normalsFlat[nT*36 + 9*3 +0],
                                       normalsFlat[nT*36 + 9*3 +1],
                                       normalsFlat[nT*36 + 9*3 +2]);
            }

            if (wEdge | nEdge) {
                wnNorms[6] = wnNorms[7] = up;
            } else {
                wnNorms[6] = glm::vec3(normalsFlat[wnT*36 + 4*3 +0],
                                       normalsFlat[wnT*36 + 4*3 +1],
                                       normalsFlat[wnT*36 + 4*3 +2]);
                wnNorms[7] = glm::vec3(normalsFlat[wnT*36 + 6*3 +0],
                                       normalsFlat[wnT*36 + 6*3 +1],
                                       normalsFlat[wnT*36 + 6*3 +2]);
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

            normals[t*36 + 1*3 +0] = neSum.x;
            normals[t*36 + 1*3 +1] = neSum.y;
            normals[t*36 + 1*3 +2] = neSum.z;
            normals[t*36 + 3*3 +0] = neSum.x;
            normals[t*36 + 3*3 +1] = neSum.y;
            normals[t*36 + 3*3 +2] = neSum.z;

            normals[t*36 + 4*3 +0] = esSum.x;
            normals[t*36 + 4*3 +1] = esSum.y;
            normals[t*36 + 4*3 +2] = esSum.z;
            normals[t*36 + 6*3 +0] = esSum.x;
            normals[t*36 + 6*3 +1] = esSum.y;
            normals[t*36 + 6*3 +2] = esSum.z;

            normals[t*36 + 7*3 +0] = swSum.x;
            normals[t*36 + 7*3 +1] = swSum.y;
            normals[t*36 + 7*3 +2] = swSum.z;
            normals[t*36 + 9*3 +0] = swSum.x;
            normals[t*36 + 9*3 +1] = swSum.y;
            normals[t*36 + 9*3 +2] = swSum.z;

            normals[t*36 + 10*3 +0] = wnSum.x;
            normals[t*36 + 10*3 +1] = wnSum.y;
            normals[t*36 + 10*3 +2] = wnSum.z;
            normals[t*36 + 0*3 +0] = wnSum.x;
            normals[t*36 + 0*3 +1] = wnSum.y;
            normals[t*36 + 0*3 +2] = wnSum.z;

            glm::vec3 normSum;
            for (int f = 0; f < 4; f++) {
                normSum += glm::vec3(normals[t*36 + f*9 +0], normals[t*36 + f*9 +1], normals[t*36 + f*9 +2]);
                normSum += glm::vec3(normals[t*36 + f*9 +3], normals[t*36 + f*9 +4], normals[t*36 + f*9 +5]);
            }
            normSum = glm::normalize(normSum);
            for (int f = 0; f < 4; f++) {
                normals[t*36 + f*9 +6] = normSum.x;
                normals[t*36 + f*9 +7] = normSum.y;
                normals[t*36 + f*9 +8] = normSum.z;
            }
        }
    }

    // Set tangents and remove non-visible tiles
    for (int t = 0; t < tileLen; t++) {
        for (int p = 0; p < 12; p++) {
            glm::vec3 normal(normals[t*36 + p*3 +0],
                             normals[t*36 + p*3 +1],
                             normals[t*36 + p*3 +2]);

            glm::vec3 tangent = sq::get_tangent(normal);
            tangents[t*36 + p*3 +0] = tangent.x;
            tangents[t*36 + p*3 +1] = tangent.y;
            tangents[t*36 + p*3 +2] = tangent.z;
        }

        if (!_visible[t]) {
            for (int c = 0; c < 36; c++) {
                points[t*36 + c] = 0.f;
            }
        }
    }

    GLuint vboPoints = 0;
    GLuint vboNormals = 0;
    GLuint vboTexcoords = 0;
    GLuint vboTangents = 0;

    gl::GenBuffers(1, &vboPoints);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboPoints);
    gl::BufferData(gl::ARRAY_BUFFER, vCount * 3 * sizeof(GLfloat), points, gl::STATIC_DRAW);

    gl::GenBuffers(1, &vboNormals);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboNormals);
    gl::BufferData(gl::ARRAY_BUFFER, vCount * 3 * sizeof(GLfloat), normals, gl::STATIC_DRAW);

    gl::GenBuffers(1, &vboTexcoords);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTexcoords);
    gl::BufferData(gl::ARRAY_BUFFER, vCount * 3 * sizeof(GLfloat), texcoords, gl::STATIC_DRAW);

    gl::GenBuffers(1, &vboTangents);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTangents);
    gl::BufferData(gl::ARRAY_BUFFER, vCount * 3 * sizeof(GLfloat), tangents, gl::STATIC_DRAW);

    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    gl::EnableVertexAttribArray(0);
    gl::EnableVertexAttribArray(1);
    gl::EnableVertexAttribArray(2);
    gl::EnableVertexAttribArray(3);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboPoints);
    gl::VertexAttribPointer(0, 3, gl::FLOAT, false, 0, NULL);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboNormals);
    gl::VertexAttribPointer(1, 3, gl::FLOAT, false, 0, NULL);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTexcoords);
    gl::VertexAttribPointer(2, 3, gl::FLOAT, false, 0, NULL);
    gl::BindBuffer(gl::ARRAY_BUFFER, vboTangents);
    gl::VertexAttribPointer(3, 3, gl::FLOAT, false, 0, NULL);

    /// Load Textures ///

    // Load Ambient Texture
    texAmbi = sq::tex2D_load_file("res/maps/" + _map + "/bakes/terrain.png", gl::R16F);
    if (texAmbi == nullptr) texAmbi = sq::tex2D_load_blank({1, 1}, gl::R16F);
    texAmbi->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texAmbi->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);

    // Load Normal Textures
    texNormArray = sq::tex2DArray_load_blank({128, 128, _texCount}, gl::RGB16F);
    texNormArray->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texNormArray->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
    for (uint i = 0; i < _texCount ; i++) {
        if (sq::tex2DArray_add_file(texNormArray, "res/textures/ground/norm/" + _texPaths[i], i))
            sq::tex2DArray_add_file(texNormArray, "res/misc/blanknorm.png", i);
    }

    // Load Diffuse Textures
    texDiffArray = sq::tex2DArray_load_blank({128, 128, _texCount}, gl::RGB16F);
    texDiffArray->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texDiffArray->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
    for (uint i = 0; i < _texCount ; i++) {
        if (sq::tex2DArray_add_file(texDiffArray, "res/textures/ground/diff/" + _texPaths[i], i))
            sq::tex2DArray_add_file(texDiffArray, "res/misc/blankdiff.png", i);
    }

    // Load Specular Textures
    texSpecArray = sq::tex2DArray_load_blank({128, 128, _texCount}, gl::RGB16F);
    texSpecArray->set_params(2, sq::MIN_MAG_FILTERS, sq::BOTH_LINEAR);
    texSpecArray->set_params(2, sq::S_T_WRAP, sq::BOTH_CLAMP_TO_EDGE);
    for (uint i = 0; i < _texCount ; i++) {
        if (sq::tex2DArray_add_file(texSpecArray, "res/textures/ground/spec/" + _texPaths[i], i))
            sq::tex2DArray_add_file(texSpecArray, "res/misc/blankspec.png", i);
    }

    return false;
}

#undef CHECK_BIT
