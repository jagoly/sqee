// GLSL Fragment Shader

layout(binding=0) uniform sampler2DArray texIrrd;

out vec3 fragColour;


#ifndef SUBDIV

/*
const mat3 matArr[6] = {
mat3( 1.f,  0.f,  0.f,  0.f,  0.f, -1.f,  0.f,  1.f,  0.f),
mat3( 0.f,  0.f, -1.f, -1.f,  0.f,  0.f,  0.f,  1.f,  0.f),
mat3(-1.f,  0.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,  0.f),
mat3( 0.f,  0.f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f),
mat3( 1.f,  0.f,  0.f,  0.f, -1.f,  0.f,  0.f,  0.f, -1.f),
mat3( 1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  0.f,  0.f,  1.f)
};
*/

const mat3 matArr[6] = {
inverse(mat3( 1.f,  0.f,  0.f,  0.f,  0.f, -1.f,  0.f,  1.f,  0.f)),
inverse(mat3( 0.f,  0.f, -1.f, -1.f,  0.f,  0.f,  0.f,  1.f,  0.f)),
inverse(mat3(-1.f,  0.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,  0.f)),
inverse(mat3( 0.f,  0.f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f)),
inverse(mat3( 1.f,  0.f,  0.f,  0.f, -1.f,  0.f,  0.f,  0.f, -1.f)),
inverse(mat3( 1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  0.f,  0.f,  1.f))
};


float harmonics(uint _pixid, vec3 _norm) {
    float x = _norm.x, y = _norm.y, z = _norm.x;

    if (_pixid == 0u) return 1.f;
    else if (_pixid == 1u) return x;
    else if (_pixid == 2u) return y;
    else if (_pixid == 3u) return z;
    else if (_pixid == 4u) return x * y;
    else if (_pixid == 5u) return y * z;
    else if (_pixid == 6u) return z * x;
    else if (_pixid == 7u) return 3.f * z * z - 1.f;
    else return x * x - y * y;
}

vec3 sample_side(uint _pixid, uint _face, uint _yOffs, uint _zOffs) {
    vec3 result = vec3(0.f);
    float divider = 0.f;

    for (uint x = 0u; x < 32u; ++x) {
        for (uint y = 0u; y < 32u; ++y) {
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f;
            vec3 normal = normalize(vec3(sidecrd*2.f-1.f, -1.f));
            float sharm = harmonics(_pixid, matArr[_face] * normal);
            ivec3 texcrd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec3 texel = texelFetch(texIrrd, texcrd, 0).rgb;
            result += texel * sharm * -normal.z; 
            divider += -normal.z;
        }
    }

    return result / divider;
}

void main() {
    uint pixid = uint(gl_FragCoord.x) % 16;
    if (pixid >= 9u) discard;

    uint probe = uint(gl_FragCoord.y) * 32u + uint(gl_FragCoord.x) / 16u;
    uint yOffs = probe % 64u * 32u; uint zOffs = probe / 64u;

    fragColour = (
        sample_side(pixid, 0u, yOffs, zOffs) +
        sample_side(pixid, 1u, yOffs, zOffs) +
        sample_side(pixid, 2u, yOffs, zOffs) +
        sample_side(pixid, 3u, yOffs, zOffs) +
        sample_side(pixid, 4u, yOffs, zOffs) +
        sample_side(pixid, 5u, yOffs, zOffs)
    ) / 6.f;
}

#else

////////////////////////////////////////

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

const mat3 matArr[6] = {
inverse(mat3( 1.f,  0.f,  0.f,  0.f,  0.f, -1.f,  0.f,  1.f,  0.f)),
inverse(mat3( 0.f,  0.f, -1.f, -1.f,  0.f,  0.f,  0.f,  1.f,  0.f)),
inverse(mat3(-1.f,  0.f,  0.f,  0.f,  0.f,  1.f,  0.f,  1.f,  0.f)),
inverse(mat3( 0.f,  0.f,  1.f,  1.f,  0.f,  0.f,  0.f,  1.f,  0.f)),
inverse(mat3( 1.f,  0.f,  0.f,  0.f, -1.f,  0.f,  0.f,  0.f, -1.f)),
inverse(mat3( 1.f,  0.f,  0.f,  0.f,  1.f,  0.f,  0.f,  0.f,  1.f))
};

////////////////////////////////////////

vec4 sample_t(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint y = 16u; y < 32u; ++y) {
        for (uint x = 0u; x < 32u; ++x) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

vec4 sample_r(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint x = 16u; x < 32u; ++x) {
        for (uint y = 0u; y < 32u; ++y) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

vec4 sample_b(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint y = 0u; y < 16u; ++y) {
        for (uint x = 0u; x < 32u; ++x) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

vec4 sample_l(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint x = 0u; x < 16u; ++x) {
        for (uint y = 0u; y < 32u; ++y) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

////////////////////////////////////////

vec4 sample_face(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint x = 0u; x < 32u; ++x) {
        for (uint y = 0u; y < 32u; ++y) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

////////////////////////////////////////

vec4 sample_tr(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint x = 1u; x < 32u; ++x) {
        for (uint y = 32u-x; y < 32u; ++y) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

vec4 sample_br(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint x = 1u; x < 32u; ++x) {
        for (uint y = 0u; y < x; ++y) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

vec4 sample_tl(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint x = 0u; x < 31u; ++x) {
        for (uint y = x+1u; y < 32u; ++y) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

vec4 sample_bl(uint _ind, uint _face, uint _yOffs, uint _zOffs) {
    vec4 retVal = vec4(0.f);
    for (uint x = 0u; x < 31u; ++x) {
        for (uint y = 0u; y < 31-x; ++y) {
            ivec3 crd = ivec3(_face*32+x, _yOffs+y, _zOffs);
            vec2 sidecrd = (vec2(x, y) + 0.5f) / 32.f * 2.f - 1.f;
            vec3 normal = matArr[_face] * normalize(vec3(sidecrd, -1.f));
            float factor = 0.4f + dot(normal, normArr[_ind]);
            retVal.rgb += texelFetch(texIrrd, crd, 0).rgb * factor;
            retVal.a += factor;
    }} return retVal;
}

////////////////////////////////////////


void main() {
    uint pixid = uint(gl_FragCoord.x) % 26;
//    if (pixid >= 26u) discard;

    vec3 normal = normArr[pixid];
    vec4 value = vec4(0.f);
    float divider = 0.f;

    uint probe = uint(gl_FragCoord.y) * 32u + uint(gl_FragCoord.x) / 26u;
    uint yOffs = probe % 64u * 32u; uint zOffs = probe / 64u;


    ////////////////////////////////////////

    if (pixid == 0u) { // WEST, SOUTH, DOWN
        value += sample_face(0u, 3u, yOffs, zOffs);
        value += sample_face(0u, 2u, yOffs, zOffs);
        value += sample_face(0u, 5u, yOffs, zOffs);
    }

    if (pixid == 1u) { // WEST, SOUTH, CENTRE
        value += sample_face(1u, 3u, yOffs, zOffs);
        value += sample_face(1u, 2u, yOffs, zOffs);
        value += sample_tl(1u, 4u, yOffs, zOffs);
        value += sample_bl(1u, 5u, yOffs, zOffs);
    }

    if (pixid == 2u) { // WEST, SOUTH, UP
        value += sample_face(2u, 3u, yOffs, zOffs);
        value += sample_face(2u, 2u, yOffs, zOffs);
        value += sample_face(2u, 4u, yOffs, zOffs);
    }

    ////////////////////

    if (pixid == 3u) { // WEST, CENTRE, DOWN
        value += sample_face(3u, 3u, yOffs, zOffs);
        value += sample_face(3u, 5u, yOffs, zOffs);
        value += sample_bl(3u, 0u, yOffs, zOffs);
        value += sample_br(3u, 2u, yOffs, zOffs);
    }

    if (pixid == 4u) { // WEST, CENTRE, CENTRE
        value += sample_face(4u, 3u, yOffs, zOffs);
        value += sample_l(4u, 0u, yOffs, zOffs);
        value += sample_r(4u, 2u, yOffs, zOffs);
        value += sample_l(4u, 4u, yOffs, zOffs);
        value += sample_l(4u, 5u, yOffs, zOffs);
    }

    if (pixid == 5u) { // WEST, CENTRE, UP
        value += sample_face(5u, 3u, yOffs, zOffs);
        value += sample_face(5u, 4u, yOffs, zOffs);
        value += sample_tl(5u, 0u, yOffs, zOffs);
        value += sample_tr(5u, 2u, yOffs, zOffs);
    }

    ////////////////////

    if (pixid == 6u) { // WEST, NORTH, DOWN
        value += sample_face(6u, 3u, yOffs, zOffs);
        value += sample_face(6u, 0u, yOffs, zOffs);
        value += sample_face(6u, 5u, yOffs, zOffs);
    }

    if (pixid == 7u) { // WEST, NORTH, CENTRE
        value += sample_face(7u, 3u, yOffs, zOffs);
        value += sample_face(7u, 0u, yOffs, zOffs);
        value += sample_bl(7u, 4u, yOffs, zOffs);
        value += sample_tl(7u, 5u, yOffs, zOffs);
    }

    if (pixid == 8u) { // WEST, NORTH, UP
        value += sample_face(8u, 3u, yOffs, zOffs);
        value += sample_face(8u, 0u, yOffs, zOffs);
        value += sample_face(8u, 4u, yOffs, zOffs);
    }

    ////////////////////////////////////////

    if (pixid == 9u) { // CENTRE, SOUTH, DOWN
        value += sample_face(9u, 2u, yOffs, zOffs);
        value += sample_face(9u, 5u, yOffs, zOffs);
        value += sample_br(9u, 1u, yOffs, zOffs);
        value += sample_bl(9u, 3u, yOffs, zOffs);
    }

    if (pixid == 10u) { // CENTRE, SOUTH, CENTRE
        value += sample_face(10u, 2u, yOffs, zOffs);
        value += sample_r(10u, 1u, yOffs, zOffs);
        value += sample_l(10u, 3u, yOffs, zOffs);
        value += sample_t(10u, 4u, yOffs, zOffs);
        value += sample_b(10u, 5u, yOffs, zOffs);
    }

    if (pixid == 11u) { // CENTRE, SOUTH, UP
        value += sample_face(11u, 2u, yOffs, zOffs);
        value += sample_face(11u, 4u, yOffs, zOffs);
        value += sample_tr(11u, 1u, yOffs, zOffs);
        value += sample_tl(11u, 3u, yOffs, zOffs);
    }

    ////////////////////

    if (pixid == 12u) { // CENTRE, CENTRE, DOWN
        value += sample_face(12u, 5u, yOffs, zOffs);
        value += sample_b(12u, 0u, yOffs, zOffs);
        value += sample_b(12u, 1u, yOffs, zOffs);
        value += sample_b(12u, 2u, yOffs, zOffs);
        value += sample_b(12u, 3u, yOffs, zOffs);
    }

    if (pixid == 13u) { // CENTRE, CENTRE, UP
        value += sample_face(13u, 4u, yOffs, zOffs);
        value += sample_t(13u, 0u, yOffs, zOffs);
        value += sample_t(13u, 1u, yOffs, zOffs);
        value += sample_t(13u, 2u, yOffs, zOffs);
        value += sample_t(13u, 3u, yOffs, zOffs);
    }

    ////////////////////

    if (pixid == 14u) { // CENTRE, NORTH, DOWN
        value += sample_face(14u, 0u, yOffs, zOffs);
        value += sample_face(14u, 5u, yOffs, zOffs);
        value += sample_bl(14u, 1u, yOffs, zOffs);
        value += sample_br(14u, 3u, yOffs, zOffs);
    }

    if (pixid == 15u) { // CENTRE, NORTH, CENTRE
        value += sample_face(15u, 0u, yOffs, zOffs);
        value += sample_l(15u, 1u, yOffs, zOffs);
        value += sample_r(15u, 3u, yOffs, zOffs);
        value += sample_b(15u, 4u, yOffs, zOffs);
        value += sample_t(15u, 5u, yOffs, zOffs);
    }

    if (pixid == 16u) { // CENTRE, NORTH, UP
        value += sample_face(16u, 0u, yOffs, zOffs);
        value += sample_face(16u, 4u, yOffs, zOffs);
        value += sample_tl(16u, 1u, yOffs, zOffs);
        value += sample_tr(16u, 3u, yOffs, zOffs);
    }

    ////////////////////////////////////////

    if (pixid == 17u) { // EAST, SOUTH, DOWN
        value += sample_face(17u, 1u, yOffs, zOffs);
        value += sample_face(17u, 2u, yOffs, zOffs);
        value += sample_face(17u, 5u, yOffs, zOffs);
    }

    if (pixid == 18u) { // EAST, SOUTH, CENTRE
        value += sample_face(18u, 1u, yOffs, zOffs);
        value += sample_face(18u, 2u, yOffs, zOffs);
        value += sample_tr(18u, 4u, yOffs, zOffs);
        value += sample_br(18u, 5u, yOffs, zOffs);
    }

    if (pixid == 19u) { // EAST, SOUTH, UP
        value += sample_face(19u, 1u, yOffs, zOffs);
        value += sample_face(19u, 2u, yOffs, zOffs);
        value += sample_face(19u, 4u, yOffs, zOffs);
    }

    ////////////////////

    if (pixid == 20u) { // EAST, CENTRE, DOWN
        value += sample_face(20u, 1u, yOffs, zOffs);
        value += sample_face(20u, 5u, yOffs, zOffs);
        value += sample_br(20u, 0u, yOffs, zOffs);
        value += sample_bl(20u, 2u, yOffs, zOffs);
    }

    if (pixid == 21u) { // EAST, CENTRE, CENTRE
        value += sample_face(21u, 1u, yOffs, zOffs);
        value += sample_r(21u, 0u, yOffs, zOffs);
        value += sample_l(21u, 2u, yOffs, zOffs);
        value += sample_r(21u, 4u, yOffs, zOffs);
        value += sample_r(21u, 5u, yOffs, zOffs);
    }

    if (pixid == 22u) { // EAST, CENTRE, UP
        value += sample_face(22u, 1u, yOffs, zOffs);
        value += sample_face(22u, 4u, yOffs, zOffs);
        value += sample_tr(22u, 0u, yOffs, zOffs);
        value += sample_tl(22u, 2u, yOffs, zOffs);
    }

    ////////////////////

    if (pixid == 23u) { // EAST, NORTH, DOWN
        value += sample_face(23u, 1u, yOffs, zOffs);
        value += sample_face(23u, 0u, yOffs, zOffs);
        value += sample_face(23u, 5u, yOffs, zOffs);
    }

    if (pixid == 24u) { // EAST, NORTH, CENTRE
        value += sample_face(24u, 1u, yOffs, zOffs);
        value += sample_face(24u, 0u, yOffs, zOffs);
        value += sample_br(24u, 4u, yOffs, zOffs);
        value += sample_tr(24u, 5u, yOffs, zOffs);
    }

    if (pixid == 25u) { // EAST, NORTH, UP
        value += sample_face(25u, 1u, yOffs, zOffs);
        value += sample_face(25u, 0u, yOffs, zOffs);
        value += sample_face(25u, 4u, yOffs, zOffs);
    }

    ////////////////////////////////////////

    fragColour = value.rgb / value.a;
}

#endif
