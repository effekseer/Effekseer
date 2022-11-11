#version 420
#ifdef GL_ARB_shader_draw_parameters
#extension GL_ARB_shader_draw_parameters : enable
#endif

out gl_PerVertex
{
    vec4 gl_Position;
};

struct VS_INPUT
{
    uint instanceID;
    vec2 a_VertexPosition;
};

struct VS_OUTPUT
{
    vec4 Position;
    vec4 v_Color;
};

layout(binding = 0, std140) uniform CB
{
    vec4 ID2TPos;
    layout(row_major) mat4 ViewMatrix;
    layout(row_major) mat4 ProjMatrix;
} CBVS0;

#ifdef GL_ARB_shader_draw_parameters
#define SPIRV_Cross_BaseInstance gl_BaseInstanceARB
#else
uniform int SPIRV_Cross_BaseInstance;
#endif
layout(location = 0) in vec2 input_a_VertexPosition;
layout(location = 0) out vec4 _entryPointOutput_v_Color;

vec2 rotate(vec2 pos, float deg)
{
    float c = cos(deg * 0.01745328865945339202880859375);
    float s = sin(deg * 0.01745328865945339202880859375);
    return mat2(vec2(c, -s), vec2(s, c)) * pos;
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

float fadeIn(float duration, float age, float lifetime)
{
    return clamp(age / duration, 0.0, 1.0);
}

float fadeOut(float duration, float age, float lifetime)
{
    return clamp((lifetime - age) / duration, 0.0, 1.0);
}

float fadeInOut(float fadeinDuration, float fadeoutDuration, float age, float lifetime)
{
    float param = fadeinDuration;
    float param_1 = age;
    float param_2 = lifetime;
    float param_3 = fadeoutDuration;
    float param_4 = age;
    float param_5 = lifetime;
    return fadeIn(param, param_1, param_2) * fadeOut(param_3, param_4, param_5);
}

VS_OUTPUT _main(VS_INPUT _input)
{
    int particleID = int(_input.instanceID);
    ivec2 ID2TPos2i = ivec2(int(CBVS0.ID2TPos.x), int(CBVS0.ID2TPos.y));
    ivec2 texPos = ivec2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y);
    vec4 data0 = vec4(0.0);
    vec4 data1 = vec4(0.0);
    float age = data1.x;
    float lifetime = data1.y;
    VS_OUTPUT _output;
    if (age >= lifetime)
    {
        _output.Position = vec4(0.0);
        _output.v_Color = vec4(0.0);
    }
    else
    {
        vec3 position = data0.xyz;
        vec2 param = _input.a_VertexPosition * 0.0030000000260770320892333984375;
        float param_1 = 45.0;
        position += vec3(rotate(param, param_1), 0.0);
        _output.Position = (CBVS0.ProjMatrix * CBVS0.ViewMatrix) * vec4(position, 1.0);
        vec3 param_2 = vec3(vec2(texPos), 0.0) / vec3(512.0);
        vec2 texCoord = vec2(snoise(param_2));
        _output.v_Color = vec4(0.0);
        float param_3 = 1.0;
        float param_4 = 10.0;
        float param_5 = age;
        float param_6 = lifetime;
        _output.v_Color.w *= fadeInOut(param_3, param_4, param_5, param_6);
    }
    return _output;
}

void main()
{
    VS_INPUT _input;
    _input.instanceID = uint((gl_InstanceID + SPIRV_Cross_BaseInstance));
    _input.a_VertexPosition = input_a_VertexPosition;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    gl_Position = flattenTemp.Position;
    _entryPointOutput_v_Color = flattenTemp.v_Color;
}

