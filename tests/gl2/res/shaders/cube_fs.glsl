#version 330

in vec3 normal;
in vec3 posEye, normalEye;

uniform vec3 lightWorldPos, lightSpec, lightDiff, lightAmbi;
uniform mat4 viewMatrix;

out vec4 fragColour;

void main() {
    vec3 reflSpec = vec3(1.f, 1.f, 1.f);
    vec3 reflDiff = vec3(1.f, 1.f, 1.f);
    vec3 reflAmbi = vec3(1.f, 1.f, 1.f);
    float specExp = 100.f;

    vec3 ambi = lightAmbi * reflAmbi;

    vec3 lightEyePos = vec3(viewMatrix * vec4(lightWorldPos, 1.f));
    vec3 lightDistToEye = lightEyePos - posEye;

    vec3 lightDirToEye = normalize(lightDistToEye);

    float dotProd = dot(lightDirToEye, normalEye);
    dotProd = max(dotProd, 0.f);

    vec3 diff = lightDiff * reflDiff * dotProd;

    vec3 reflEye = reflect(-lightDirToEye, normalEye);
    vec3 surfToViewerEye = normalize(-lightEyePos);
    float dotProdSpec = dot(reflEye, surfToViewerEye);
    dotProdSpec = max(dotProdSpec, 0.f);
    float specFactor = pow(dotProdSpec, specExp);

    vec3 spec = lightSpec * reflSpec * specFactor;

    fragColour = vec4(spec+diff+ambi, 1.f);

    //fragColour = vec4(normal, 1.f);
}
