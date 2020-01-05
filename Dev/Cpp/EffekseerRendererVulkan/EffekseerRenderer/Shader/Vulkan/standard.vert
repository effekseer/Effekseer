#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 atPosition;
layout(location = 1) in vec4 atColor;
layout(location = 2) in vec4 atTexCoord;

layout(location = 0) out vec4 vaColor;
layout(location = 1) out vec4 vaTexCoord;
layout(location = 2) out vec4 vaPos;
layout(location = 3) out vec4 vaPosR;
layout(location = 4) out vec4 vaPosU;

layout(set = 0, binding = 0) uniform Block 
{
	mat4 uMatCamera;
	mat4 uMatProjection;
	vec4 mUVInversed;
};

void main() {
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
}