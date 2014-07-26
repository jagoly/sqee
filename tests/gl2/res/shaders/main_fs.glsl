#version 330

in vec3 texCoords, w_norm, e_pos;

uniform vec3 w_lightPos, lightDiff, lightAmbi;
uniform mat4 viewMatrix;
uniform sampler2DArray texArray;
uniform sampler2DArray nMapArray;

out vec4 fragColour;

void main() {
    vec4 texel = texture(texArray, texCoords);
    vec3 nMNorm = normalize(texture(nMapArray, texCoords).rgb*2.f - 1.f);
    vec3 adj_w_norm = vec3(w_norm.x + nMNorm.x, w_norm.y + nMNorm.y, w_norm.z);
    vec3 e_norm = vec3(viewMatrix * vec4(adj_w_norm, 0.f));

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
