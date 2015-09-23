// GLSL Fragment Shader

#pragma optionNV (unroll none)

in vec3 o_pos;

layout(binding=0) uniform sampler2D texIrrd;

uniform uint probe;

out vec3 fragColour;


#ifndef SUBDIV

vec3 get_harmonics(vec3 _norm, uint _xOffs, uint _yOffs) {
//    const float SH_c0 = 1.15f;
//    const float SH_c1 = 2.42f;
//    const float SH_c2 = 0.52f;
//    const float SH_c3 = 3.14f;
//    const float SH_c4 = 0.55f;

//    const float SH_c0 = 1.f;
//    const float SH_c1 = 1.8f;
//    const float SH_c2 = 0.83f;
//    const float SH_c3 = 2.9f;
//    const float SH_c4 = 0.58f;

    const float SH_c0 = 0.282095f * 3.141592654f;
    const float SH_c1 = 0.488603f * 3.141592654f;
    const float SH_c2 = 1.092548f * 3.141592654f;
    const float SH_c3 = 0.315392f * 3.141592654f;
    const float SH_c4 = 0.546274f * 3.141592654f;

//    const float SH_c0 = 1.f;
//    const float SH_c1 = 1.f;
//    const float SH_c2 = 1.f;
//    const float SH_c3 = 1.f;
//    const float SH_c4 = 1.f;

    vec3 l00 = texelFetch(texIrrd, ivec2(_xOffs + 0, _yOffs), 0).rgb;
    vec3 l10 = texelFetch(texIrrd, ivec2(_xOffs + 1, _yOffs), 0).rgb;
    vec3 l11 = texelFetch(texIrrd, ivec2(_xOffs + 2, _yOffs), 0).rgb;
    vec3 l12 = texelFetch(texIrrd, ivec2(_xOffs + 3, _yOffs), 0).rgb;
    vec3 l20 = texelFetch(texIrrd, ivec2(_xOffs + 4, _yOffs), 0).rgb;
    vec3 l21 = texelFetch(texIrrd, ivec2(_xOffs + 5, _yOffs), 0).rgb;
    vec3 l22 = texelFetch(texIrrd, ivec2(_xOffs + 6, _yOffs), 0).rgb;
    vec3 l23 = texelFetch(texIrrd, ivec2(_xOffs + 7, _yOffs), 0).rgb;
    vec3 l24 = texelFetch(texIrrd, ivec2(_xOffs + 8, _yOffs), 0).rgb;

    float x = _norm.x, y = _norm.y, z = _norm.z;

    vec3 result = (
        l00 * SH_c0 +
        l10 * SH_c1 * x +
        l11 * SH_c1 * y + 
        l12 * SH_c1 * z +
        l20 * SH_c2 * x * y +
        l21 * SH_c2 * y * z +
        l22 * SH_c2 * z * x +
        l23 * SH_c3 * (3.f * z * z - 1.f) +
        l24 * SH_c4 * (x * x - y * y)
    );

    return max(result, vec3(0.f));
}

void main() {
    uint xOffs = probe % 32u * 16u, yOffs = probe / 32u;
    fragColour = get_harmonics(normalize(o_pos), xOffs, yOffs);
//    fragColour = normalize(o_pos) * 0.5f + 0.5f;
}


#else

//////////////////////////////

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


void main() {
    fragColour = sample_env_sdv(probe, normalize(o_pos), texIrrd);
}

#endif
