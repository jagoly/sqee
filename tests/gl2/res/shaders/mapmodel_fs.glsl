#version 330

in vec2 texCoord;
in vec3 t_viewDir, t_lightDir;

uniform vec3 lightDiff, lightAmbi;
uniform sampler2D texDiff;

layout(location = 0) out vec4 fragColour;

void main() {
    vec3 lightSpec = vec3(1.f, 1.f, 1.f);
    vec3 ambiTexel = texture(texArray, vec3(texCoord, 0)).rgb;
    vec3 diffTexel = texture(texArray, vec3(texCoord, 1)).rgb;
    vec3 specTexel = texture(texArray, vec3(texCoord, 2)).rgb;
    float alpha = texture(texArray, vec3(texCoord, 1)).a;

    vec3 t_norm = texture(texArray, vec3(texCoord, 3)).rgb;
    t_norm = normalize(t_norm * 2.f - 1.f);

    // ambient
    vec3 ambi = lightAmbi * ambiTexel;

    // diffuse
    vec3 t_dirToLight = normalize(-t_lightDir);
    float dotProd = dot(t_dirToLight, t_norm);
    dotProd = max(dotProd, 0.f);
    vec3 diff = lightDiff * dotProd;

    // specular
    vec3 t_reflection = reflect(normalize(t_lightDir), t_norm);
    dotProd = dot(t_reflection, normalize(t_viewDir));
    dotProd = max(dotProd, 0.f);
    float factor = pow(dotProd, 100.f);
    vec3 spec = lightSpec * specTexel * factor;

    // set
    fragColour = vec4(diffTexel * (diff + ambi) + spec, alpha);
    //fragColour = vec4(t_norm, 1.f);
    //fragColour = vec4(specTexel, 1.f);
    //fragColour = vec4(1.f, 1.f, 1.f, 1.f);
}
