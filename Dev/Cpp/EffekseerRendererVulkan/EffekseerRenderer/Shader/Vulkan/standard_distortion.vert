#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 atPosition;
layout(location = 1) in vec4 atColor;
layout(location = 2) in vec4 atTexCoord;
layout(location = 3) in vec4 atBinormal;
layout(location = 4) in vec4 atTangent;

layout(location = 0) out vec4 vaColor;
layout(location = 1) out vec4 vaTexCoord;
layout(location = 2) out vec4 vaPos;
layout(location = 3) out vec4 vaPosR;
layout(location = 4) out vec4 vaPosU;

layout(set = 0, binding = 0) uniform Block 
{
	uniform mat4 uMatCamera;
	uniform mat4 uMatProjection;
	uniform vec4 mUVInversed;
};

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
}
