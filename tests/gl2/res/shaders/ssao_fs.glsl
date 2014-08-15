#version 330
in vec2 texcoord;

uniform sampler2D texNorm;
uniform sampler2D texDepth;

uniform mat4 invProjMat;

layout(location = 0) out vec4 fragColour;

vec3 get_v_pos(in vec2 coord) {
    float bufZ = texture(texDepth, coord).r * 2.f - 1.f;
    vec4 projPos = vec4(coord.xy * 2.f - 1.f, bufZ, 1.f);
    vec4 viewPos = invProjMat * projPos;
    return viewPos.xyz / viewPos.w;
}

const float distanceThreshold = 0.6f;
const vec2 filterRadius = vec2(64.f / 1280.f, 64.f / 720.f);

const int sampleCount = 16;
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
                                vec2( -0.094184101, -0.92938870 ),
                                vec2(  0.34495938,   0.29387760 ),
                                vec2( -0.91588581,   0.45771432 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                               );

void main() {
    vec3 pos = get_v_pos(texcoord);

    vec3 normal = normalize(texture(texNorm, texcoord).rgb * 2.f - 1.f);

    float ambientOcclusion = 0;
    // perform AO
    for (int i = 0; i < sampleCount; i++) {
        // sample at an offset specified by the current Poisson-Disk sample and scale it by a radius (has to be in Texture-Space)
        vec2 sampleTexcoord = texcoord + (poisson16[i] * (filterRadius / pos.z));
        vec3 samplePos = get_v_pos(sampleTexcoord);
        vec3 sampleDir = normalize(samplePos - pos);

        // angle between SURFACE-NORMAL and SAMPLE-DIRECTION (vector from SURFACE-POSITION to SAMPLE-POSITION)
        float NdotS = max(dot(normal, sampleDir), 0.f);
        // distance between SURFACE-POSITION and SAMPLE-POSITION
        float VPdistSP = distance(pos, samplePos);

        // a = distance function
        float a = 1.f - smoothstep(distanceThreshold, distanceThreshold * 2, VPdistSP);
        // b = dot-Product
        float b = NdotS;

        ambientOcclusion += a * b;
    }

    float ao = 1.f - ambientOcclusion / sampleCount;
    fragColour = vec4(ao, ao, ao, 1.f);
    //fragColour = vec4(pos / 2.f + 0.5f, 1.f);
}
