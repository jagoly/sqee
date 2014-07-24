#version 330

in vec3 e_pos, e_norm;
in vec2 texCoords;

uniform vec3 w_lightPos, lightSpec, lightDiff, lightAmbi;
uniform mat4 viewMatrix;
uniform sampler2D basicTex;

out vec4 fragColour;

void main() {
    vec4 texel = texture2D(basicTex, texCoords);

    vec3 reflSpec = vec3(1.f, 1.f, 1.f);
    vec3 reflDiff = vec3(1.f, 1.f, 1.f);
    vec3 reflAmbi = vec3(1.f, 1.f, 1.f);
    float specExp = 100.f;

    vec3 ambi = lightAmbi * reflAmbi;

    vec3 e_lightPos = vec3(viewMatrix * vec4(w_lightPos, 1.f));
    vec3 lightDistToEye = e_lightPos - e_pos;

    vec3 lightDirToEye = normalize(lightDistToEye);

    float dotProd = dot(lightDirToEye, e_norm);
    dotProd = max(dotProd, 0.f);

    vec3 diff = lightDiff * reflDiff * dotProd;

    vec3 reflEye = reflect(-lightDirToEye, e_norm);
    vec3 surfToViewerEye = normalize(-e_lightPos);
    float dotProdSpec = dot(reflEye, surfToViewerEye);
    dotProdSpec = max(dotProdSpec, 0.f);
    float specFactor = pow(dotProdSpec, specExp);

    vec3 spec = lightSpec * reflSpec * specFactor;

    fragColour = texel * vec4(spec+diff+ambi, 1.f);
}
