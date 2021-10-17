struct VS_INPUT {
    float2 a_Position : POSITION0;
};

struct VS_OUTPUT {
    float4 Position: SV_POSITION;
	float2 ScreenUV: TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;
    output.Position = float4(input.a_Position, 0.0, 1.0);
	output.ScreenUV = input.a_Position;
    return output;
}
