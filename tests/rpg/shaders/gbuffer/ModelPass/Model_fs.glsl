// GLSL Fragment Shader

in vec2 texcrd;
in vec3 N, T, B;

uniform ivec3 mtrlMode;
uniform vec3 colourDiff;
uniform vec3 colourSpec;

layout(binding=0) uniform sampler2D texDiff;
layout(binding=1) uniform sampler2D texNorm;
layout(binding=2) uniform sampler2D texSpec;

layout(location=0) out vec3 fragDiff;
layout(location=1) out vec3 fragSurf;
layout(location=2) out vec3 fragNorm;
layout(location=3) out vec3 fragSpec;


void main() 
{
    // use constant colours by default
    fragDiff = colourDiff; fragSpec = colourSpec;

    // if enabled, use diffuse and/or specular textures
    if (bool(mtrlMode.x)) fragDiff = texture(texDiff, texcrd).rgb;
    if (bool(mtrlMode.z)) fragSpec = texture(texSpec, texcrd).rgb;

    // write the fragment surface normal
    fragSurf = fragNorm = normalize(N);

    if (bool(mtrlMode.y)) // if enabled, perform normal mapping
    {
        vec3 t_norm = normalize(texture(texNorm, texcrd).rgb);
        fragNorm = T * t_norm.x + B * t_norm.y + N * t_norm.z;
        fragNorm = normalize(fragNorm);
    }

}
