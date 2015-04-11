// GLSL Header File

// require "float rand(vec2)" "vec2 disk4[4]" "vec2 disk8[8]" "vec2 disk16[16]"

float get_bias(vec3 _normal, vec3 _lightDir) {
    float lightDot = dot(_normal, -_lightDir);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;
    return clamp(0.006f * magicTan, 0.f, 0.03f);
}

float sample_shadow(vec3 _sc, float _bias, sampler2DShadow _tex) {
    return texture(_tex, vec3(_sc.xy, _sc.z - _bias));
}

float sample_shadow_d4(vec3 _sc, float _bias, sampler2DShadow _tex) {
    float angle = rand(_sc.xy);
    float s = sin(angle);
    float c = cos(angle);
    float vis = 0.f;
    for (int ind = 0; ind < 4; ind++) {
        vec2 offs = disk4[ind] * (1.f / 512.f);
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec3(_sc.x+offs.x, _sc.y+offs.y, _sc.z - _bias));
    }
    return vis / 4.f;
}

float sample_shadow_d8(vec3 _sc, float _bias, sampler2DShadow _tex) {
    float angle = rand(_sc.xy);
    float s = sin(angle);
    float c = cos(angle);
    float vis = 0.f;
    for (int ind = 0; ind < 8; ind++) {
        vec2 offs = disk8[ind] * (1.f / 512.f);
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec3(_sc.x+offs.x, _sc.y+offs.y, _sc.z - _bias));
    }
    return vis / 8.f;
}

float sample_shadow_d16(vec3 _sc, float _bias, sampler2DShadow _tex) {
    float angle = rand(_sc.xy);
    float s = sin(angle);
    float c = cos(angle);
    float vis = 0.f;
    for (int ind = 0; ind < 16; ind++) {
        vec2 offs = disk16[ind] * (1.f / 512.f);
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec3(_sc.x+offs.x, _sc.y+offs.y, _sc.z - _bias));
    }
    return vis / 16.f;
}