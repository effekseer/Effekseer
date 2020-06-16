
static const char g_model_distortion_vs_src[] = "IN vec4 a_Position;\n"
												"IN vec4 a_Normal;\n"
												"IN vec4 a_Binormal;\n"
												"IN vec4 a_Tangent;\n"
												"IN vec4 a_TexCoord;\n"
												"IN vec4 a_Color;\n"
#if defined(MODEL_SOFTWARE_INSTANCING)
												"IN float a_InstanceID;\n"
												"IN vec4 a_UVOffset;\n"
												"IN vec4 a_ModelColor;\n"
#endif

												R"(
OUT mediump vec4 v_Normal;
OUT mediump vec4 v_Binormal;
OUT mediump vec4 v_Tangent;
OUT mediump vec4 v_TexCoord;
OUT mediump vec4 v_Pos;
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
												"uniform mat4 ModelMatrix[20];\n"
												"uniform vec4 UVOffset[20];\n"
												"uniform vec4 ModelColor[20];\n"
#else
												"uniform mat4 ModelMatrix;\n"
												"uniform vec4 UVOffset;\n"
												"uniform vec4 ModelColor;\n"
#endif
												"uniform mat4 ProjectionMatrix;\n"
												"uniform vec4 mUVInversed;\n"

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

												"void main() {\n"
#if defined(MODEL_SOFTWARE_INSTANCING)
												"	mat4 modelMatrix = ModelMatrix[int(a_InstanceID)];\n"
												"	vec4 uvOffset = a_UVOffset;\n"
												"	vec4 modelColor = a_ModelColor;\n"
#else
												"	mat4 modelMatrix = ModelMatrix;\n"
												"	vec4 uvOffset = UVOffset;\n"
												"	vec4 modelColor = ModelColor;\n"
#endif

												R"(
	vec4 localPosition = vec4( a_Position.x, a_Position.y, a_Position.z, 1.0 );
	vec4 localNormal = vec4( a_Position.x + a_Normal.x, a_Position.y + a_Normal.y, a_Position.z + a_Normal.z, 1.0 );
	vec4 localBinormal = vec4( a_Position.x + a_Binormal.x, a_Position.y + a_Binormal.y, a_Position.z + a_Binormal.z, 1.0 );
	vec4 localTangent = vec4( a_Position.x + a_Tangent.x, a_Position.y + a_Tangent.y, a_Position.z + a_Tangent.z, 1.0 );


	localPosition = modelMatrix * localPosition;
	localNormal = modelMatrix * localNormal;
	localBinormal = modelMatrix * localBinormal;
	localTangent = modelMatrix * localTangent;

	localNormal = localPosition + normalize(localNormal - localPosition);
	localBinormal = localPosition + normalize(localBinormal - localPosition);
	localTangent = localPosition + normalize(localTangent - localPosition);

	gl_Position = ProjectionMatrix * localPosition;

	v_TexCoord.xy = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;

	v_Normal = ProjectionMatrix * localNormal;
	v_Binormal = ProjectionMatrix * localBinormal;
	v_Tangent = ProjectionMatrix * localTangent;
	v_Pos = gl_Position;

	v_Color = modelColor * a_Color;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
												R"(
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