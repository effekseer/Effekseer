#version 450
#extension GL_ARB_separate_shader_objects : enable

#define TextureEnable true
#define LightingEnable true
#define NormalMapEnable true

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Normal;
layout(location = 2) in vec4 a_Binormal;
layout(location = 3) in vec4 a_Tangent;
layout(location = 4) in vec4 a_TexCoord;
layout(location = 5) in vec4 a_Color;
layout(location = 6) in uvec4 a_InstanceID;

#if defined(MODEL_SOFTWARE_INSTANCING)
layout(location = 6) in float a_InstanceID;
layout(location = 7) in vec4 a_UVOffset;
layout(location = 8) in vec4 a_ModelColor;
#endif

layout(location = 0) out vec4 v_Normal;
layout(location = 1) out vec4 v_Binormal;
layout(location = 2) out vec4 v_Tangent;
layout(location = 3) out vec4 v_TexCoord;
layout(location = 4) out vec4 v_Color;
layout(location = 5) out vec4 v_Pos;

layout(set = 0, binding = 0) uniform Block 
{
	uniform mat4 ProjectionMatrix;

#if defined(MODEL_SOFTWARE_INSTANCING)

	uniform mat4 ModelMatrix[20];
	uniform vec4 UVOffset[20];
	uniform vec4 ModelColor[20];

#else

	uniform mat4 ModelMatrix;
	uniform vec4 UVOffset;
	uniform vec4 ModelColor;

#endif

	uniform vec4 LightDirection;
	uniform vec4 LightColor;
	uniform vec4 LightAmbient;
	uniform vec4 mUVInversed;
};


void main() {
#if defined(MODEL_SOFTWARE_INSTANCING)
    mat4 modelMatrix = ModelMatrix[int(a_InstanceID.x)];
    vec4 uvOffset = a_UVOffset;
    vec4 modelColor = a_ModelColor;
#else
    mat4 modelMatrix = ModelMatrix;
    vec4 uvOffset = UVOffset;
    vec4 modelColor = ModelColor;
#endif

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

	v_TexCoord.y = mUVInversed.x + mUVInversed.y * v_TexCoord.y;
}
