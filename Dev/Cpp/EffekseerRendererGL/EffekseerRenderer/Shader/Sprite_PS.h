

static const char g_sprite_fs_texture_src[] =
	R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
IN mediump vec2 vaAlphaUV;
IN mediump vec2 vaUVDistortionUV;
IN mediump float vaFlipbookRate;
IN mediump vec2 vaFlipbookNextIndexUV;
IN mediump float vaAlphaThreshold;
)"
#endif

	R"(
uniform sampler2D uTexture0;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
uniform sampler2D uAlphaTexture;
uniform sampler2D uuvDistortionTexture;
uniform float4 flipbookParameter; // x:enable, y:interpolationType
uniform float4 uvDistortionParameter; // x:intensity
)"
#endif

	R"(

void main()
{
    vec2 UVOffset = vec2(0.0, 0.0);
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__

	R"(
    UVOffset = TEX2D(uuvDistortionTexture, vaUVDistortionUV).xy * 2.0 - 1.0;
    UVOffset *= uvDistortionParameter.x;
)"
#endif

	R"(
	FRAGCOLOR = vaColor * TEX2D(uTexture0, vaTexCoord.xy + UVOffset);
	if(FRAGCOLOR.w <= 0.0) discard;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	ApplyFlipbook(FRAGCOLOR, uTexture0, flipbookParameter, vaColor, vaFlipbookNextIndexUV, vaFlipbookRate);
    FRAGCOLOR.a *= TEX2D(uAlphaTexture, vaAlphaUV + UVOffset).a;
    if (FRAGCOLOR.a <= vaAlphaThreshold)
    {
        discard;
    }
)"
#endif

	R"(
}
)";