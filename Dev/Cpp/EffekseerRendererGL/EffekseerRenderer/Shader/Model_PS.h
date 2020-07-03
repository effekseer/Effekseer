
static const char g_model_fs_src[] =
	R"(

IN mediump vec4 v_Normal;
IN mediump vec4 v_Binormal;
IN mediump vec4 v_Tangent;
IN mediump vec4 v_TexCoord;
IN lowp vec4 v_Color;

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

uniform sampler2D ColorTexture;
uniform sampler2D NormalTexture;
uniform vec4 LightDirection;
uniform vec4 LightColor;
uniform vec4 LightAmbient;

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
	FRAGCOLOR = v_Color * TEX2D(ColorTexture, v_TexCoord.xy + UVOffset);

	if (LightingEnable)
	{
		vec3 texNormal = (TEX2D(NormalTexture, v_TexCoord.xy).xyz - 0.5) * 2.0;
		mat3 normalMatrix = mat3(v_Tangent.xyz, v_Binormal.xyz, v_Normal.xyz );
		vec3 localNormal = normalize( normalMatrix * texNormal );
		float diffuse = max(0.0, dot(localNormal, LightDirection.xyz));
		FRAGCOLOR.xyz = FRAGCOLOR.xyz * (LightColor.xyz * diffuse + LightAmbient.xyz);
	}
	if(FRAGCOLOR.w <= 0.0) discard;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	ApplyFlipbook(FRAGCOLOR, ColorTexture, flipbookParameter, v_Color, vaFlipbookNextIndexUV, vaFlipbookRate);
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
