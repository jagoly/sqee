// GLSL Header File

#include builtin/funcs/random
#include builtin/misc/disks

//============================================================================//

float get_bias(vec3 normal, vec3 dirToLight)
{
    float lightDot = dot(normal, dirToLight);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;

    return clamp(0.001f * magicTan, 0.f, 0.01f);
}

float sample_shadow(sampler2DArrayShadow tex, vec3 sc, int layer, float bias)
{
    return texture(tex, vec4(sc.xy, layer, sc.z - bias));
}

//============================================================================//

float sample_shadow_x4(sampler2DArrayShadow tex, vec3 sc, int layer, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 4; ++i)
    {
        vec2 os = rrs * c_Disk4[i], oc = rrc * c_Disk4[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec4(sc.xy + offset, layer, sc.z - bias));
    }

    return result / 4.f;
}

//============================================================================//

float sample_shadow_x6(sampler2DArrayShadow tex, vec3 sc, int layer, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 6; ++i)
    {
        vec2 os = rrs * c_Disk6[i], oc = rrc * c_Disk6[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec4(sc.xy + offset, layer, sc.z - bias));
    }

    return result / 6.f;
}

//============================================================================//

float sample_shadow_x8(sampler2DArrayShadow tex, vec3 sc, int layer, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 8; ++i)
    {
        vec2 os = rrs * c_Disk8[i], oc = rrc * c_Disk8[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec4(sc.xy + offset, layer, sc.z - bias));
    }

    return result / 8.f;
}

//============================================================================//

float sample_shadow_x12(sampler2DArrayShadow tex, vec3 sc, int layer, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 12; ++i)
    {
        vec2 os = rrs * c_Disk12[i], oc = rrc * c_Disk12[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec4(sc.xy + offset, layer, sc.z - bias));
    }

    return result / 12.f;
}

//============================================================================//

float sample_shadow_x16(sampler2DArrayShadow tex, vec3 sc, int layer, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 16; ++i)
    {
        vec2 os = rrs * c_Disk16[i], oc = rrc * c_Disk16[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec4(sc.xy + offset, layer, sc.z - bias));
    }

    return result / 16.f;
}

//============================================================================//

float sample_shadow_x24(sampler2DArrayShadow tex, vec3 sc, int layer, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 24; ++i)
    {
        vec2 os = rrs * c_Disk24[i], oc = rrc * c_Disk24[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec4(sc.xy + offset, layer, sc.z - bias));
    }

    return result / 24.f;
}

