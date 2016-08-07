// GLSL Header File

#include builtin/funcs/random
#include builtin/disks/uniform

float get_bias(vec3 _normal, vec3 _lightDir) {
    float lightDot = dot(_normal, -_lightDir);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;
    return clamp(0.001f * magicTan, 0.f, 0.01f);
}

float sample_shadow(vec3 _sc, int _layer, float _bias, sampler2DArrayShadow _tex) {
    return texture(_tex, vec4(_sc.xy, _layer, _sc.z - _bias));
}

float sample_shadow_x4(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy), vis = 0.f;
    float s = sin(angle), c = cos(angle);
    for (int ind = 0; ind < 4; ind++) {
        vec2 offs = disk4[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    } return vis / 4.f;
}

float sample_shadow_x6(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy), vis = 0.f;
    float s = sin(angle), c = cos(angle);
    for (int ind = 0; ind < 6; ind++) {
        vec2 offs = disk6[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    } return vis / 6.f;
}

float sample_shadow_x8(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy), vis = 0.f;
    float s = sin(angle), c = cos(angle);
    for (int ind = 0; ind < 8; ind++) {
        vec2 offs = disk8[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    } return vis / 8.f;
}

float sample_shadow_x12(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy), vis = 0.f;
    float s = sin(angle), c = cos(angle);
    for (int ind = 0; ind < 12; ind++) {
        vec2 offs = disk12[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    } return vis / 12.f;
}

float sample_shadow_x16(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy), vis = 0.f;
    float s = sin(angle), c = cos(angle);
    for (int ind = 0; ind < 16; ind++) {
        vec2 offs = disk16[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    } return vis / 16.f;
}

float sample_shadow_x24(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy), vis = 0.f;
    float s = sin(angle), c = cos(angle);
    for (int ind = 0; ind < 24; ind++) {
        vec2 offs = disk24[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    } return vis / 24.f;
}
