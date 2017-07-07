//============================================================================//

// input data for full screen with TRIANGLE_STRIP /////

extern const char data_glsl_misc_screen[] = R"glsl(

const vec2 c_PositionArray[4] =
{
    vec2(-1.f, -1.f), vec2(-1.f, +1.f),
    vec2(+1.f, -1.f), vec2(+1.f, +1.f)
};

vec2 v_Position = c_PositionArray[gl_VertexID];

)glsl";


//============================================================================//

// functions for position reconstruction /////

extern const char data_glsl_funcs_position[] = R"glsl(

vec3 get_view_pos(sampler2D depthTex, vec2 texcrd, mat4 invProjMat)
{
    float depth = texture(depthTex, texcrd).r * 2.f - 1.f;

    vec4 projPos = vec4(texcrd * 2.f - 1.f, depth, 1.f);
    vec4 viewPosW = invProjMat * projPos;

    return viewPosW.xyz / viewPosW.w;
}

vec3 get_world_pos(vec3 viewPos, mat4 invViewMat)
{
   vec4 worldPosW = invViewMat * vec4(viewPos, 1.f);

   return worldPosW.xyz / worldPosW.w;
}

)glsl";


//============================================================================//

// functions for special sampling using depth /////

extern const char data_glsl_funcs_depth[] = R"glsl(

float linearise(float depth, float n, float f)
{
    return (2.f * n) / (f + n - depth * (f - n)) * (f - n);
}

float get_linear_depth(sampler2D depthTex, vec2 texcrd, float near, float far)
{
    return linearise(texture(depthTex, texcrd).r, near, far);
}

float nearest_depth_float ( vec2 texcrd, sampler2D sampler,
                            sampler2D depthFull, sampler2D depthSmall,
                            float thres, float near, float far )
{
    float depth = linearise(texture(depthFull, texcrd).r, near, far);
    vec4 depthGather = textureGather(depthSmall, texcrd);

    float distA = distance(depth, linearise(depthGather.x, near, far));
    float distB = distance(depth, linearise(depthGather.y, near, far));
    float distC = distance(depth, linearise(depthGather.z, near, far));
    float distD = distance(depth, linearise(depthGather.w, near, far));

    if (max(max(max(distA, distB), distC), distD) > thres)
    {
        vec4 gather = textureGather(sampler, texcrd);

        float minDist = distA; float result = gather.x;
        if (distB < minDist) { minDist = distB; result = gather.y; }
        if (distC < minDist) { minDist = distC; result = gather.z; }
        if (distD < minDist) { minDist = distD; result = gather.w; }

        return result;
    }

    return texture(sampler, texcrd).r;
}

vec3 nearest_depth_vec3 ( vec2 texcrd, sampler2D sampler,
                          sampler2D depthFull, sampler2D depthSmall,
                          float thres, float near, float far )
{
    float depth = linearise(texture(depthFull, texcrd).r, near, far);
    vec4 depthGather = textureGather(depthSmall, texcrd);

    float distA = distance(depth, linearise(depthGather.x, near, far));
    float distB = distance(depth, linearise(depthGather.y, near, far));
    float distC = distance(depth, linearise(depthGather.z, near, far));
    float distD = distance(depth, linearise(depthGather.w, near, far));

    if (max(max(max(distA, distB), distC), distD) > thres)
    {
        vec4 gatherR = textureGather(sampler, texcrd, 0);
        vec4 gatherG = textureGather(sampler, texcrd, 1);
        vec4 gatherB = textureGather(sampler, texcrd, 2);

        float minDist = distA; vec3 result = vec3(gatherR.x, gatherG.x, gatherB.x);
        if (distB < minDist) { minDist = distB; result = vec3(gatherR.y, gatherG.y, gatherB.y); }
        if (distC < minDist) { minDist = distC; result = vec3(gatherR.z, gatherG.z, gatherB.z); }
        if (distD < minDist) { minDist = distD; result = vec3(gatherR.w, gatherG.w, gatherB.w); }

        return result;
    }

    return texture(sampler, texcrd).rgb;
}

)glsl";


//============================================================================//

// Functions for generating random values from a seed /////

extern const char data_glsl_funcs_random[] = R"glsl(

float rand1(float seed)
{
    return fract(sin(seed * 12.9898f) * 43758.5453f);
}

float rand2(vec2 seed)
{
    return fract(sin(dot(seed, vec2(12.9898f, 78.233f))) * 43758.5453f);
}

float rand3(vec3 seed)
{
    return fract(sin(dot(seed, vec3(12.9898f, 78.233f, 151.7182f))) * 43758.5453f);
}

)glsl";


//============================================================================//

// functions for colour space manipulation /////

extern const char data_glsl_funcs_colour[] = R"glsl(

vec3 rgb_to_hsv(vec3 _c)
{
    vec3 c = clamp(_c, 0.f, 1.f);
    vec4 K = vec4(0.f, -1.f / 3.f, 2.f / 3.f, -1.f);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y); float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.f * d + e)), d / (q.x + e), q.x);
}

vec3 hsv_to_rgb(vec3 _c)
{
    vec3 c = vec3(_c.x, clamp(_c.y, 0.f, 1.f), _c.z);
    vec4 K = vec4(1.f, 2.f / 3.f, 1.f / 3.f, 3.f);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.f - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.f, 1.f), c.y);
}

vec3 rgb_to_hsl(vec3 _c)
{
    float minc = min(min(_c.r, _c.g), _c.b);
    float maxc = max(max(_c.r, _c.g), _c.b);
    float delta = maxc - minc;

    float hue = 0.f, sat = 0.f;
    float lum = (minc + maxc) * 0.5f;

    if (lum > 0.f && lum < 1.f)                     // check me
    {
        float mult = lum < 0.5f ? lum : 1.f - lum;
        sat = delta / (mult * 2.f);
    }

    vec3 masks = vec3
    (
        float(maxc == _c.r && maxc != _c.g),
        float(maxc == _c.g && maxc != _c.b),
        float(maxc == _c.b && maxc != _c.r)
    );

    vec3 adds = vec3
    (
        0.f + (_c.g - _c.b) / delta,
        2.f + (_c.b - _c.r) / delta,
        4.f + (_c.r - _c.g) / delta
    );

    hue += dot(adds, masks);
    hue *= (delta > 0.f ? 1.f : 0.f) / 6.f;
    if (hue < 0.f) hue += 1.f;

    return vec3(hue, sat, lum);
}

vec3 hsl_to_rgb(vec3 _c)
{
    vec3 xt = vec3
    (
        6.f * (_c.x - 2.f / 3.f),
        0.f, 6.f * (1.f - _c.x)
    );

    if (_c.x < 2.f / 3.f)
    {
        xt.r = 0.f;
        xt.g = 6.f * (2.f / 3.f - _c.x);
        xt.b = 6.f * (_c.x - 1.f / 3.f);
    }

    if (_c.x < 1.f / 3.f)
    {
        xt.b = 0.0;
        xt.r = 6.f * (1.f / 3.f - _c.x);
        xt.g = 6.f * _c.x;
    }

    xt = min(xt, 1.f);
    vec3 ct = 2.f * _c.y * xt - _c.y + 1.f;

    if (_c.z < 0.5f) return ct * _c.z;
    else return ct * (1.f - _c.z) + _c.z * 2.f - 1.f;
}

float rgb_to_luma(vec3 rgb)
{
    return dot(vec3(0.22f, 0.69f, 0.09f), rgb);
}

)glsl";


//============================================================================//

// a set of uniform spiral disks /////

extern const char data_glsl_misc_disks[] = R"glsl(

const vec2 c_Disk6[6] =
{
  vec2( 0.000000, +1.000000),
  vec2(-0.787800, +0.454837),
  vec2(-0.696923, -0.402369),
  vec2( 0.000000, -0.683013),
  vec2(+0.467086, -0.269672),
  vec2(+0.311526, +0.179859)
};

const vec2 c_Disk12[12] =
{
  vec2( 0.000000, +1.000000),
  vec2(-0.478297, -0.828435),
  vec2(+0.787800, +0.454837),
  vec2(-0.859123,  0.000000),
  vec2(+0.696923, -0.402369),
  vec2(-0.373098, +0.646225),
  vec2( 0.000000, -0.683013),
  vec2(+0.307225, +0.532129),
  vec2(-0.467086, -0.269672),
  vec2(+0.455719,  0.000000),
  vec2(-0.311526, +0.179859),
  vec2(+0.120746, -0.209139)
};

const vec2 c_Disk24[24] =
{
  vec2( 0.000000, +1.000000),
  vec2(-0.945383, -0.253315),
  vec2(+0.478297, -0.828435),
  vec2(+0.660140, +0.660140),
  vec2(-0.787800, +0.454837),
  vec2(-0.229019, -0.854711),
  vec2(+0.859123,  0.000000),
  vec2(-0.215448, +0.804062),
  vec2(-0.696923, -0.402369),
  vec2(+0.548724, -0.548724),
  vec2(+0.373098, +0.646225),
  vec2(-0.690853, +0.185113),
  vec2( 0.000000, -0.683013),
  vec2(+0.627333, +0.168093),
  vec2(-0.307225, +0.532129),
  vec2(-0.408575, -0.408575),
  vec2(+0.467086, -0.269672),
  vec2(+0.129092, +0.481777),
  vec2(-0.455719,  0.000000),
  vec2(+0.106023, -0.395685),
  vec2(+0.311526, +0.179859),
  vec2(-0.215357, +0.215357),
  vec2(-0.120746, -0.209139),
  vec2(+0.158083, -0.042358)
};

const vec2 c_Disk4[4] =
{
  vec2( 0.000000, +0.615793),
  vec2(-0.786598,  0.000000),
  vec2( 0.000000, -0.920423),
  vec2(+1.000000,  0.000000)
};

const vec2 c_Disk8[8] =
{
  vec2( 0.000000, +0.374386),
  vec2(-0.347073, -0.347073),
  vec2(+0.603291,  0.000000),
  vec2(-0.501968, +0.501968),
  vec2( 0.000000, -0.807897),
  vec2(+0.631658, +0.631658),
  vec2(-0.960211,  0.000000),
  vec2(+0.707107, -0.707107)
};

const vec2 c_Disk16[16] =
{
  vec2( 0.000000, +0.195923),
  vec2(-0.098818, -0.238567),
  vec2(+0.226579, +0.226579),
  vec2(-0.353385, -0.146377),
  vec2(+0.444372,  0.000000),
  vec2(-0.467436, +0.193618),
  vec2(+0.400996, -0.400996),
  vec2(-0.240175, +0.579834),
  vec2( 0.000000, -0.687193),
  vec2(+0.285259, +0.688676),
  vec2(-0.566849, -0.566849),
  vec2(+0.789866, +0.327173),
  vec2(-0.903948,  0.000000),
  vec2(+0.874590, -0.362267),
  vec2(-0.693039, +0.693039),
  vec2(+0.382683, -0.923880)
};

)glsl";


//============================================================================//
