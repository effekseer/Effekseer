
float3 decodeOct(float2 oct)
{
	float3 v = float3(oct.xy, 1.0 - abs(oct.x) - abs(oct.y));
	float t = max(-v.z, 0.0);
	v.xy += t * -sign(v.xy);
	return normalize(v);
}

void decodeOct(float2 octNormal, float2 octTangent, out float3 normal, out float3 binormal, out float3 tangent)
{
	octNormal = octNormal * 2.0f - 1.0f;
	normal = decodeOct(octNormal);
	
	octTangent = octTangent * 2.0f - 1.0f;
	float s = sign(octTangent.y);
	octTangent = float2(octTangent.x, abs(octTangent.y) * 2.0 - 1.0);
	tangent = decodeOct(octTangent);

	binormal = cross(normal, tangent) * s;
}
