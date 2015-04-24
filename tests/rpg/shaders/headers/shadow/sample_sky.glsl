// GLSL Header File

// require "float rand2(vec2)" "vec2 disk6[6]" "vec2 disk12[12]" "vec2 disk24[24]"

float get_bias(vec3 _normal, vec3 _lightDir) {
    float lightDot = dot(_normal, -_lightDir);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;
    return clamp(0.0006f * magicTan, 0.f, 0.003f);
}

float sample_shadow(vec3 _sc, int _layer, float _bias, sampler2DArrayShadow _tex) {
    return texture(_tex, vec4(_sc.xy, _layer, _sc.z - _bias));
}

float sample_shadow_x6(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy);
    float s = sin(angle);
    float c = cos(angle);
    float vis = 0.f;
    for (int ind = 0; ind < 6; ind++) {
        vec2 offs = disk6[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    }
    return vis / 6.f;
}

float sample_shadow_x12(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy);
    float s = sin(angle);
    float c = cos(angle);
    float vis = 0.f;
    for (int ind = 0; ind < 12; ind++) {
        vec2 offs = disk12[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    }
    return vis / 12.f;
}

float sample_shadow_x24(vec3 _sc, int _layer, float _bias, float _radius, sampler2DArrayShadow _tex) {
    float angle = rand2(_sc.xy);
    float s = sin(angle);
    float c = cos(angle);
    float vis = 0.f;
    for (int ind = 0; ind < 24; ind++) {
        vec2 offs = disk24[ind] * _radius;
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vis += texture(_tex, vec4(_sc.x+offs.x, _sc.y+offs.y, _layer, _sc.z - _bias));
    }
    return vis / 24.f;
}
