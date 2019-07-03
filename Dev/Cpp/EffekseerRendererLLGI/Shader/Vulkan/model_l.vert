#version 450
#extension GL_ARB_separate_shader_objects : enable

#define TextureEnable false
#define LightingEnable true
#define NormalMapEnable false

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Normal;
layout(location = 2) in vec4 a_Binormal;
layout(location = 3) in vec4 a_Tangent;
layout(location = 4) in vec4 a_TexCoord;
layout(location = 5) in vec4 a_Color;
layout(location = 6) in vec4 a_InstanceID;

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


void main()
{

#if defined(MODEL_SOFTWARE_INSTANCING)
	mat4 modelMatrix = ModelMatrix[int(a_InstanceID)];
	vec4 uvOffset = a_UVOffset;
	vec4 modelColor = a_ModelColor;
#else
	mat4 modelMatrix = ModelMatrix;
	vec4 uvOffset = UVOffset;
	vec4 modelColor = ModelColor * a_Color;
#endif

	vec4 localPosition = modelMatrix * a_Position;
	gl_Position = ProjectionMatrix * localPosition;

	v_TexCoord.xy = a_TexCoord.xy * uvOffset.zw + uvOffset.xy;

	if (LightingEnable)
	{
		mat3 lightMatrix = mat3(modelMatrix[0].xyz, modelMatrix[1].xyz, modelMatrix[2].xyz);
		vec3 localNormal = normalize( lightMatrix * a_Normal.xyz );
		float diffuse = 1.0;
		if (NormalMapEnable)
		{
			v_Normal = vec4(localNormal, 1.0);
			v_Binormal = vec4(normalize( lightMatrix * a_Binormal.xyz ), 1.0);
			v_Tangent = vec4(normalize( lightMatrix * a_Tangent.xyz ), 1.0);
		}
		else
		{
			diffuse = max(0.0, dot(localNormal, LightDirection.xyz));
		}
		v_Color = modelColor * vec4(diffuse * LightColor.rgb, 1.0);
	}
	else
	{
		v_Color = modelColor;
	}
	v_Color = modelColor;

	v_TexCoord.y = mUVInversed.x + mUVInversed.y * v_TexCoord.y;
}
