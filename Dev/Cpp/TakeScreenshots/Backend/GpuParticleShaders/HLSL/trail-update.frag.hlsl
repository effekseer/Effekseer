#line 1 "trail-update.frag.hlsl"
#line 1 "./noise.hlsli"


float3 mod289(float3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 mod289(float4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float4 permute(float4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

float4 taylorInvSqrt(float4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(float3 v)
  {
  const float2 C = float2(1.0/6.0, 1.0/3.0);
  const float4 D = float4(0.0, 0.5, 1.0, 2.0);


  float3 i = floor(v + dot(v, C.yyy) );
  float3 x0 = v - i + dot(i, C.xxx) ;


  float3 g = step(x0.yzx, x0.xyz);
  float3 l = 1.0 - g;
  float3 i1 = min( g.xyz, l.zxy );
  float3 i2 = max( g.xyz, l.zxy );





  float3 x1 = x0 - i1 + C.xxx;
  float3 x2 = x0 - i2 + C.yyy;
  float3 x3 = x0 - D.yyy;


  i = mod289(i);
  float4 p = permute( permute( permute(
             i.z + float4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + float4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + float4(0.0, i1.x, i2.x, 1.0 ));



  float n_ = 0.142857142857;
  float3 ns = n_ * D.wyz - D.xzx;

  float4 j = p - 49.0 * floor(p * ns.z * ns.z);

  float4 x_ = floor(j * ns.z);
  float4 y_ = floor(j - 7.0 * x_ );

  float4 x = x_ *ns.x + ns.yyyy;
  float4 y = y_ *ns.x + ns.yyyy;
  float4 h = 1.0 - abs(x) - abs(y);

  float4 b0 = float4( x.xy, y.xy );
  float4 b1 = float4( x.zw, y.zw );



  float4 s0 = floor(b0)*2.0 + 1.0;
  float4 s1 = floor(b1)*2.0 + 1.0;
  float4 sh = -step(h, float4(0.0,0.0,0.0,0.0));

  float4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  float4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  float3 p0 = float3(a0.xy,h.x);
  float3 p1 = float3(a0.zw,h.y);
  float3 p2 = float3(a1.xy,h.z);
  float3 p3 = float3(a1.zw,h.w);


  float4 norm = taylorInvSqrt(float4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;


  float4 m = max(0.6 - float4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, float4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
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
#line 2 "trail-update.frag.hlsl"

Texture2D i_Position : register(t0);
SamplerState i_PositionSampler : register(s0);
Texture2D i_Velocity : register(t1);
SamplerState i_VelocitySampler : register(s1);

struct PS_INPUT {
    float2 TexUV: TEXCOORD0;
};

struct PS_OUTPUT {
    float4 o_Histories: COLOR0;
};

PS_OUTPUT main(PS_INPUT input) {
 PS_OUTPUT output;


 float4 position = i_Position.Sample(i_PositionSampler, float4(input.TexUV, 0, 0));
 float4 velocity = i_Velocity.Sample(i_VelocitySampler, float4(input.TexUV, 0, 0));


 output.o_Histories = float4(position.xyz, packVec3(normalize(velocity.xyz)));

 return output;
}
