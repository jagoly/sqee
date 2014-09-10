#version 330

in vec3 v_pos;
in vec3 shadcoord;
in vec2 texcoord;
in vec3 n, t, b;

uniform mat4 projMat, viewMat;
uniform vec3 camPos, skyLightDir;
uniform vec3 skyLightDiff, skyLightAmbi, skyLightSpec;
uniform sampler2D texNorm;
uniform sampler2D texDiff;
uniform sampler2D texAmbi;
uniform sampler2D texSpec;
uniform sampler2DShadow texShad;

const float pix = 1.f / 4096f;

out vec4 fragColour;

void main() {
    vec3 t_norm = normalize(texture(texNorm, texcoord).rgb * 2.f - 1.f);
    vec3 v_norm = t * t_norm.x + b * t_norm.y + n * t_norm.z;

    vec4 reflDiff = texture(texDiff, texcoord);
    float reflAmbi = texture(texAmbi, texcoord).r;
    vec3 reflSpec = texture(texSpec, texcoord).rgb;

    vec3 sc = vec3(shadcoord.xy / 2.f + 0.5f, shadcoord.z / 2.f + 0.5f - 0.0005f);
    float visibility = texture(texShad, sc);

    // Diffuse
    vec3 dirToLight = vec4(viewMat * vec4(-skyLightDir, 0.f)).xyz;
    float dotProd = dot(dirToLight, v_norm);
    dotProd = max(dotProd, 0.f);
    vec3 diff = skyLightDiff * reflDiff.rgb * dotProd * visibility;

    // Ambient
    vec3 ambi = reflDiff.rgb * reflAmbi * skyLightAmbi;

    // Specular
    vec3 dirFromCam = normalize(vec4(viewMat * vec4(camPos, 1.f)).xyz - v_pos);

    vec3 reflection = reflect(vec4(viewMat * vec4(skyLightDir, 0.f)).xyz, v_norm);
    dotProd = max(dot(reflection, dirFromCam), 0.f);
    float factor = pow(dotProd, 50.f);
    vec3 spec = skyLightSpec * reflSpec * factor * visibility;

    fragColour = vec4(ambi + spec + diff, reflDiff.a);
    //fragColour = vec4(v_norm / 2.f + 0.5f, 1.f);
}
