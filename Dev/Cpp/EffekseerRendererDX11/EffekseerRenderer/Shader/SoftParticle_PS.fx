#ifndef DISABLED_SOFT_PARTICLE

float SoftParticle(float backgroundZ, float meshZ, float4 softparticleParam, float4 reconstruct1, float4 reconstruct2)
{
	float distanceFar = softparticleParam.x;
	float distanceNear = softparticleParam.y;
	float distanceNearOffset = softparticleParam.z;

	float2 rescale = reconstruct1.xy;
	float4 params = reconstruct2;

	float2 zs = float2(backgroundZ * rescale.x + rescale.y, meshZ);

	float2 depth = (zs * params.w - params.y) / (params.x - zs * params.z);

	float alphaFar = (depth.y - depth.x) / distanceFar;
	float alphaNear = (-distanceNearOffset-depth.y) / distanceNear;
	return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

#endif