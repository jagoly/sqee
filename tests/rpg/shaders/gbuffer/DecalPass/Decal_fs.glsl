// GLSL Fragment Shader

//============================================================================//

#include headers/blocks/Camera

layout(std140, binding=0) uniform CAMERA { CameraBlock CB; };

//============================================================================//

in noperspective vec2 screenPos;
in vec3 viewNorm, viewTan;

layout(location=1) uniform mat4 u_InvModelView;
layout(location=2) uniform ivec3 u_MtrlMode;
layout(location=3) uniform float u_Alpha;

layout(binding=0) uniform sampler2D tex_Diffuse;
layout(binding=1) uniform sampler2D tex_Normal;
layout(binding=2) uniform sampler2D tex_Specular;

layout(binding=3) uniform sampler2D tex_GbufDiff;
layout(binding=4) uniform sampler2D tex_GbufSurf;
layout(binding=5) uniform sampler2D tex_GbufNorm;
layout(binding=6) uniform sampler2D tex_GbufSpec;
layout(binding=7) uniform sampler2D tex_Depth;

layout(location=0) out vec3 frag_Diffuse;
layout(location=1) out vec3 frag_Surface;
layout(location=2) out vec3 frag_Normal;
layout(location=3) out vec3 frag_Specular;

//============================================================================//

void main()
{
    float depth = texture(tex_Depth, screenPos).r * 2.f - 1.f;
    vec4 vPos = CB.invProjMat * vec4(screenPos * 2.f - 1.f, depth, 1.f);
    vec3 oPos = vec3(u_InvModelView * vec4(vPos.xyz / vPos.w, 1.f));

    if (abs(oPos.x) > 0.5f) discard;
    if (abs(oPos.y) > 0.5f) discard;
    if (abs(oPos.z) > 0.5f) discard;

    vec3 crntDiff = frag_Diffuse  = texture(tex_GbufDiff, screenPos).rgb;
    vec3 crntSurf = frag_Surface  = texture(tex_GbufSurf, screenPos).rgb;
    vec3 crntNorm = frag_Normal   = texture(tex_GbufNorm, screenPos).rgb;
    vec3 crntSpec = frag_Specular = texture(tex_GbufSpec, screenPos).rgb;

    vec3 surface = normalize(crntSurf * 2.f - 1.f);

    vec3 decalNorm = normalize(viewNorm);
    vec3 decalTan = normalize(viewTan);

    float difference = dot(decalNorm, surface);
    difference = min(difference, 0.25f) * 4.f;
    if (difference < -0.01f) discard;

    float factor = difference * u_Alpha;
    vec2 texcrd = oPos.xy + 0.5f;

    if (bool(u_MtrlMode.x) == true) {
        vec4 diff = texture(tex_Diffuse, texcrd);
        frag_Diffuse = mix(crntDiff, diff.rgb, diff.a * factor);
    }

    if (bool(u_MtrlMode.y) == true) {
        vec3 tangent = normalize(cross(surface, decalTan));
        vec3 binormal = normalize(cross(surface, tangent));
        vec3 T = tangent, B = binormal, N = surface;

        vec4 texel = texture(tex_Normal, texcrd);
        texel.rgb = normalize(texel.rgb);
        texel.rgb = T * texel.x + B * texel.y + N * texel.z;
        frag_Normal = mix(crntNorm, texel.rgb, texel.a * factor);
        frag_Normal = normalize(frag_Normal);
    }

    if (bool(u_MtrlMode.z) == true) {
        vec4 spec = texture(tex_Specular, texcrd);
        frag_Specular = mix(crntSpec, spec.rgb, spec.a * factor);
    }

//    fragDiff = vec3(0.f);
}
