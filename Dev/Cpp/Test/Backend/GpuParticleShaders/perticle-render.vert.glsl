#version 330

precision highp float;
precision highp int;

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}



float fadeIn(float duration, float age, float lifetime) {
	return clamp(age / duration, 0.0, 1.0);
}

float fadeOut(float duration, float age, float lifetime) {
	return clamp((lifetime - age) / duration, 0.0, 1.0);
}

float fadeInOut(float fadeinDuration, float fadeoutDuration, float age, float lifetime) {
	return fadeIn(fadeinDuration, age, lifetime) * fadeOut(fadeoutDuration, age, lifetime);
}


//layout(location = 0) in float a_ParticleIndex;
layout(location = 0) in vec2 a_VertexPosition;

out vec4 v_Color;

uniform sampler2D ParticleData0; // |   Pos X   |   Pos Y   |   Pos Z   |  Dir XYZ  |
uniform sampler2D ParticleData1; // | LifeCount | Lifetime  |   Index   |   Seed    |
uniform sampler2D ColorTable;
uniform vec4 ID2TPos;

uniform mat4 ViewMatrix;
uniform mat4 ProjMatrix;

vec2 rotate(vec2 pos, float deg)
{
	const float toRad = 3.141592 / 180.0;
	float c = cos(deg * toRad);
	float s = sin(deg * toRad);
	return mat2(c, -s, s, c) * pos;
}

void main() {
	//int particleID = int(a_ParticleIndex);
	int particleID = gl_InstanceID;
	ivec2 ID2TPos2i = ivec2(ID2TPos.x,ID2TPos.y);
	ivec2 texPos = ivec2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y);
	vec4 data0 = texelFetch(ParticleData0, texPos, 0);
	vec4 data1 = texelFetch(ParticleData1, texPos, 0);
	
	float age = data1.x;
	float lifetime = data1.y;

	if (age >= lifetime) {
		gl_Position = vec4(0.0);
		v_Color = vec4(0.0);
	} else {
		vec3 position = data0.xyz;
		position.xyz += vec3(rotate(a_VertexPosition * 0.003, 45.0), 0.0);
		gl_Position = ProjMatrix * ViewMatrix * vec4(position, 1.0);
		
		vec2 texCoord = vec2(snoise(vec2(texPos) / 512.0));
		v_Color = texture(ColorTable, texCoord);
		v_Color.a *= fadeInOut(1.0, 10.0, age, lifetime);
	}
}
