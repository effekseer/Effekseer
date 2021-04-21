#include "../../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"

#include "../TestHelper.h"

namespace EffekseerRenderer
{

} // namespace EffekseerRenderer

template <typename T>
void VertexTest(bool isValidValue)
{
	auto v = T();
	EffekseerRenderer::SetVertexAlphaUV(v, 1.0f, 0);
	EffekseerRenderer::SetVertexAlphaUV(v, 2.0f, 1);

	EffekseerRenderer::SetVertexUVDistortionUV(v, 3.0f, 0);
	EffekseerRenderer::SetVertexUVDistortionUV(v, 4.0f, 1);

	EffekseerRenderer::SetVertexBlendUV(v, 5.0f, 0);
	EffekseerRenderer::SetVertexBlendUV(v, 6.0f, 1);

	EffekseerRenderer::SetVertexBlendAlphaUV(v, 7.0f, 0);
	EffekseerRenderer::SetVertexBlendAlphaUV(v, 8.0f, 1);

	EffekseerRenderer::SetVertexBlendUVDistortionUV(v, 9.0f, 0);
	EffekseerRenderer::SetVertexBlendUVDistortionUV(v, 10.0f, 1);

	EffekseerRenderer::SetVertexFlipbookIndexAndNextRate(v, 11.0f);

	EffekseerRenderer::SetVertexAlphaThreshold(v, 12.0f);

	auto alphaUV = EffekseerRenderer::GetVertexAlphaUV(v);
	auto uvdistUV = EffekseerRenderer::GetVertexUVDistortionUV(v);

	auto blendUV = EffekseerRenderer::GetVertexBlendUV(v);
	auto blendAlphaUV = EffekseerRenderer::GetVertexBlendAlphaUV(v);
	auto blendUVDistUV = EffekseerRenderer::GetVertexBlendUVDistortionUV(v);

	auto flip = EffekseerRenderer::GetVertexFlipbookIndexAndNextRate(v);

	auto alphaThreshold = EffekseerRenderer::GetVertexAlphaThreshold(v);

	auto getExpect = [&](float value) -> float {
		if (isValidValue)
		{
			return value;
		}
		return 0.0f;
	};

	EXPECT_TRUE(alphaUV[0] == getExpect(1.0f) && alphaUV[1] == getExpect(2.0f))
	EXPECT_TRUE(uvdistUV[0] == getExpect(3.0f) && uvdistUV[1] == getExpect(4.0f))
	EXPECT_TRUE(blendUV[0] == getExpect(5.0f) && blendUV[1] == getExpect(6.0f))
	EXPECT_TRUE(blendAlphaUV[0] == getExpect(7.0f) && blendAlphaUV[1] == getExpect(8.0f))
	EXPECT_TRUE(blendUVDistUV[0] == getExpect(9.0f) && blendUVDistUV[1] == getExpect(10.0f))
	EXPECT_TRUE(flip == getExpect(11.0f))
	EXPECT_TRUE(alphaThreshold == getExpect(12.0f))
}

void VertexTest()
{
	VertexTest<EffekseerRenderer::AdvancedSimpleVertex>(true);
	VertexTest<EffekseerRenderer::AdvancedLightingVertex>(true);
	//VertexTest<EffekseerRenderer::AdvancedVertexDistortion>(true);
	VertexTest<EffekseerRenderer::SimpleVertex>(false);
	VertexTest<EffekseerRenderer::LightingVertex>(false);
	//VertexTest<EffekseerRenderer::VertexDistortion>(false);
	VertexTest<EffekseerRenderer::DynamicVertex>(false);
}
TestRegister Runtime_VertexTest("Runtime.Vertex", []() -> void { VertexTest(); });
