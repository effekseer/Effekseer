
struct PS_INPUT {
    float3 v_Color	: COLOR0;
};

struct PS_OUTPUT {
    float4 o_FragColor: SV_Target0;
};

PS_OUTPUT main(PS_INPUT input) {
	PS_OUTPUT output;
	output.o_FragColor = float4(input.v_Color, 1.0);
	return output;
}
