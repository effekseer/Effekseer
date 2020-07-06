
static const char g_sprite_fs_lighting_src[] =
	R"(

IN lowp vec4 v_VColor;
IN mediump vec2 v_UV1;
IN mediump vec2 v_UV2;
IN mediump vec3 v_WorldP;
IN mediump vec3 v_WorldN;
IN mediump vec3 v_WorldT;
IN mediump vec3 v_WorldB;
IN mediump vec2 v_ScreenUV;

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

	vec3 texNormal = (TEX2D(NormalTexture, v_UV1.xy).xyz - 0.5) * 2.0;
	mat3 normalMatrix = mat3(v_WorldT.xyz, v_WorldB.xyz, v_WorldN.xyz );
	vec3 localNormal = normalize( normalMatrix * texNormal );
	float diffuse = max(0.0, dot(localNormal, LightDirection.xyz));
	
	FRAGCOLOR = v_VColor * TEX2D(ColorTexture, v_UV1.xy + UVOffset);
	if(FRAGCOLOR.w <= 0.0) discard;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	ApplyFlipbook(FRAGCOLOR, ColorTexture, flipbookParameter, v_VColor, vaFlipbookNextIndexUV, vaFlipbookRate);
    FRAGCOLOR.a *= TEX2D(uAlphaTexture, vaAlphaUV + UVOffset).a;
    if (FRAGCOLOR.a <= vaAlphaThreshold)
    {
        discard;
    }
)"
#endif

	R"(
	FRAGCOLOR.xyz = FRAGCOLOR.xyz * (LightColor.xyz * diffuse + LightAmbient.xyz);
}


)";