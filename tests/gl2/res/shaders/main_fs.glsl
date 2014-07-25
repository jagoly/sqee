#version 330

in vec3 w_norm;
in vec3 e_pos, e_norm;
in vec2 texCoords;

uniform vec3 w_lightPos, lightDiff, lightAmbi;
uniform mat4 viewMatrix;
uniform sampler2D basicTex;

out vec4 fragColour;

void main() {
    vec4 texel = texture2D(basicTex, texCoords);

    vec3 reflDiff = vec3(1.f, 1.f, 1.f);
    vec3 reflAmbi = vec3(1.f, 1.f, 1.f);

    vec3 ambi = lightAmbi * reflAmbi;

    vec3 e_lightPos = vec3(viewMatrix * vec4(w_lightPos, 1.f));
    vec3 lightDistToEye = e_lightPos - e_pos;

    vec3 lightDirToEye = normalize(lightDistToEye);

    float dotProd = dot(lightDirToEye, e_norm);
    dotProd = max(dotProd, 0.f);

    vec3 diff = lightDiff * reflDiff * dotProd;

    //fragColour = vec4(diff+ambi, 1.f);
    fragColour = texel * vec4(diff+ambi, 1.f);
    //fragColour = vec4(1.f, 1.f, 1.f, 1.f);
    //fragColour = vec4(w_norm, 1.f);
}
