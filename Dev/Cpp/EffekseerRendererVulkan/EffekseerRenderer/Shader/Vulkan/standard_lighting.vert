#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 atPosition;
layout(location = 1) in vec4 atColor;
layout(location = 2) in vec3 atNormal;
layout(location = 3) in vec3 atTangent;
layout(location = 4) in vec2 atTexCoord;
layout(location = 5) in vec2 atTexCoord2;

layout(location = 0) centroid out vec4 v_VColor;
layout(location = 1) centroid out vec2 v_UV1;
layout(location = 2) out vec2 v_UV2;
layout(location = 3) out vec3 v_WorldP;
layout(location = 4) out vec3 v_WorldN;
layout(location = 5) out vec3 v_WorldT;
layout(location = 6) out vec3 v_WorldB;
layout(location = 7) out vec2 v_ScreenUV;

layout(set = 0, binding = 0) uniform Block 
{
	mat4 uMatCamera;
	mat4 uMatProjection;
	vec4 mUVInversed;
};

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

	gl_Position.y = - gl_Position.y;

}