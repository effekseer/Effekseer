
static const char g_model_vs_src[] =
	R"(
IN vec4 a_Position;
IN vec4 a_Normal;
IN vec4 a_Binormal;
IN vec4 a_Tangent;
IN vec4 a_TexCoord;
IN vec4 a_Color;
)"
#if defined(MODEL_SOFTWARE_INSTANCING)
	R"(
IN float a_InstanceID;
IN vec4 a_UVOffset;
IN vec4 a_ModelColor;
)"
#endif

	R"(
OUT mediump vec4 v_Normal;
OUT mediump vec4 v_Binormal;
OUT mediump vec4 v_Tangent;
OUT mediump vec4 v_TexCoord;
OUT lowp vec4 v_Color;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
OUT vec2 vaAlphaUV;
OUT vec2 vaUVDistortionUV;
OUT float vaFlipbookRate;
OUT vec2 vaFlipbookNextIndexUV;
OUT float vaAlphaThreshold;
)"
#endif

#if defined(MODEL_SOFTWARE_INSTANCING)
	R"(
uniform mat4 ModelMatrix[20];
uniform vec4 UVOffset[20];
uniform vec4 ModelColor[20];
)"
#else
	R"(
uniform mat4 ModelMatrix;
uniform vec4 UVOffset;
uniform vec4 ModelColor;
)"
#endif

	R"(
uniform mat4 ProjectionMatrix;
uniform vec4 LightDirection;
uniform vec4 LightColor;
uniform vec4 LightAmbient;
uniform vec4 mUVInversed;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
uniform vec4 atAlphaUV;
uniform vec4 atUVDistortionUV;
uniform vec4 atFlipbookIndexAndNextRate;
uniform vec4 atAlphaThreshold;
)"
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
uniform vec4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
)"
#endif

	R"(
void main()
{
)"
#if defined(MODEL_SOFTWARE_INSTANCING)
	R"(
	mat4 modelMatrix = ModelMatrix[int(a_InstanceID)];
	vec4 uvOffset = a_UVOffset;
	vec4 modelColor = a_ModelColor;
)"
#else
	R"(
	mat4 modelMatrix = ModelMatrix;
	vec4 uvOffset = UVOffset;
	vec4 modelColor = ModelColor * a_Color;
)"
#endif
	R"(
	vec4 localPosition = modelMatrix * a_Position;
	gl_Position = ProjectionMatrix * localPosition;

	v_TexCoord.xy = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;

	if (LightingEnable)
	{
		mat3 lightMatrix = mat3(modelMatrix[0].xyz, modelMatrix[1].xyz, modelMatrix[2].xyz);
		vec3 localNormal = normalize( lightMatrix * a_Normal.xyz );

		v_Normal = vec4(localNormal, 1.0);
		v_Binormal = vec4(normalize( lightMatrix * a_Binormal.xyz ), 1.0);
		v_Tangent = vec4(normalize( lightMatrix * a_Tangent.xyz ), 1.0);
	}

	v_Color = modelColor;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
    // alpha texture
    vaAlphaUV.x = v_TexCoord.x * atAlphaUV.z + atAlphaUV.x;
	vaAlphaUV.y = v_TexCoord.y * atAlphaUV.w + atAlphaUV.y;
	vaAlphaUV.y = mUVInversed.x + mUVInversed.y * vaAlphaUV.y;

    vaUVDistortionUV.x = v_TexCoord.x * atUVDistortionUV.z + atUVDistortionUV.x;
	vaUVDistortionUV.y = v_TexCoord.y * atUVDistortionUV.w + atUVDistortionUV.y;
	vaUVDistortionUV.y = mUVInversed.x + mUVInversed.y * vaUVDistortionUV.y;

	vaFlipbookRate = 0.0;
	vaFlipbookNextIndexUV = vec2(0.0, 0.0);

	ApplyFlipbookVS(vaFlipbookRate, vaFlipbookNextIndexUV, mflipbookParameter, atFlipbookIndexAndNextRate.x, v_TexCoord.xy);
	vaAlphaThreshold = atAlphaThreshold.x;
)"
#endif

	R"(
	v_TexCoord.y = mUVInversed.x + mUVInversed.y * v_TexCoord.y;
}
)";