static const char g_sprite_vs_src[] =
	R"(
IN vec4 atPosition;
IN vec4 atColor;
IN vec4 atTexCoord;
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
void main()
{
	vec4 cameraPos = uMatCamera * atPosition;
	cameraPos = cameraPos / cameraPos.w;

	gl_Position = uMatProjection * cameraPos;

	vaPos = gl_Position;

	vec4 cameraPosU = cameraPos + vec4(0.0, 1.0, 0.0, 0.0);
	vec4 cameraPosR = cameraPos + vec4(1.0, 0.0, 0.0, 0.0);

	vaPosR = uMatProjection * cameraPosR;
	vaPosU = uMatProjection * cameraPosU;
	
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