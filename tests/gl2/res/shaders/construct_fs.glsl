#version 330

in vec2 texcoord;

uniform mat4 invProjMat;
uniform mat4 viewMat;
uniform vec3 camPos, skyLightDir;
uniform vec3 skyLightDiff, skyLightAmbi, skyLightSpec;
uniform sampler2D texNorm;
uniform sampler2D texDiff;
uniform sampler2D texAmbi;
uniform sampler2D texSpec;
uniform sampler2D texDepth;

out vec4 fragColour;

const float xPix = 2.f / 1280.f;
const float yPix = 2.f / 720.f;

const float influArray[9] = float[9](1.f, 2.f, 1.f,
                                     2.f, 3.f, 2.f,
                                     1.f, 2.f, 1.f);

vec3 sampleBox(in vec2 coord) {
    vec3 texel = vec3(0.f, 0.f, 0.f);
    int p = 0;
    vec2 iCoord;
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            iCoord = vec2(coord.x + (xPix * x), coord.y + (yPix * y));
            texel += texture(texAmbi, iCoord).rgb * influArray[p];
            p++;
        }
    }
    return texel / 15.f;
}

void main() {
    vec3 normal = normalize(texture(texNorm, texcoord).xyz * 2.f - 1.f);
    vec4 clipPos = vec4(texcoord * 2.f - 1.f, texture(texDepth, texcoord).r * 2.f - 1.f, 1.f);
    vec4 homogPos = vec4(invProjMat * clipPos);
    vec3 pos = homogPos.xyz / homogPos.w;

    vec3 reflDiff = texture(texDiff, texcoord).rgb;
    vec3 reflAmbi = texture(texAmbi, texcoord).rgb;
    vec3 reflSpec = texture(texSpec, texcoord).rgb;


    // Diffuse
    vec3 dirToLight = vec4(viewMat * vec4(-skyLightDir, 0.f)).xyz;
    float dotProd = dot(dirToLight, normal);
    dotProd = max(dotProd, 0.f);
    vec3 diff = skyLightDiff * reflDiff * dotProd;


    // Ambient
    vec3 ambi = reflDiff * skyLightAmbi;


    // Specular
    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1.f)).xyz - pos);

    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0.f)).xyz, normal);
    dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 50.f);
    vec3 spec = skyLightSpec * reflSpec * factor;

    fragColour = vec4(ambi + diff + spec, 1.f);
    //fragColour = vec4(skyLightDir / 2.f + 0.5f, 1.f);
    //fragColour = vec4(normal / 2.f + 0.5f, 1.f);
    //fragColour = vec4(reflAmbi, 1.f);
    //fragColour = vec4(normal / 2.f + 0.5f, 1.f);
    //fragColour = vec4(1.f, 1.f, 0.f, 1.f);
}
