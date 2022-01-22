//#line 1 "perticle-render.frag.hlsl"

struct PS_INPUT {
    float4 Position : SV_POSITION;
    float4 v_Color : COLOR0;
};

struct PS_OUTPUT {
    float4 o_FragColor: SV_Target0;
};

PS_OUTPUT main(PS_INPUT input) {
 PS_OUTPUT output;
 output.o_FragColor = float4(input.v_Color.rgb, 1.0);
 return output;
}
