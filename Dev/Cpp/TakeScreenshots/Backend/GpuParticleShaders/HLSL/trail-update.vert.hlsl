#line 1 "trail-update.vert.hlsl"

struct VS_INPUT {
    float2 a_Position : POSITION0;
    float2 a_TexUV : TEXCOORD0;
};

struct VS_OUTPUT {
    float4 Position: SV_POSITION;
    float2 TexUV: TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Position = float4(input.a_Position, 0.0, 1.0);
    output.TexUV = input.a_TexUV;//input.a_Position;
    return output;
}
