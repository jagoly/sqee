#version 330

in vec3 texCoord, t_viewDir, t_lightDir;

uniform vec3 lightDiff, lightAmbi;
uniform sampler2DArray texArray;
uniform sampler2DArray nMapArray;


layout(location = 0) out vec4 fragColour;

void main() {
    vec3 lightSpec = vec3(1.f, 1.f, 1.f);

    vec4 texel = texture(texArray, texCoord);

    vec3 t_norm = texture(nMapArray, texCoord).rgb;
    t_norm = normalize(t_norm * 2.f - 1.f);

    // ambient
    vec3 reflAmbi = vec3(1.f, 1.f, 1.f);
    vec3 ambi = lightAmbi * reflAmbi;

    // diffuse
    vec3 reflDiff = vec3(1.f, 1.f, 1.f);
    vec3 t_dirToLight = normalize(-t_lightDir);
    float dotProd = dot(t_dirToLight, t_norm);
    dotProd = max(dotProd, 0.f);
    vec3 diff = lightDiff * reflDiff * dotProd;

    // specular
    //vec3 reflSpec = vec3(1.f, 1.f, 1.f);
    //vec3 t_reflection = reflect(normalize(t_lightDir), t_norm);
    //dotProd = dot(t_reflection, normalize(t_viewDir));
    //dotProd = max(dotProd, 0.f);
    //float factor = pow(dotProd, 300.f);
    //vec3 spec = lightSpec * reflSpec * factor;

    // set
    fragColour = texel * vec4(ambi + diff, 1.f);
    //fragColour = vec4(t_norm, 1.f);
}
