struct VS_INPUT {
    float3 a_Particle: POSITION0;  // x:ParticleID, y:Lifetime, z:Seed
    float3 a_Position : POSITION1;
    float3 a_Direction : POSITION2;
};

struct VS_OUTPUT {
    float4 Position : SV_POSITION;
    float3 v_Particle: TEXCOORD0;
    float3 v_Position : POSITION0;
    float3 v_Direction : TEXCOORD1;
};

cbuffer CB : register(b0) {
    float4 ID2TPos;
    float4 TPos2VPos;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    
    int particleID = int(input.a_Particle.x);
    int2 ID2TPos2i= int2(int(ID2TPos.x),int(ID2TPos.y));
    float2 glpos = float2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y) * TPos2VPos.xy + TPos2VPos.zw;
    output.Position = float4(glpos, 0.0, 1.0);
    //gl_PointSize = 1.0;
    output.v_Particle = input.a_Particle;
    output.v_Position = input.a_Position;
    output.v_Direction = input.a_Direction;

    return output;
}
