
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
uniform float4 flipbookParameter; // x:enable, y:interpolationType
)"
#endif

	R"(

void main()
{
	FRAGCOLOR = v_Color * TEX2D(ColorTexture, v_TexCoord.xy);

	if (LightingEnable)
	{
		vec3 texNormal = (TEX2D(NormalTexture, v_TexCoord.xy).xyz - 0.5) * 2.0;
		mat3 normalMatrix = mat3(v_Tangent.xyz, v_Binormal.xyz, v_Normal.xyz );
		vec3 localNormal = normalize( normalMatrix * texNormal );
		float diffuse = max(0.0, dot(localNormal, LightDirection.xyz));
		FRAGCOLOR.xyz = FRAGCOLOR.xyz * (LightColor.xyz * diffuse + LightAmbient.xyz);
	}

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	ApplyFlipbook(FRAGCOLOR, ColorTexture, flipbookParameter, v_Color, vaFlipbookNextIndexUV, vaFlipbookRate);
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
