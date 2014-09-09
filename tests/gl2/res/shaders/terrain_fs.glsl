#version 330

in vec3 v_pos;
in vec3 texcoord;
in vec2 mapPos;
in vec3 n, t, b;

uniform mat4 projMat, viewMat;
uniform vec3 camPos, skyLightDir;
uniform vec3 skyLightDiff, skyLightAmbi, skyLightSpec;
uniform sampler2DArray texNormArray;
uniform sampler2DArray texDiffArray;
uniform sampler2D texAmbi;
uniform sampler2DArray texSpecArray;

out vec4 fragColour;

void main() {
    vec3 t_norm = normalize(texture(texNormArray, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec3 reflDiff = texture(texDiffArray, texcoord).rgb;
    float reflAmbi = texture(texAmbi, mapPos).r;
    vec3 reflSpec = texture(texSpecArray, texcoord).rgb;

    // Diffuse
    vec3 dirToLight = vec4(viewMat * vec4(-skyLightDir, 0.f)).xyz;
    float dotProd = dot(dirToLight, v_norm);
    dotProd = max(dotProd, 0.f);
    vec3 diff = skyLightDiff * reflDiff * dotProd;


    // Ambient
    vec3 ambi = reflDiff * reflAmbi * skyLightAmbi;

    // Specular
    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1.f)).xyz - v_pos);

    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0.f)).xyz, v_norm);
    dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 50.f);
    vec3 spec = skyLightSpec * reflSpec * factor;

    fragColour = vec4(ambi + spec + diff, 1.f);
}
