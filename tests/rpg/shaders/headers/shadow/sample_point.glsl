// GLSL Header File

// require "float rand(vec2)" "vec2 disk4[4]" "vec2 disk8[8]" "vec2 disk16[16]"

float get_bias(vec3 _normal, vec3 _lightDir) {
    float lightDot = dot(_normal, -_lightDir);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;
    return clamp(0.006f * magicTan, 0.f, 0.03f);
}

float sample_shadow(vec4 _sc, float _bias, samplerCubeShadow _tex) {
    return texture(_tex, vec4(_sc.xyz, _sc.w-_bias));
}

float sample_shadow_x4(vec4 _sc, float _bias, samplerCubeShadow _tex) {
    float angle = rand3(_sc.xyz);
    float s = sin(angle), c = cos(angle);
    vec3 absVec = abs(_sc.xyz); int ind = 0;
    if (absVec.x > absVec.y && absVec.x > absVec.z) ind = 0;
    if (absVec.y > absVec.z && absVec.y > absVec.x) ind = 1;
    if (absVec.z > absVec.x && absVec.z > absVec.y) ind = 2;

    float vis = 0.f;
    for (int i = 0; i < 4; i++) {
        vec2 offs = disk4[i] * (1.f / 256.f);
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vec3 sampDir = _sc.xyz;
        if (ind == 0) sampDir.y += offs.x, sampDir.z += offs.y;
        if (ind == 1) sampDir.z += offs.x, sampDir.x += offs.y;
        if (ind == 2) sampDir.x += offs.x, sampDir.y += offs.y;
        vis += texture(_tex, vec4(normalize(sampDir), _sc.w - _bias));
    }
    return vis / 4.f;
}

float sample_shadow_x8(vec4 _sc, float _bias, samplerCubeShadow _tex) {
    float angle = rand3(_sc.xyz);
    float s = sin(angle), c = cos(angle);
    vec3 absVec = abs(_sc.xyz); int ind = 0;
    if (absVec.x > absVec.y && absVec.x > absVec.z) ind = 0;
    if (absVec.y > absVec.z && absVec.y > absVec.x) ind = 1;
    if (absVec.z > absVec.x && absVec.z > absVec.y) ind = 2;

    float vis = 0.f;
    for (int i = 0; i < 8; i++) {
        vec2 offs = disk8[i] * (1.f / 256.f);
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vec3 sampDir = _sc.xyz;
        if (ind == 0) sampDir.y += offs.x, sampDir.z += offs.y;
        if (ind == 1) sampDir.z += offs.x, sampDir.x += offs.y;
        if (ind == 2) sampDir.x += offs.x, sampDir.y += offs.y;
        vis += texture(_tex, vec4(normalize(sampDir), _sc.w - _bias));
    }
    return vis / 8.f;
}

float sample_shadow_x16(vec4 _sc, float _bias, samplerCubeShadow _tex) {
    float angle = rand3(_sc.xyz);
    float s = sin(angle), c = cos(angle);
    vec3 absVec = abs(_sc.xyz); int ind = 0;
    if (absVec.x > absVec.y && absVec.x > absVec.z) ind = 0;
    if (absVec.y > absVec.z && absVec.y > absVec.x) ind = 1;
    if (absVec.z > absVec.x && absVec.z > absVec.y) ind = 2;

    float vis = 0.f;
    for (int i = 0; i < 16; i++) {
        vec2 offs = disk16[i] * (1.f / 256.f);
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vec3 sampDir = _sc.xyz;
        if (ind == 0) sampDir.y += offs.x, sampDir.z += offs.y;
        if (ind == 1) sampDir.z += offs.x, sampDir.x += offs.y;
        if (ind == 2) sampDir.x += offs.x, sampDir.y += offs.y;
        vis += texture(_tex, vec4(normalize(sampDir), _sc.w - _bias));
    }
    return vis / 16.f;
}
