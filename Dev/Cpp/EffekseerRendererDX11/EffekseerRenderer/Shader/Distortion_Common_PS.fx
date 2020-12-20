
cbuffer PS_ConstanBuffer : register(b0)
{
	float4 g_scale;
	float4 mUVInversedBack;

	float4 fFlipbookParameter; // x:enable, y:interpolationType

	float4 fUVDistortionParameter; // x:intensity, y:blendIntensity, zw:uvInversed

	float4 fBlendTextureParameter; // x:blendType

	// which is used for only softparticle
	float4 softParticleParam;
	float4 reconstructionParam1;
	float4 reconstructionParam2;
};