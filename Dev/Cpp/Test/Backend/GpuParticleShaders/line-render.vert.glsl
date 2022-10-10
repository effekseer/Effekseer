#version 330

precision highp float;
precision highp int;

#include "noise/noise2D.glsl"

out vec4 v_Color;

uniform highp sampler2DArray Histories;
uniform sampler2D Position;
uniform sampler2D Velocity;
uniform sampler2D ColorTable;
uniform ivec2 Trail;
uniform ivec2 ID2TPos;

uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

void main() {
	//int particleID = int(a_ParticleIndex);
	int particleID = gl_InstanceID;
	ivec2 texPos = ivec2(particleID & ID2TPos.x, particleID >> ID2TPos.y);
	vec4 position = texelFetch(Position, texPos, 0);
	vec4 velocity = texelFetch(Velocity, texPos, 0);
	
	int lineCount = int(position.w);
	int lineID = gl_VertexID;
	if (position.w >= velocity.w || lineID >= lineCount) {
		gl_Position = vec4(0.0);
		v_Color = vec4(0.0);
	} else {
		if (gl_VertexID > 0) {
			int texIndex = (Trail.x + gl_VertexID - 1) % Trail.y;
			position = texelFetch(Histories, ivec3(texPos, texIndex), 0);
			//position.xyz = vec3(0.0);
		}
		gl_Position = ProjMatrix * ViewMatrix * vec4(position.xyz, 1.0);
		
		vec2 texCoord = vec2(snoise(vec2(texPos) / 512.0));
		v_Color = texture(ColorTable, texCoord);
	}
}
