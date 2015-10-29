/// Input data for full screen with TRIANGLE_STRIP
extern const char data_data_screen[] = R"glsl(

const vec2 V_pos_data[4] = {
    vec2(-1.f, -1.f), vec2(-1.f, +1.f),
    vec2(+1.f, -1.f), vec2(+1.f, +1.f)
};

vec2 V_pos = V_pos_data[gl_VertexID];

)glsl";



/// Functions for sampling and using depth
extern const char data_funcs_depth[] = R"glsl(

float linearise(float _depth, float _n, float _f) {
    return (2.f * _n) / (_f + _n - _depth * (_f - _n)) * (_f - _n);
}

vec3 get_view_pos(vec2 _texcrd, mat4 _invProj, sampler2D _depthTex) {
    float depth = texture(_depthTex, _texcrd).r * 2.f - 1.f;
    vec4 p_pos = vec4(_texcrd * 2.f - 1.f, depth, 1.f);
    vec4 v_pos = _invProj * p_pos;
    return v_pos.xyz / v_pos.w;
}

float nearest_depth_sca(vec2 _texcrd, sampler2D _sampler,
                        sampler2D _depTexFull, sampler2D _depTexHalf,
                        float _thres, float _rmin, float _rmax) {
    vec4 txHalf = textureGather(_depTexHalf, _texcrd);
    float txFull = texture(_depTexFull, _texcrd).r;
    txFull = linearise(txFull, _rmin, _rmax);

    float distA = distance(txFull, linearise(txHalf.r, _rmin, _rmax));
    float distB = distance(txFull, linearise(txHalf.g, _rmin, _rmax));
    float distC = distance(txFull, linearise(txHalf.b, _rmin, _rmax));
    float distD = distance(txFull, linearise(txHalf.a, _rmin, _rmax));

    if (max(max(distA, distB), max(distC, distD)) > _thres) {
        vec4 texel = textureGather(_sampler, _texcrd);
        float minDiff = distA, value = texel.r;
        if (distB < minDiff) { minDiff = distB; value = texel.g; }
        if (distC < minDiff) { minDiff = distC; value = texel.b; }
        if (distD < minDiff) { minDiff = distD; value = texel.a; }
        return value;
    } else return texture(_sampler, _texcrd).r;
}

)glsl";



/// Functions for generating random values from a seed
extern const char data_funcs_random[] = R"glsl(

float rand1(float _seed) {
    return fract(sin(_seed * 12.9898f) * 43758.5453f);
}

float rand2(vec2 _seed) {
    return fract(sin(dot(_seed, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

float rand3(vec3 _seed) {
    return fract(sin(dot(_seed, vec3(12.9898f, 78.233f, 151.7182f))) * 43758.5453f);
}

)glsl";



/// Functions for colour-space manipulation
extern const char data_funcs_colours[] = R"glsl(

vec3 rgb_to_hsv(vec3 _c) {
    vec3 c = clamp(_c, 0.f, 1.f);
    vec4 K = vec4(0.f, -1.f / 3.f, 2.f / 3.f, -1.f);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y); float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.f * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 _c) {
    vec3 c = vec3(_c.x, clamp(_c.y, 0.f, 1.f), _c.z);
    vec4 K = vec4(1.f, 2.f / 3.f, 1.f / 3.f, 3.f);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.f - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.f, 1.f), c.y);
}

vec3 rgb_to_hsl(vec3 _c) {
    float minc = min(min(_c.r, _c.g), _c.b);
    float maxc = max(max(_c.r, _c.g), _c.b);
    float delta = maxc - minc;

    float hue = 0.f, sat = 0.f;
    float lum = (minc + maxc) * 0.5f;

    if (lum > 0.f && lum < 1.f) {                      // check me
        float mult = lum < 0.5f ? lum : 1.f - lum;
        sat = delta / (mult * 2.f);
    }

    vec3 masks = vec3(
        float(maxc == _c.r && maxc != _c.g),
        float(maxc == _c.g && maxc != _c.b),
        float(maxc == _c.b && maxc != _c.r)
    );
    vec3 adds = vec3(
        0.f + (_c.g - _c.b) / delta,
        2.f + (_c.b - _c.r) / delta,
        4.f + (_c.r - _c.g) / delta
    );

    hue += dot(adds, masks);
    hue *= (delta > 0.f ? 1.f : 0.f) / 6.f;
    if (hue < 0.f) hue += 1.f;

    return vec3(hue, sat, lum);
}

vec3 hsl_to_rgb(vec3 _c) {
    vec3 xt = vec3(
        6.f * (_c.x - 2.f / 3.f),
        0.f, 6.f * (1.f - _c.x)
    );

    if (_c.x < 2.f / 3.f) { xt.r = 0.f;
        xt.g = 6.f * (2.f / 3.f - _c.x);
        xt.b = 6.f * (_c.x - 1.f / 3.f);
    }

    if (_c.x < 1.f / 3.f) { xt.b = 0.0;
        xt.r = 6.f * (1.f / 3.f - _c.x);
        xt.g = 6.f * _c.x;
    }

    xt = min(xt, 1.f);
    vec3 ct = 2.f * _c.y * xt - _c.y + 1.f;

    if (_c.z < 0.5f) return ct * _c.z;
    else return ct * (1.f - _c.z) + _c.z * 2.f - 1.f;
}

float rgb_to_luma(vec3 _c) {
    return dot(vec3(0.22f, 0.69f, 0.09f), _c);
}

)glsl";



/// Standard SQEE Camera Block
extern const char data_blocks_camera[] = R"glsl(

struct CameraBlock {
    mat4 proj;    // 16
    mat4 view;    // 16
    mat4 invProj; // 16
    mat4 invView; // 16
    mat4 trnView; // 16
    vec3 pos;     // 3
    float rmin;   // 1
    vec3 dir;     // 3
    float rmax;   // 1
    // Size: 88
};

)glsl";



/// A set of uniform-spiral disks
extern const char data_disks_uniform[] = R"glsl(

const vec2 disk6[6] = {
vec2(0.000000, 1.000000),
vec2(-0.787800, 0.454837),
vec2(-0.696923, -0.402369),
vec2(-0.000000, -0.683013),
vec2(0.467086, -0.269672),
vec2(0.311526, 0.179859) };

const vec2 disk12[12] = {
vec2(0.000000, 1.000000),
vec2(-0.478297, -0.828435),
vec2(0.787800, 0.454837),
vec2(-0.859123, 0.000000),
vec2(0.696923, -0.402369),
vec2(-0.373098, 0.646225),
vec2(-0.000000, -0.683013),
vec2(0.307225, 0.532129),
vec2(-0.467086, -0.269672),
vec2(0.455719, -0.000000),
vec2(-0.311526, 0.179859),
vec2(0.120746, -0.209139) };

const vec2 disk24[24] = {
vec2(0.000000, 1.000000),
vec2(-0.945383, -0.253315),
vec2(0.478297, -0.828435),
vec2(0.660140, 0.660140),
vec2(-0.787800, 0.454837),
vec2(-0.229019, -0.854711),
vec2(0.859123, -0.000000),
vec2(-0.215448, 0.804062),
vec2(-0.696923, -0.402369),
vec2(0.548724, -0.548724),
vec2(0.373098, 0.646225),
vec2(-0.690853, 0.185113),
vec2(-0.000000, -0.683013),
vec2(0.627333, 0.168093),
vec2(-0.307225, 0.532129),
vec2(-0.408575, -0.408575),
vec2(0.467086, -0.269672),
vec2(0.129092, 0.481777),
vec2(-0.455719, -0.000000),
vec2(0.106023, -0.395685),
vec2(0.311526, 0.179859),
vec2(-0.215357, 0.215357),
vec2(-0.120746, -0.209139),
vec2(0.158083, -0.042358) };

const vec2 disk4[4] = {
vec2(0.000000, 0.615793),
vec2(-0.786598, 0.000000),
vec2(-0.000000, -0.920423),
vec2(1.000000, -0.000000) };

const vec2 disk8[8] = {
vec2(0.000000, 0.374386),
vec2(-0.347073, -0.347073),
vec2(0.603291, -0.000000),
vec2(-0.501968, 0.501968),
vec2(-0.000000, -0.807897),
vec2(0.631658, 0.631658),
vec2(-0.960211, 0.000000),
vec2(0.707107, -0.707107) };

const vec2 disk16[16] = {
vec2(0.000000, 0.195923),
vec2(-0.098818, -0.238567),
vec2(0.226579, 0.226579),
vec2(-0.353385, -0.146377),
vec2(0.444372, -0.000000),
vec2(-0.467436, 0.193618),
vec2(0.400996, -0.400996),
vec2(-0.240175, 0.579834),
vec2(-0.000000, -0.687193),
vec2(0.285259, 0.688676),
vec2(-0.566849, -0.566849),
vec2(0.789866, 0.327173),
vec2(-0.903948, -0.000000),
vec2(0.874590, -0.362267),
vec2(-0.693039, 0.693039),
vec2(0.382683, -0.923880) };

)glsl";
