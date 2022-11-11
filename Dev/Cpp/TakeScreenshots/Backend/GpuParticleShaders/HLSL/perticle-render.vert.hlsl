//#line 1 "perticle-render.vert.hlsl"
//#line 1 "./noise2d.hlsli"

float3 mod289(float3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float2 mod289(float2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float3 permute(float3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(float2 v)
  {
  const float4 C = float4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  float2 i  = floor(v + dot(v, C.yy) );
  float2 x0 = v -   i + dot(i, C.xx);

// Other corners
  float2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  float4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  float3 p = permute( permute( i.y + float3(0.0, i1.y, 1.0 ))
		+ i.x + float3(0.0, i1.x, 1.0 ));

  float3 m = max(0.5 - float3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  float3 x = 2.0 * frac(p * C.www) - 1.0;
  float3 h = abs(x) - 0.5;
  float3 ox = floor(x + 0.5);
  float3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  float3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}


float packVec3(float3 v) {
    uint3 i = uint3((v + 1.0) * 0.5 * 1023.0);
    return asfloat(i.x | (i.y << 10) | (i.z << 20));
}

float3 unpackVec3(float s) {
    uint bits = asuint(s);
    float3 v = float3(uint3(bits, bits >> 10, bits >> 20) & 1023u);
    return v / 1023.0 * 2.0 - 1.0;
}

float rand(float2 seed) {
    return frac(sin(dot(seed, float2(12.9898, 78.233))) * 43758.5453);
}

float3 noise3(float3 seed) {
    return float3(snoise(seed.xyz), snoise(seed.yzx), snoise(seed.zxy));
}
//#line 2 "perticle-render.vert.hlsl"

struct VS_INPUT {

    uint instanceID : SV_InstanceID;
    float2 a_VertexPosition: POSITION0;
};

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
    float4 v_Color: COLOR0;
};

Texture2D ParticleData0 : register(t0);
SamplerState ParticleData0Sampler : register(s0);
Texture2D ParticleData1 : register(t1);
SamplerState ParticleData1Sampler : register(s1);
Texture2D ColorTable : register(t2);
SamplerState ColorTableSampler : register(s2);

cbuffer CB : register(b0) {
    float4 ID2TPos;
    float4x4 ViewMatrix;
    float4x4 ProjMatrix;
};

float2 rotate(float2 pos, float deg) {
    const float toRad = 3.141592 / 180.0;
    float c = cos(deg * toRad);
    float s = sin(deg * toRad);
    return mul(pos, float2x2(c, -s, s, c));
}

float fadeIn(float duration, float age, float lifetime) {
    return clamp(age / duration, 0.0, 1.0);
}

float fadeOut(float duration, float age, float lifetime) {
    return clamp((lifetime - age) / duration, 0.0, 1.0);
}

float fadeInOut(float fadeinDuration, float fadeoutDuration, float age, float lifetime) {
    return fadeIn(fadeinDuration, age, lifetime) * fadeOut(fadeoutDuration, age, lifetime);
}

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    int particleID = input.instanceID;
    int2 ID2TPos2i = int2(ID2TPos.x,ID2TPos.y);
    int2 texPos = int2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y);
    //float4 data0 = ParticleData0.Sample(ParticleData0Sampler, float4((float)texPos.x, (float)texPos.y, 0, 0));
    //float4 data1 = ParticleData1.Sample(ParticleData1Sampler, float4((float)texPos.x, (float)texPos.y, 0, 0));
    //float4 data0 = ParticleData0.SampleLevel(ParticleData0Sampler, (float2)texPos, 0);
    //float4 data1 = ParticleData1.SampleLevel(ParticleData1Sampler, (float2)texPos, 0);
	float4 data0 = ParticleData0.Load(int3(texPos, 0));
	float4 data1 = ParticleData1.Load(int3(texPos, 0));

    float age = data1.x;
    float lifetime = data1.y;

    if (age >= lifetime) {
        output.Position = float4(0.0,0.0,0.0,0.0);
        output.v_Color = float4(0.0,0.0,0.0,0.0);
    }
    else {
        float3 position = data0.xyz;
        position.xyz += float3(rotate(input.a_VertexPosition * 0.003, 45.0), 0.0);
        //output.Position = ProjMatrix * ViewMatrix * float4(position, 1.0);
        output.Position = float4(position, 1.0);
        output.Position = mul(ViewMatrix, output.Position);
        output.Position = mul(ProjMatrix, output.Position);

        float v = snoise(float2(texPos) / 512.0);
        float2 texCoord = float2(v,v);
        //output.v_Color = ColorTable.Sample(ColorTableSampler, float4(texCoord, 0, 0));
        output.v_Color = ColorTable.SampleLevel(ColorTableSampler, texCoord, 0);
        output.v_Color.a *= fadeInOut(1.0, 10.0, age, lifetime);
    }

    return output;
}
