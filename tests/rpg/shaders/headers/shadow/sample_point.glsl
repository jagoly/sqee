// GLSL Header File

#include builtin/funcs/random
#include builtin/misc/disks

//============================================================================//

float get_bias(vec3 normal, vec3 dirToLight)
{
    float lightDot = dot(normal, dirToLight);
    float magicTan = sqrt(1.f - lightDot * lightDot) / lightDot;

    return clamp(0.0002f * magicTan, 0.f, 0.004f);
}

float sample_shadow(samplerCubeShadow tex, vec4 sc, float bias)
{
    return texture(tex, vec4(sc.xyz, sc.w - bias));
}

//============================================================================//

float sample_shadow_x4(samplerCubeShadow tex, vec4 sc, float bias, float radius)
{
    const float rotation = rand3(sc.xyz);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    const vec3 absVec = abs(sc.xyz);
    
    int axis = 0;

    if (absVec.x > absVec.y && absVec.x > absVec.z) axis = 0;
    if (absVec.y > absVec.z && absVec.y > absVec.x) axis = 1;
    if (absVec.z > absVec.x && absVec.z > absVec.y) axis = 2;

    float result = 0.f;

    for (int i = 0; i < 4; ++i)
    {
        vec2 os = rrs * c_Disk4[i], oc = rrc * c_Disk4[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);
        
        vec3 sampleDir = sc.xyz;

        if (axis == 0) sampleDir.yz += offset;
        if (axis == 1) sampleDir.zx += offset;
        if (axis == 2) sampleDir.xy += offset;

        result += texture(tex, vec4(normalize(sampleDir), sc.w - bias));
    }

    return result / 4.f;
}

//============================================================================//


float sample_shadow_x8(samplerCubeShadow tex, vec4 sc, float bias, float radius)
{
    const float rotation = rand3(sc.xyz);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    const vec3 absVec = abs(sc.xyz);
    
    int axis = 0;

    if (absVec.x > absVec.y && absVec.x > absVec.z) axis = 0;
    if (absVec.y > absVec.z && absVec.y > absVec.x) axis = 1;
    if (absVec.z > absVec.x && absVec.z > absVec.y) axis = 2;

    float result = 0.f;

    for (int i = 0; i < 8; ++i)
    {
        vec2 os = rrs * c_Disk8[i], oc = rrc * c_Disk8[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);
        
        vec3 sampleDir = sc.xyz;

        if (axis == 0) sampleDir.yz += offset;
        if (axis == 1) sampleDir.zx += offset;
        if (axis == 2) sampleDir.xy += offset;

        result += texture(tex, vec4(normalize(sampleDir), sc.w - bias));
    }

    return result / 8.f;
}

//============================================================================//

float sample_shadow_x16(samplerCubeShadow tex, vec4 sc, float bias, float radius)
{
    const float rotation = rand3(sc.xyz);
    const float rrs = radius * sin(rotation);
    const float rrc = radius * cos(rotation);

    const vec3 absVec = abs(sc.xyz);
    
    int axis = 0;

    if (absVec.x > absVec.y && absVec.x > absVec.z) axis = 0;
    if (absVec.y > absVec.z && absVec.y > absVec.x) axis = 1;
    if (absVec.z > absVec.x && absVec.z > absVec.y) axis = 2;

    float result = 0.f;

    for (int i = 0; i < 16; ++i)
    {
        vec2 os = rrs * c_Disk16[i], oc = rrc * c_Disk16[i];
        vec2 offset = vec2(oc.x - os.y, oc.y + os.x);
        
        vec3 sampleDir = sc.xyz;

        if (axis == 0) sampleDir.yz += offset;
        if (axis == 1) sampleDir.zx += offset;
        if (axis == 2) sampleDir.xy += offset;

        result += texture(tex, vec4(normalize(sampleDir), sc.w - bias));
    }

    return result / 16.f;
}

