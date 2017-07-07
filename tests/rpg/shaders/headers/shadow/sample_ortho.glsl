// GLSL Header File

#include builtin/funcs/random
#include builtin/misc/disks

//============================================================================//

float get_bias(vec3 normal, vec3 dirToLight)
{
    float lightDot = dot(normal, -dirToLight);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;

    return clamp(0.001f * magicTan, 0.f, 0.01f);
}

float sample_shadow(sampler2DShadow tex, vec3 sc, float bias)
{
    return texture(tex, vec3(sc.xy, sc.z - bias));
}

//============================================================================//

float sample_shadow_x4(sampler2DShadow tex, vec3 sc, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 4; ++i)
    {
        vec2 os = rrs * c_Disk4[i], oc = rrc * c_Disk4[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec3(sc.xy + offset, sc.z - bias));
    }

    return result / 4.f;
}

//============================================================================//

float sample_shadow_x8(sampler2DShadow tex, vec3 sc, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 8; ++i)
    {
        vec2 os = rrs * c_Disk8[i], oc = rrc * c_Disk8[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec3(sc.xy + offset, sc.z - bias));
    }

    return result / 8.f;
}

//============================================================================//

float sample_shadow_x16(sampler2DShadow tex, vec3 sc, float bias, float radius)
{
    const float rotation = rand2(sc.xy);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    float result = 0.f;

    for (int i = 0; i < 16; ++i)
    {
        vec2 os = rrs * c_Disk16[i], oc = rrc * c_Disk16[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);

        result += texture(tex, vec3(sc.xy + offset, sc.z - bias));
    }

    return result / 16.f;
}

