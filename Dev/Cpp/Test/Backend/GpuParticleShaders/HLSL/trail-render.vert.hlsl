//#line 1 "trail-render.vert.hlsl"
//#line 1 "./noise.hlsli"


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
//#line 2 "trail-render.vert.hlsl"
//#line 1 "./noise2D.hlsli"
//#line 12 "./noise2D.hlsli"
float2 mod289(float2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float3 permute(float3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(float2 v)
  {
  const float4 C = float4(0.211324865405187,
                      0.366025403784439,
                     -0.577350269189626,
                      0.024390243902439);

  float2 i = floor(v + dot(v, C.yy) );
  float2 x0 = v - i + dot(i, C.xx);


  float2 i1;


  i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);



  float4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;


  i = mod289(i);
  float3 p = permute( permute( i.y + float3(0.0, i1.y, 1.0 ))
  + i.x + float3(0.0, i1.x, 1.0 ));

  float3 m = max(0.5 - float3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;




  float3 x = 2.0 * frac(p * C.www) - 1.0;
  float3 h = abs(x) - 0.5;
  float3 ox = floor(x + 0.5);
  float3 a0 = x - ox;



  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );


  float3 g;
  g.x = a0.x * x0.x + h.x * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}
//#line 3 "trail-render.vert.hlsl"
//#line 1 "./render-utils.hlsli"

float fadeIn(float duration, float age, float lifetime) {
 return clamp(age / duration, 0.0, 1.0);
}

float fadeOut(float duration, float age, float lifetime) {
 return clamp((lifetime - age) / duration, 0.0, 1.0);
}

float fadeInOut(float fadeinDuration, float fadeoutDuration, float age, float lifetime) {
 return fadeIn(fadeinDuration, age, lifetime) * fadeOut(fadeoutDuration, age, lifetime);
}
//#line 4 "trail-render.vert.hlsl"

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
Texture2DArray Histories : register(t3);
SamplerState HistoriesSampler : register(s3);

cbuffer CB : register(b0) {
    float4 ID2TPos;
    float4x4 ViewMatrix;
    float4x4 ProjMatrix;
 float4 Trail;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

 int particleID = input.instanceID;
 int2 ID2TPos2i = int2(ID2TPos.x, ID2TPos.y);
 int2 texPos = int2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y);
 //float4 data0 = ParticleData0.Sample(ParticleData0Sampler, float4(texPos, 0, 0));
 //float4 data1 = ParticleData1.Sample(ParticleData1Sampler, float4(texPos, 0, 0));
 //float4 data0 = ParticleData0.SampleLevel(ParticleData0Sampler, (float2)texPos, 0);
 //float4 data1 = ParticleData1.SampleLevel(ParticleData1Sampler, (float2)texPos, 0);
 float4 data0 = ParticleData0.Load(int3(texPos, 0));
 float4 data1 = ParticleData1.Load(int3(texPos, 0));

 float age = data1.x;
 float lifetime = data1.y;

 if (age >= lifetime || age <= 0.0) {
  output.Position = float4(0.0, 0.0, 0.0, 0.0);
  output.v_Color = float4(0.0, 0.0, 0.0, 0.0);
 } else {
  float historyID = input.a_VertexPosition.x * min(float(Trail.y), age);
  float3 position, direction;
  if (historyID >= 1.0) {
   int texIndex = (int(Trail.x) + int(historyID) - 1) % int(Trail.y);

   //float4 trailData = Histories.Sample(HistoriesSampler, float4(texPos, texIndex, 0));
   //float4 trailData = Histories.SampleLevel(HistoriesSampler, float3(texPos, texIndex), 0);
   float4 trailData = Histories.Load(int4(texPos, texIndex, 0)); // is it correct?

   position = trailData.xyz;
   direction = unpackVec3(trailData.w);
  } else {
   position = data0.xyz;
   direction = unpackVec3(data0.w);
  }
  float width = 0.05;
  float3 vertex = cross(float3(0.0, 1.0, 0.0), direction) * input.a_VertexPosition.y * width;



  //output.Position = ProjMatrix * (ViewMatrix * float4(position.xyz + vertex, 1.0));
  output.Position = float4(position.xyz + vertex, 1.0);
  output.Position = mul(ViewMatrix, output.Position);
  output.Position = mul(ProjMatrix, output.Position);

  float v = snoise(float3(texPos, 0) / 512.0);
  float2 texCoord = float2(v,v);
  //float2 texCoord = float2(snoise(float2(texPos) / 512.0));
  output.v_Color = ColorTable.SampleLevel(ColorTableSampler, texCoord, 0);
  output.v_Color.a *= 0.5 * fadeInOut(10.0, 10.0, age, lifetime);
 }

 return output;
}
