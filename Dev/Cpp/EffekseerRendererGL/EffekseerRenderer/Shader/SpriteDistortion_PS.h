
static const char g_sprite_fs_texture_distortion_src[] =
	R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;
IN mediump vec4 vaPos;
IN mediump vec4 vaPosR;
IN mediump vec4 vaPosU;
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
	vec4 color = TEX2D(uTexture0, vaTexCoord.xy);
	color.w = color.w * vaColor.w;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	ApplyFlipbook(color, uTexture0, flipbookParameter, vaColor, vaFlipbookNextIndexUV, vaFlipbookRate);
    color.a *= TEX2D(uAlphaTexture, vaAlphaUV).a;
    if (color.a <= vaAlphaThreshold)
    {
        discard;
    }
)"
#endif

	R"(

	vec2 pos = vaPos.xy / vaPos.w;
	vec2 posU = vaPosU.xy / vaPosU.w;
	vec2 posR = vaPosR.xy / vaPosR.w;

	vec2 uv = pos + (posR - pos) * (color.x * 2.0 - 1.0) * vaColor.x * g_scale.x + (posU - pos) * (color.y * 2.0 - 1.0) * vaColor.y * g_scale.x;
	uv.x = (uv.x + 1.0) * 0.5;
	uv.y = (uv.y + 1.0) * 0.5;
	//uv.y = 1.0 - (uv.y + 1.0) * 0.5;

	uv.y = mUVInversedBack.x + mUVInversedBack.y * uv.y;

	color.xyz = TEX2D(uBackTexture0, uv).xyz;
	
	FRAGCOLOR = color;
}
)";