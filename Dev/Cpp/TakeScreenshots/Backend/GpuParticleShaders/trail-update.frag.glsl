#version 330

precision highp float;
precision highp int;

uniform sampler2D i_Position;
uniform sampler2D i_Velocity;

layout(location = 0) out vec4 o_Histories;  // xyz:position, w:velocity(packed-vec3)

float packVec3(vec3 v) {
	uvec3 i = uvec3((v + 1.0) * 0.5 * 1023.0);
	return uintBitsToFloat(i.x | (i.y << 10) | (i.z << 20));
}

void main() {
	// Load data
	vec4 position = texelFetch(i_Position, ivec2(gl_FragCoord.xy), 0);
	vec4 velocity = texelFetch(i_Velocity, ivec2(gl_FragCoord.xy), 0);
	
	// Store data
	o_Histories = vec4(position.xyz, packVec3(normalize(velocity.xyz)));
}
