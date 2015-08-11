// GLSL Fragment Shader

// define HIGH
// define PIXSIZE vec2

in vec2 texcrd;

#include builtin/blocks/camera

layout(std140, binding=0) uniform CAMERABLOCK { CameraBlock CB; };

layout(binding=0) uniform sampler2D texDepHalf;
layout(binding=4) uniform sampler2D defrSurf;

out float fragColour;


#ifdef HIGH
const float distThres = 1.5f;
const float filterSize = 300.f;
const int diskSize = 36;
const vec2 disk[36] = {
vec2(0.000000, 0.027778),
vec2(-0.054712, -0.009647),
vec2(0.028502, -0.078308),
vec2(0.096225, 0.055556),
vec2(-0.089276, 0.106395),
vec2(-0.107131, -0.127674),
vec2(0.168394, -0.097222),
vec2(0.076004, 0.208821),
vec2(-0.246202, 0.043412),
vec2(-0.000000, -0.277778),
vec2(0.300913, 0.053059),
vec2(-0.114007, 0.313231),
vec2(-0.312731, -0.180556),
vec2(0.249973, -0.297906),
vec2(0.267828, 0.319185),
vec2(-0.384900, 0.222222),
vec2(-0.161510, -0.443744),
vec2(0.492404, -0.086824),
vec2(0.000000, 0.527778),
vec2(-0.547115, -0.096471),
vec2(0.199512, -0.548154),
vec2(0.529238, 0.305556),
vec2(-0.410670, 0.489417),
vec2(-0.428525, -0.510696),
vec2(0.601407, -0.347222),
vec2(0.247015, 0.678667),
vec2(-0.738606, 0.130236),
vec2(-0.000000, -0.777778),
vec2(0.793317, 0.139883),
vec2(-0.285017, 0.783077),
vec2(-0.745744, -0.430556),
vec2(0.571367, -0.680928),
vec2(0.589222, 0.702207),
vec2(-0.817913, 0.472222),
vec2(-0.332520, -0.913590),
vec2(0.984808, -0.173648),
};
#else
const float distThres = 0.5f;
const float filterSize = 100.f;
const int diskSize = 18;
const vec2 disk[18] = {
vec2(0.000000, 0.055556),
vec2(-0.071421, -0.085116),
vec2(0.164135, 0.028941),
vec2(-0.192450, 0.111111),
vec2(0.095006, -0.261026),
vec2(0.114007, 0.313231),
vec2(-0.336788, -0.194444),
vec2(0.437692, -0.077177),
vec2(-0.321394, 0.383022),
vec2(-0.000000, -0.555556),
vec2(0.392815, 0.468138),
vec2(-0.656539, -0.115765),
vec2(0.625463, -0.361111),
vec2(-0.266016, 0.730872),
vec2(-0.285017, -0.783077),
vec2(0.769800, 0.444444),
vec2(-0.930096, 0.164001),
vec2(0.642788, -0.766044),
};
#endif

vec3 get_view_pos(in vec2 _tc) {
    float dep = texture(texDepHalf, _tc).r * 2.f - 1.f;
    vec4 p_pos = vec4(_tc * 2.f - 1.f, dep, 1.f);
    vec4 v_pos = CB.invProj * p_pos;
    return v_pos.xyz / v_pos.w;
}

float rand(vec2 _crd){
    return fract(sin(dot(_crd, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

void main() {
    const float scale = 16.f / 9.f / (PIXSIZE.y / PIXSIZE.x) / PIXSIZE.y / 720.f;

    vec3 pos = get_view_pos(texcrd);
    vec2 fsvspz = scale * filterSize * PIXSIZE / pos.z;
    vec3 v_surf = normalize(texture(defrSurf, texcrd).rgb * 2.f - 1.f);

    float angle = rand(texcrd - CB.dir.xy);
    float s = sin(angle), c = cos(angle);

    float ambiOcc = 0.f;
    for (int i = 0; i < diskSize; i++) {
        vec2 offs = disk[i];
        offs = vec2(c * offs.x - s * offs.y, c * offs.y + s * offs.x);
        vec3 sampPos = get_view_pos(texcrd + offs * fsvspz);
        vec3 sampDir = normalize(sampPos - pos);
        float dist = distance(pos, sampPos);

        float a = max(dot(v_surf, sampDir), 0.f);
        float b = 1.f - smoothstep(distThres, distThres * 2.f, dist);
        ambiOcc += a * b;
    }

    fragColour = 1.f - ambiOcc / float(diskSize) * 0.8f;
}
