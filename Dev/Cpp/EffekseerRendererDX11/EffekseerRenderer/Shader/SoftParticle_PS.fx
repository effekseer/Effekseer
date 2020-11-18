#ifndef DISABLED_SOFT_PARTICLE

float SoftParticle(float backgroundZ, float meshZ, float softparticleParam, float2 reconstruct1, float4 reconstruct2)
{
	float distance = softparticleParam;
	float2 rescale = reconstruct1;
	float4 params = reconstruct2;

	#ifdef __OPENGL__
	float tempY = params.y;
	params.y = params.z;
	params.z = tempY;
	#endif

	float2 zs = float2(backgroundZ * rescale.x + rescale.y, meshZ);

	float2 depth = (zs * params.w - params.y) / (params.x - zs * params.z);

	return min(max((depth.y - depth.x) / distance, 0.0), 1.0);
}

#endif