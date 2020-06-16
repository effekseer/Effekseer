
static const char g_sprite_distortion_vs_src[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
IN vec4 atBinormal;
IN vec4 atTangent;

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
OUT vec4 vaColor;
OUT vec4 vaTexCoord;
OUT vec4 vaPos;
OUT vec4 vaPosR;
OUT vec4 vaPosU;
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

	vec4 localBinormal = vec4( atPosition.x + atBinormal.x, atPosition.y + atBinormal.y, atPosition.z + atBinormal.z, 1.0 );
	vec4 localTangent = vec4( atPosition.x + atTangent.x, atPosition.y + atTangent.y, atPosition.z + atTangent.z, 1.0 );
	localBinormal = uMatCamera * localBinormal;
	localTangent = uMatCamera * localTangent;

	vec4 cameraPos = uMatCamera * atPosition;
	cameraPos = cameraPos / cameraPos.w;

	localBinormal = localBinormal / localBinormal.w;
	localTangent = localTangent / localTangent.w;

	localBinormal = cameraPos + normalize(localBinormal - cameraPos);
	localTangent = cameraPos + normalize(localTangent - cameraPos);

	gl_Position = uMatProjection * cameraPos;

	vaPos = gl_Position;


	vaPosR = uMatProjection * localTangent;
	vaPosU = uMatProjection * localBinormal;
	
	vaPos = vaPos / vaPos.w;
	vaPosR = vaPosR / vaPosR.w;
	vaPosU = vaPosU / vaPosU.w;

	vaColor = atColor;
	vaTexCoord = atTexCoord;

	vaTexCoord.y = mUVInversed.x + mUVInversed.y * vaTexCoord.y;

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

	ApplyFlipbookVS(vaFlipbookRate, vaFlipbookNextIndexUV, mflipbookParameter, atFlipbookIndex, vaTexCoord.xy);
	vaAlphaThreshold = atAlphaThreshold;
)"
#endif

	R"(

}

)";