#version 330

in vec2 texCoord;

uniform mat4 invProjViewMat;
uniform vec3 camPos, skyLightDir;
uniform vec3 skyLightDiff, skyLightAmbi, skyLightSpec;
uniform sampler2D texNorm;
uniform sampler2D texDiff;
uniform sampler2D texAmbi;
uniform sampler2D texSpec;
uniform sampler2D texDepth;

out vec4 fragColour;

void main() {
    vec3 normal = texture(texNorm, texCoord).xyz * 2.f - 1.f;

    vec4 clipPos = vec4(texCoord * 2.f - 1.f, texture(texDepth, texCoord).r * 2.f - 1.f, 1.f);
    vec4 homogPos = vec4(invProjViewMat * clipPos);
    vec3 pos = homogPos.xyz / homogPos.w;

    vec3 reflDiff = texture(texDiff, texCoord).rgb;
    vec3 dirToLight = -skyLightDir;
    float dotProd = dot(dirToLight, normal);
    dotProd = max(dotProd, 0.f);
    vec3 diff = skyLightDiff * reflDiff * dotProd;

    vec3 reflAmbi = texture(texAmbi, texCoord).rgb;
    vec3 ambi = reflAmbi * skyLightAmbi;

    vec3 reflSpec = texture(texSpec, texCoord).rgb;
    vec3 dirFromCam = normalize(camPos - pos);


    vec3 reflection = reflect(skyLightDir, normal);
    dotProd = dot(reflection, dirFromCam);
    dotProd = max(dotProd, 0.f);
    float factor = pow(dotProd, 25.f);
    vec3 spec = skyLightSpec * reflSpec * factor;

    fragColour = vec4(ambi + diff + spec, 1.f);
    //fragColour = vec4(normal / 2.f + 0.5f, 1.f);
}
