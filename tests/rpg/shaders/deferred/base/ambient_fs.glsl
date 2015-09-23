// GLSL Fragment Shader

// define SSAO

in vec2 texcrd;

#include builtin/blocks/camera
#include headers/blocks/ambient

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };
layout(std140, binding=1) uniform AMBIENTBLOCK { AmbientBlock LB; };

layout(binding=3) uniform sampler2D defrDiff;
layout(binding=4) uniform sampler2D defrNorm;

//layout(binding=8) uniform sampler2D texIrrd;

#ifdef SSAO
#include builtin/funcs/depth
layout(binding=0) uniform sampler2D texAmbOcc;
layout(binding=1) uniform sampler2D texDepHalf;
layout(binding=7) uniform sampler2D defrDepth;
#endif

out vec3 fragColour;

/*
/////////////////////////////////////

const vec3 normArr[26] = {
normalize(vec3(-1, -1, -1)), normalize(vec3(-1, -1,  0)), normalize(vec3(-1, -1, +1)),
normalize(vec3(-1,  0, -1)), normalize(vec3(-1,  0,  0)), normalize(vec3(-1,  0, +1)),
normalize(vec3(-1, +1, -1)), normalize(vec3(-1, +1,  0)), normalize(vec3(-1, +1, +1)),
normalize(vec3( 0, -1, -1)), normalize(vec3( 0, -1,  0)), normalize(vec3( 0, -1, +1)),
normalize(vec3( 0,  0, -1)),                              normalize(vec3( 0,  0, +1)),
normalize(vec3( 0, +1, -1)), normalize(vec3( 0, +1,  0)), normalize(vec3( 0, +1, +1)),
normalize(vec3(+1, -1, -1)), normalize(vec3(+1, -1,  0)), normalize(vec3(+1, -1, +1)),
normalize(vec3(+1,  0, -1)), normalize(vec3(+1,  0,  0)), normalize(vec3(+1,  0, +1)),
normalize(vec3(+1, +1, -1)), normalize(vec3(+1, +1,  0)), normalize(vec3(+1, +1, +1))
};

const uint NNN=0, NNZ=1, NNP=2, NZN=3, NZZ=4,  NZP=5, NPN=6, NPZ=7, NPP=8, ZNN=9, ZNZ=10, ZNP=11, ZZN=12,
ZZP=13, ZPN=14, ZPZ=15, ZPP=16, PNN=17, PNZ=18, PNP=19, PZN=20, PZZ=21, PZP=22, PPN=23, PPZ=24, PPP=25;


vec3 barycentric(vec3 _p, vec3 _a, vec3 _b, vec3 _c) {
    vec3 v0 = vec3(_b - _a);
    vec3 v1 = vec3(_c - _a);
    vec3 v2 = vec3(_p - _a);
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    return vec3(1.f - v - w, v, w);
}

uvec2 calc_tri_yz(vec3 _norm, uint _t0, uint _t1, uint _t2, uint _a, uint _b, uint _c, uint _d, uint _e, uint _f) {
    bool sideA = dot(_norm, normArr[_t0]) > 0.f;
    bool sideB = dot(_norm, normArr[_t1]) > 0.f;
    bool sideC = dot(_norm, normArr[_t2]) > 0.f;
    if (sideA && !sideB)           return uvec2(_a, _b);
    if (sideA && sideB && !sideC)  return uvec2(_b, _c);
    if (sideA && sideC)            return uvec2(_c, _d);
    if (!sideA && sideB)           return uvec2(_d, _e);
    if (!sideA && !sideB && sideC) return uvec2(_e, _f);
    if (!sideA && !sideC)          return uvec2(_f, _a);
}

vec3 sample_env_sdv(uint _probe, vec3 _norm, sampler2D _tex) {
    uint xOffs = _probe % 32 * 26, yOffs = _probe / 32;
    bool east = _norm.x >= 0.f, north = _norm.y >= 0.f, up = _norm.z >= 0.f; 

    uvec3 indices = uvec3(0u);
    if ( east &&  north &&  up) indices = uvec3(PPP, calc_tri_yz(_norm, NPZ, ZPN, PZN, ZZP, ZPP, ZPZ, PPZ, PZZ, PZP));
    if ( east &&  north && !up) indices = uvec3(PPN, calc_tri_yz(_norm, PNZ, PZP, ZPP, ZZN, PZN, PZZ, PPZ, ZPZ, ZPN));
    if ( east && !north &&  up) indices = uvec3(PNP, calc_tri_yz(_norm, PPZ, PZN, ZNN, ZZP, PZP, PZZ, PNZ, ZNZ, ZNP));
    if ( east && !north && !up) indices = uvec3(PNN, calc_tri_yz(_norm, NNZ, ZNP, PZP, ZZN, ZNN, ZNZ, PNZ, PZZ, PZN));
    if (!east &&  north &&  up) indices = uvec3(NPP, calc_tri_yz(_norm, NNZ, NZN, ZPN, ZZP, NZP, NZZ, NPZ, ZPZ, ZPP));
    if (!east &&  north && !up) indices = uvec3(NPN, calc_tri_yz(_norm, PPZ, ZPP, NZP, ZZN, ZPN, ZPZ, NPZ, NZZ, NZN));
    if (!east && !north &&  up) indices = uvec3(NNP, calc_tri_yz(_norm, PNZ, ZNN, NZN, ZZP, ZNP, ZNZ, NNZ, NZZ, NZP));
    if (!east && !north && !up) indices = uvec3(NNN, calc_tri_yz(_norm, NPZ, NZP, ZNP, ZZN, NZN, NZZ, NNZ, ZNZ, ZNN));
    vec3 weights = barycentric(_norm, normArr[indices.x], normArr[indices.y], normArr[indices.z]);
//    vec3 weights = barycentric(cubeNorm, normArr[0], normArr[1], normArr[2]);

    return texelFetch(_tex, ivec2(xOffs + indices.x, yOffs), 0).rgb * weights.x
         + texelFetch(_tex, ivec2(xOffs + indices.y, yOffs), 0).rgb * weights.y
         + texelFetch(_tex, ivec2(xOffs + indices.z, yOffs), 0).rgb * weights.z;
}

//////////////////////////
*/


void main() {
    vec3 value = texture(defrDiff, texcrd).rgb;

    #ifdef SSAO
    value *= nearest_depth_sca(texcrd, texAmbOcc, defrDepth, texDepHalf, 0.05f, CB.rmin, CB.rmax);
    #endif

    fragColour = value * LB.colour;

/*    vec3 v_pos = get_view_pos(texcrd);
    vec4 wp = CB.invView * vec4(v_pos, 1.f);
    vec3 w_pos = wp.xyz / wp.w;

    vec3 v_norm = normalize(texture(defrNorm, texcrd).rgb * 2.f - 1.f);
    vec3 w_norm = normalize(vec3(CB.trnView * vec4(v_norm, 0.f)));

//    float mapX = (w_pos.x + 16.9f) / 33.8f;
//    float mapY = (w_pos.y + 7.5f)  / 15.f;
//    float mapZ = (w_pos.z - 0.2f)  / 18.f;
//    vec3 mapPos = vec3(mapX, mapY, mapZ);

//    uint pX = uint(mapX * 15.f);
//    uint pY = uint(mapY * 9.f);
//    uint pZ = uint(mapZ * 10.f);

//    uint probe = pZ * 9u * 15u + pY * 15u + pX;


    float mapX = (w_pos.x + 16.f) / 32.f;
    float mapY = (w_pos.y + 7.f) / 14.f;
    float mapZ = (w_pos.z - 0.f) / 16.f;
    vec3 mapPos = vec3(mapX, mapY, mapZ);

    int pX = max(int(mapX * 7.f - 0.5f), 0);
    int pY = max(int(mapY * 3.f - 0.5f), 0);
    int pZ = max(int(mapZ * 3.f - 0.5f), 0);

//    uint pX = uint(round(mapX * 15.f));
//    uint pY = uint(round(mapY * 9.f));
//    uint pZ = uint(round(mapZ * 10.f));

    int pXB = min(pX + 1, 6);
    int pYB = min(pY + 1, 2);
    int pZB = min(pZ + 1, 2);

    uint probe000 = pZ  * 3u * 7u + pY  * 7u + pX ;
    uint probe001 = pZB * 3u * 7u + pY  * 7u + pX ;
    uint probe010 = pZ  * 3u * 7u + pYB * 7u + pX ;
    uint probe011 = pZB * 3u * 7u + pYB * 7u + pX ;
    uint probe100 = pZ  * 3u * 7u + pY  * 7u + pXB;
    uint probe101 = pZB * 3u * 7u + pY  * 7u + pXB;
    uint probe110 = pZ  * 3u * 7u + pYB * 7u + pXB;
    uint probe111 = pZB * 3u * 7u + pYB * 7u + pXB;

//    w_norm = reflect(CB.dir, w_norm);

    vec3 samp000 = sample_env_sdv(probe000, w_norm, texIrrd);
    vec3 samp001 = sample_env_sdv(probe001, w_norm, texIrrd);
    vec3 samp010 = sample_env_sdv(probe010, w_norm, texIrrd);
    vec3 samp011 = sample_env_sdv(probe011, w_norm, texIrrd);
    vec3 samp100 = sample_env_sdv(probe100, w_norm, texIrrd);
    vec3 samp101 = sample_env_sdv(probe101, w_norm, texIrrd);
    vec3 samp110 = sample_env_sdv(probe110, w_norm, texIrrd);
    vec3 samp111 = sample_env_sdv(probe111, w_norm, texIrrd);

    float xPWeight = ((mapX * 7.f - 0.5f) - float(pX));
    float yPWeight = ((mapY * 3.f - 0.5f) - float(pY));
    float zPWeight = ((mapZ * 3.f - 0.5f) - float(pZ));
    float xNWeight = 1.f - xPWeight;
    float yNWeight = 1.f - yPWeight;
    float zNWeight = 1.f - zPWeight;

    fragColour = samp000 * xNWeight * yNWeight * zNWeight
               + samp001 * xNWeight * yNWeight * zPWeight
               + samp010 * xNWeight * yPWeight * zNWeight
               + samp011 * xNWeight * yPWeight * zPWeight
               + samp100 * xPWeight * yNWeight * zNWeight
               + samp101 * xPWeight * yNWeight * zPWeight
               + samp110 * xPWeight * yPWeight * zNWeight
               + samp111 * xPWeight * yPWeight * zPWeight;

    fragColour *= value * 1.5f;
    fragColour += 0.1f * value; */

//    fragColour = sample_env_sdv(probe, w_norm, texIrrd);

//    fragColour = vec3(xPWeight, yPWeight, zPWeight);
    
//    fragColour = sample_env_sdv(probe, reflect(CB.dir, w_norm), texIrrd);

//    fragColour = vec3(float(pX)/15.f, float(pY)/9.f, float(pZ)/10.f);

//    fragColour = vec3(mapX);
}
