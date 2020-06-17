static const char g_sprite_vs_lighting_src[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec3 atNormal;
IN vec3 atTangent;
IN vec2 atTexCoord;
IN vec2 atTexCoord2;
)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
IN vec2 atAlphaUV;
IN vec2 atUVDistortionUV;
IN float atFlipbookIndex;
IN float atAlphaThreshold;
)"
#endif

	R"(
OUT lowp vec4 v_VColor;
OUT mediump vec2 v_UV1;
OUT mediump vec2 v_UV2;
OUT mediump vec3 v_WorldP;
OUT mediump vec3 v_WorldN;
OUT mediump vec3 v_WorldT;
OUT mediump vec3 v_WorldB;
OUT mediump vec2 v_ScreenUV;
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

	R"(
uniform mat4 uMatCamera;
uniform mat4 uMatProjection;
uniform vec4 mUVInversed;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
uniform vec4 mflipbookParameter; // x:enable, y:loopType, z:divideX, w:divideY
)"
#endif

	R"(
void main() {
	vec3 worldPos = atPosition.xyz;

	// UV
	vec2 uv1 = atTexCoord.xy;
	uv1.y = mUVInversed.x + mUVInversed.y * uv1.y;
	vec2 uv2 = atTexCoord2.xy;
	uv2.y = mUVInversed.x + mUVInversed.y * uv2.y;

	// NBT
	vec3 worldNormal = (atNormal - vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 worldTangent = (atTangent - vec3(0.5, 0.5, 0.5)) * 2.0;
	vec3 worldBinormal = cross(worldNormal, worldTangent);

	v_WorldN = worldNormal;
	v_WorldB = worldBinormal;
	v_WorldT = worldTangent;
	vec3 pixelNormalDir = vec3(0.5, 0.5, 1.0);

	vec4 cameraPos = uMatCamera * vec4(worldPos, 1.0);
	cameraPos = cameraPos / cameraPos.w;

	gl_Position = uMatProjection * cameraPos;

	v_WorldP = worldPos;
	v_VColor = atColor;

	v_UV1 = uv1;
	v_UV2 = uv2;
	v_ScreenUV.xy = gl_Position.xy / gl_Position.w;
	v_ScreenUV.xy = vec2(v_ScreenUV.x + 1.0, v_ScreenUV.y + 1.0) * 0.5;

)"

#ifdef __EFFEKSEER_BUILD_VERSION16__
	R"(
	vaAlphaUV = atAlphaUV;
	vaAlphaUV.y = mUVInversed.x + mUVInversed.y * vaAlphaUV.y;

    // uv distortion texture
    vaUVDistortionUV = atUVDistortionUV;
    vaUVDistortionUV.y = mUVInversed.x + mUVInversed.y * vaUVDistortionUV.y;

	vaFlipbookRate = 0.0;
	vaFlipbookNextIndexUV = vec2(0.0, 0.0);

	ApplyFlipbookVS(vaFlipbookRate, vaFlipbookNextIndexUV, mflipbookParameter, atFlipbookIndex, v_UV1);
	vaAlphaThreshold = atAlphaThreshold;
)"
#endif

	R"(

}

)";