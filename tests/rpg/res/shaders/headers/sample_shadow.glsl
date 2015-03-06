// GLSL Header File

float get_bias(vec3 _normal, vec3 _lightDir, float _modifier) {
    float lightDot = dot(_normal, -_lightDir);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;
    return clamp(0.0025f * _modifier * magicTan, 0.0f, 0.01f);
}

float sample_shadow_x1(vec3 _sc, int _layer, float _bias, sampler2DArrayShadow _tex) {
    return texture(_tex, vec4(_sc.xy, _layer, _sc.z-_bias));
}

float sample_shadow_x2(vec3 _sc, int _layer, float _bias, sampler2DArrayShadow _tex, float _texSize) {
    float px = 1.f / _texSize; float vis = 0.f;
    for (float x = -0.5f; x < 0.51f; x+=1.f)
        for (float y = -0.5f; y < 0.51f; y+=1.f)
            vis += texture(_tex, vec4(_sc.x+x*px, _sc.y+y*px, _layer, _sc.z-_bias));
    return vis / 4.f;
}

float sample_shadow_x4(vec3 _sc, int _layer, float _bias, sampler2DArrayShadow _tex, float _texSize) {
    float px = 1.f / _texSize; float vis = 0.f;
    for (float x = -1.5f; x < 1.51f; x+=1.f)
        for (float y = -1.5f; y < 1.51f; y+=1.f)
            vis += texture(_tex, vec4(_sc.x+x*px, _sc.y+y*px, _layer, _sc.z - _bias));
    return vis / 16.f;
}
