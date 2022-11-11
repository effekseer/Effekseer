#version 420

struct PS_INPUT
{
    vec2 ScreenUV;
};

struct VS_OUTPUT
{
    vec4 o_ParticleData0;
    vec4 o_ParticleData1;
};

layout(binding = 0, std140) uniform CB
{
    vec4 DeltaTime;
} CBVS0;

layout(binding = 0) uniform sampler2D Sampler_i_ParticleData0Sampler;
layout(binding = 1) uniform sampler2D Sampler_i_ParticleData1Sampler;

layout(location = 0) in vec2 input_ScreenUV;
layout(location = 0) out vec4 _entryPointOutput_o_ParticleData0;
layout(location = 1) out vec4 _entryPointOutput_o_ParticleData1;

vec3 unpackVec3(float s)
{
    uint bits = floatBitsToUint(s);
    vec3 v = vec3(uvec3(bits, bits >> uint(10), bits >> uint(20)) & uvec3(1023u));
    return ((v / vec3(1023.0)) * 2.0) - vec3(1.0);
}

vec3 mod289(vec3 x)
{
    return x - (floor(x * 0.00346020772121846675872802734375) * 289.0);
}

vec4 mod289(vec4 x)
{
    return x - (floor(x * 0.00346020772121846675872802734375) * 289.0);
}

vec4 permute(vec4 x)
{
    vec4 param = ((x * 34.0) + vec4(1.0)) * x;
    return mod289(param);
}

vec4 taylorInvSqrt(vec4 r)
{
    return vec4(1.792842864990234375) - (r * 0.8537347316741943359375);
}

float snoise(vec3 v)
{
    vec3 i = floor(v + vec3(dot(v, vec3(0.3333333432674407958984375))));
    vec3 x0 = (v - i) + vec3(dot(i, vec3(0.16666667163372039794921875)));
    vec3 g = step(x0.yzx, x0);
    vec3 l = vec3(1.0) - g;
    vec3 i1 = min(g, l.zxy);
    vec3 i2 = max(g, l.zxy);
    vec3 x1 = (x0 - i1) + vec3(0.16666667163372039794921875);
    vec3 x2 = (x0 - i2) + vec3(0.3333333432674407958984375);
    vec3 x3 = x0 - vec3(0.5);
    vec3 param = i;
    i = mod289(param);
    vec4 param_1 = vec4(i.z) + vec4(0.0, i1.z, i2.z, 1.0);
    vec4 param_2 = (permute(param_1) + vec4(i.y)) + vec4(0.0, i1.y, i2.y, 1.0);
    vec4 param_3 = (permute(param_2) + vec4(i.x)) + vec4(0.0, i1.x, i2.x, 1.0);
    vec4 p = permute(param_3);
    float n_ = 0.14285714924335479736328125;
    vec3 ns = (vec3(2.0, 0.5, 1.0) * n_) - vec3(0.0, 1.0, 0.0);
    vec4 j = p - (floor((p * ns.z) * ns.z) * 49.0);
    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - (x_ * 7.0));
    vec4 x = (x_ * ns.x) + ns.yyyy;
    vec4 y = (y_ * ns.x) + ns.yyyy;
    vec4 h = (vec4(1.0) - abs(x)) - abs(y);
    vec4 b0 = vec4(x.xy, y.xy);
    vec4 b1 = vec4(x.zw, y.zw);
    vec4 s0 = (floor(b0) * 2.0) + vec4(1.0);
    vec4 s1 = (floor(b1) * 2.0) + vec4(1.0);
    vec4 sh = -step(h, vec4(0.0));
    vec4 a0 = b0.xzyw + (s0.xzyw * sh.xxyy);
    vec4 a1 = b1.xzyw + (s1.xzyw * sh.zzww);
    vec3 p0 = vec3(a0.xy, h.x);
    vec3 p1 = vec3(a0.zw, h.y);
    vec3 p2 = vec3(a1.xy, h.z);
    vec3 p3 = vec3(a1.zw, h.w);
    vec4 param_4 = vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3));
    vec4 norm = taylorInvSqrt(param_4);
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    vec4 m = max(vec4(0.60000002384185791015625) - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), vec4(0.0));
    m *= m;
    return 42.0 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}

vec3 noise3(vec3 seed)
{
    vec3 param = seed;
    vec3 param_1 = seed.yzx;
    vec3 param_2 = seed.zxy;
    return vec3(snoise(param), snoise(param_1), snoise(param_2));
}

float packVec3(vec3 v)
{
    uvec3 i = uvec3(((v + vec3(1.0)) * 0.5) * 1023.0);
    return uintBitsToFloat((i.x | (i.y << uint(10))) | (i.z << uint(20)));
}

VS_OUTPUT _main(PS_INPUT _input)
{
    vec4 data0 = textureLod(Sampler_i_ParticleData0Sampler, vec2(vec4(_input.ScreenUV, 0.0, 0.0).xy), 0.0);
    vec4 data1 = textureLod(Sampler_i_ParticleData1Sampler, vec2(vec4(_input.ScreenUV, 0.0, 0.0).xy), 0.0);
    vec3 position = data0.xyz;
    float param = data1.w;
    vec3 direction = unpackVec3(param);
    float DeltaTime1f = CBVS0.DeltaTime.x;
    data1.x += DeltaTime1f;
    float lifetimeRatio = data1.x / data1.y;
    vec3 velocity = vec3(0.0);
    velocity += (direction * mix(0.00999999977648258209228515625, 0.0, lifetimeRatio));
    vec3 param_1 = position;
    velocity += (noise3(param_1) * 0.00999999977648258209228515625);
    vec3 targetPosition = vec3(0.20000000298023223876953125, 0.0, 0.0);
    vec3 diff = targetPosition - position;
    velocity += (normalize(diff) * 0.00999999977648258209228515625);
    position += velocity;
    vec3 _567 = normalize(velocity);
    bvec3 _569 = bvec3(length(velocity) < 9.9999997473787516355514526367188e-05);
    direction = vec3(_569.x ? direction.x : _567.x, _569.y ? direction.y : _567.y, _569.z ? direction.z : _567.z);
    vec3 param_2 = direction;
    VS_OUTPUT _output;
    _output.o_ParticleData0 = vec4(position, packVec3(param_2));
    _output.o_ParticleData1 = data1;
    return _output;
}

void main()
{
    PS_INPUT _input;
    _input.ScreenUV = input_ScreenUV;
    PS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_o_ParticleData0 = flattenTemp.o_ParticleData0;
    _entryPointOutput_o_ParticleData1 = flattenTemp.o_ParticleData1;
}

