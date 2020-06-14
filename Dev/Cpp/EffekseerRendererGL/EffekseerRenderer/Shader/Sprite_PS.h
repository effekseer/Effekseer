

static const char g_sprite_fs_texture_src[] =
	R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;
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

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
uniform sampler2D uAlphaTexture;
uniform float4 flipbookParameter; // x:enable, y:interpolationType
)"
#endif

	R"(

void main()
{
	FRAGCOLOR = vaColor * TEX2D(uTexture0, vaTexCoord.xy);

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	ApplyFlipbook(FRAGCOLOR, uTexture0, flipbookParameter, vaColor, vaFlipbookNextIndexUV, vaFlipbookRate);
    FRAGCOLOR.a *= TEX2D(uAlphaTexture, vaAlphaUV).a;
    if (FRAGCOLOR.a <= vaAlphaThreshold)
    {
        discard;
    }
)"
#endif

	R"(
}
)";