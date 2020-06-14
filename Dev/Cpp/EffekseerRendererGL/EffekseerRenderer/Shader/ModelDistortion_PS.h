

static const char g_model_distortion_fs_src[] =

	R"(
IN mediump vec4 v_Normal;
IN mediump vec4 v_Binormal;
IN mediump vec4 v_Tangent;
IN mediump vec4 v_TexCoord;
IN mediump vec4 v_Pos;
IN lowp vec4 v_Color;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
IN mediump vec2 vaAlphaUV;
IN mediump float vaFlipbookRate;
IN mediump vec2 vaFlipbookNextIndexUV;
IN mediump float vaAlphaThreshold;
)"
#endif

	R"(
uniform sampler2D uTexture0;
uniform sampler2D uBackTexture0;

uniform	vec4	g_scale;
uniform	vec4	mUVInversedBack;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
uniform sampler2D uAlphaTexture;
uniform float4 flipbookParameter; // x:enable, y:interpolationType
)"
#endif

	R"(

void main() {
	if (TextureEnable)
	{
		FRAGCOLOR = TEX2D(uTexture0, v_TexCoord.xy);
	}
	else
	{
		FRAGCOLOR = vec4(1.0, 1.0, 1.0, 1.0);
	}

	FRAGCOLOR.a = FRAGCOLOR.a * v_Color.a;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	ApplyFlipbook(FRAGCOLOR, uTexture0, flipbookParameter, v_Color, vaFlipbookNextIndexUV, vaFlipbookRate);
    FRAGCOLOR.a *= TEX2D(uAlphaTexture, vaAlphaUV).a;
    if (FRAGCOLOR.a <= vaAlphaThreshold)
    {
        discard;
    }
)"
#endif

	R"(

	vec2 pos = v_Pos.xy / v_Pos.w;
	vec2 posU = v_Tangent.xy / v_Tangent.w;
	vec2 posR = v_Binormal.xy / v_Binormal.w;

	float xscale = (FRAGCOLOR.x * 2.0 - 1.0) * v_Color.x * g_scale.x;
	float yscale = (FRAGCOLOR.y * 2.0 - 1.0) * v_Color.y * g_scale.x;

	vec2 uv = pos + (posR - pos) * xscale + (posU - pos) * yscale;

	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = (uv.y + 1.0) * 0.5;
//	uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	vec3 color = TEX2D(uBackTexture0, uv).xyz;
	FRAGCOLOR.xyz = color;
}
)";