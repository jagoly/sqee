#version 330

in vec2 texcoord;

uniform mat4 invProjMat;
uniform mat4 viewMat;
uniform vec3 camPos, skyLightDir;
uniform vec3 skyLightDiff, skyLightAmbi, skyLightSpec;
uniform uvec2 viewSize;
uniform sampler2D texNorm;
uniform sampler2D texDiff;
uniform sampler2D texAmbi;
uniform sampler2D texSpec;
uniform sampler2D texDepth;

out vec4 fragColour;

void main() {
    vec3 normal = normalize(texture(texNorm, texcoord).xyz * 2.f - 1.f);
    vec4 clipPos = vec4(texcoord * 2.f - 1.f, texture(texDepth, texcoord).r * 2.f - 1.f, 1.f);
    vec4 homogPos = vec4(invProjMat * clipPos);
    vec3 pos = homogPos.xyz / homogPos.w;

    vec3 reflDiff = texture(texDiff, texcoord).rgb;
    float reflAmbi = texture(texAmbi, texcoord).r;
    vec3 reflSpec = texture(texSpec, texcoord).rgb;


    // Diffuse
    vec3 dirToLight = vec4(viewMat * vec4(-skyLightDir, 0.f)).xyz;
    float dotProd = dot(dirToLight, normal);
    dotProd = max(dotProd, 0.f);
    vec3 diff = skyLightDiff * reflDiff * dotProd;


    // Ambient
    vec3 ambi = reflDiff * reflAmbi * skyLightAmbi;


    // Specular
    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1.f)).xyz - pos);

    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0.f)).xyz, normal);
    dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 50.f);
    vec3 spec = skyLightSpec * reflSpec * factor;

    fragColour = vec4(ambi + spec + diff, 1.f);
    fragColour.a = 0.299f * fragColour.r + 0.587f * fragColour.g + 0.114f * fragColour.b;
    //fragColour = vec4(reflSpec, 1.f);
    //fragColour = vec4(skyLightDir / 2.f + 0.5f, 1.f);
    //fragColour = vec4(normal / 2.f + 0.5f, 1.f);
    //fragColour = vec4(reflAmbi, 1.f);
    //fragColour = vec4(1.f, 1.f, 0.f, 1.f);
}
