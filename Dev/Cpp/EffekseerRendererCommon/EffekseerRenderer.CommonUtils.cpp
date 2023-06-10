

#include "EffekseerRenderer.CommonUtils.h"

namespace EffekseerRenderer
{

std::array<std::array<float, 4>, 13> ToUniform(const Effekseer::Gradient& gradient)
{
	std::array<std::array<float, 4>, 13> ret;
	ret[0][0] = gradient.ColorCount;
	ret[0][1] = gradient.AlphaCount;
	ret[0][2] = 0.0F;
	ret[0][3] = 0.0F;

	const auto getColorKey = [](const Effekseer::Gradient& gradient, size_t index)
	{
		if (gradient.ColorCount == 0)
		{
			Effekseer::Gradient::ColorKey key;
			key.Color = {1.0f, 1.0f, 1.0f};
			key.Intensity = 1.0f;
			key.Position = 0.0;
			return key;
		}
		else
		{
			if (gradient.ColorCount <= index)
			{
				auto key = gradient.Colors[gradient.ColorCount - 1];
				key.Position += index;
				return key;
			}

			return gradient.Colors[index];
		}
	};

	const auto getAlphaKey = [](const Effekseer::Gradient& gradient, size_t index)
	{
		if (gradient.AlphaCount == 0)
		{
			Effekseer::Gradient::AlphaKey key;
			key.Alpha = 1.0f;
			key.Position = 0.0;
			return key;
		}
		else
		{
			if (gradient.AlphaCount <= index)
			{
				auto key = gradient.Alphas[gradient.AlphaCount - 1];
				key.Position += index;
				return key;
			}

			return gradient.Alphas[index];
		}
	};

	for (size_t i = 0; i < gradient.Colors.size(); i++)
	{
		const auto colorKey = getColorKey(gradient, i);
		ret[1 + i][0] = colorKey.Color[0] * colorKey.Intensity;
		ret[1 + i][1] = colorKey.Color[1] * colorKey.Intensity;
		ret[1 + i][2] = colorKey.Color[2] * colorKey.Intensity;
		ret[1 + i][3] = colorKey.Position;
	}

	for (size_t i = 0; i < 4; i++)
	{
		const auto alphaKey0 = getAlphaKey(gradient, i * 2 + 0);
		const auto alphaKey1 = getAlphaKey(gradient, i * 2 + 1);
		ret[9 + i][0] = alphaKey0.Alpha;
		ret[9 + i][1] = alphaKey0.Position;
		ret[9 + i][2] = alphaKey1.Alpha;
		ret[9 + i][3] = alphaKey1.Position;
	}

	return ret;
}

void CalcBillboard(::Effekseer::BillboardType billboardType,
				   Effekseer::SIMD::Mat43f& dst,
				   ::Effekseer::SIMD::Vec3f& s,
				   ::Effekseer::SIMD::Vec3f& R,
				   ::Effekseer::SIMD::Vec3f& F,
				   const ::Effekseer::SIMD::Mat43f& src,
				   const ::Effekseer::SIMD::Vec3f& frontDirection)
{
	auto frontDir = frontDirection;

	if (billboardType == ::Effekseer::BillboardType::Billboard || billboardType == ::Effekseer::BillboardType::RotatedBillboard ||
		billboardType == ::Effekseer::BillboardType::YAxisFixed)
	{
		::Effekseer::SIMD::Mat43f r;
		::Effekseer::SIMD::Vec3f t;
		src.GetSRT(s, r, t);

		::Effekseer::SIMD::Vec3f U;

		if (billboardType == ::Effekseer::BillboardType::Billboard)
		{
			::Effekseer::SIMD::Vec3f Up(0.0f, 1.0f, 0.0f);

			F = frontDir;
			R = ::Effekseer::SIMD::Vec3f::Cross(Up, F).Normalize();
			U = ::Effekseer::SIMD::Vec3f::Cross(F, R).Normalize();
		}
		else if (billboardType == ::Effekseer::BillboardType::RotatedBillboard)
		{
			::Effekseer::SIMD::Vec3f Up(0.0f, 1.0f, 0.0f);

			F = frontDir;
			R = ::Effekseer::SIMD::Vec3f::Cross(Up, F).Normalize();
			U = ::Effekseer::SIMD::Vec3f::Cross(F, R).Normalize();

			float c_zx2 = Effekseer::SIMD::Vec3f::Dot(r.Y, r.Y) - r.Y.GetZ() * r.Y.GetZ();
			float c_zx = sqrt(std::max(0.0f, c_zx2));
			float s_z = 0.0f;
			float c_z = 0.0f;

			if (fabsf(c_zx) > 0.001f)
			{
				s_z = r.Y.GetX() / c_zx;
				c_z = r.Y.GetY() / c_zx;
			}
			else
			{
				s_z = 0.0f;
				c_z = 1.0f;
			}

			::Effekseer::SIMD::Vec3f r_temp = R;
			::Effekseer::SIMD::Vec3f u_temp = U;

			R = r_temp * c_z + u_temp * s_z;
			U = u_temp * c_z - r_temp * s_z;
		}
		else if (billboardType == ::Effekseer::BillboardType::YAxisFixed)
		{
			U = ::Effekseer::SIMD::Vec3f(r.X.GetY(), r.Y.GetY(), r.Z.GetY());
			F = frontDir;
			R = ::Effekseer::SIMD::Vec3f::Cross(U, F).Normalize();
			F = ::Effekseer::SIMD::Vec3f::Cross(R, U).Normalize();
		}

		dst.X = {R.GetX(), U.GetX(), F.GetX(), t.GetX()};
		dst.Y = {R.GetY(), U.GetY(), F.GetY(), t.GetY()};
		dst.Z = {R.GetZ(), U.GetZ(), F.GetZ(), t.GetZ()};
	}
}

static void FastScale(::Effekseer::SIMD::Mat43f& mat, float scale)
{
	float x = mat.X.GetW();
	float y = mat.Y.GetW();
	float z = mat.Z.GetW();

	mat.X *= scale;
	mat.Y *= scale;
	mat.Z *= scale;

	mat.X.SetW(x);
	mat.Y.SetW(y);
	mat.Z.SetW(z);
}

static void FastScale(::Effekseer::SIMD::Mat44f& mat, float scale)
{
	float x = mat.X.GetW();
	float y = mat.Y.GetW();
	float z = mat.Z.GetW();

	mat.X *= scale;
	mat.Y *= scale;
	mat.Z *= scale;

	mat.X.SetW(x);
	mat.Y.SetW(y);
	mat.Z.SetW(z);
}

void ApplyDepthParameters(::Effekseer::SIMD::Mat43f& mat,
						  const ::Effekseer::SIMD::Vec3f& cameraFront,
						  const ::Effekseer::SIMD::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scales = mat.GetScale();
			auto scale = (scales.GetX() + scales.GetY() + scales.GetZ()) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();
		// auto cl = cameraFront.X * cx + cameraFront.Y * cy * cameraFront.Z * cz;

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

void ApplyDepthParameters(::Effekseer::SIMD::Mat43f& mat,
						  ::Effekseer::SIMD::Vec3f& translationValues,
						  ::Effekseer::SIMD::Vec3f& scaleValues,
						  const ::Effekseer::SIMD::Vec3f& cameraFront,
						  const ::Effekseer::SIMD::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scale = (scaleValues.GetX() + scaleValues.GetY() + scaleValues.GetZ()) / 3.0f;
			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto c = translationValues - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;

				scaleValues *= scale;
			}
		}

		auto objPos = translationValues;
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			translationValues += dir * offset;
		}
		else
		{
			translationValues += dir * offset;
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto cam2t = translationValues - cameraPos;
		auto cl = cam2t.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			for (auto r = 0; r < 3; r++)
			{
				for (auto c = 0; c < 3; c++)
				{
					scaleValues *= scale;
				}
			}
		}
	}
}

void ApplyDepthParameters(::Effekseer::SIMD::Mat43f& mat,
						  const ::Effekseer::SIMD::Vec3f& cameraFront,
						  const ::Effekseer::SIMD::Vec3f& cameraPos,
						  ::Effekseer::SIMD::Vec3f& scaleValues,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scale = (scaleValues.GetX() + scaleValues.GetY() + scaleValues.GetZ()) / 3.0f;
			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

void ApplyDepthParameters(::Effekseer::SIMD::Mat44f& mat,
						  const ::Effekseer::SIMD::Vec3f& cameraFront,
						  const ::Effekseer::SIMD::Vec3f& cameraPos,
						  ::Effekseer::NodeRendererDepthParameter* depthParameter,
						  bool isRightHand)
{
	auto depthOffset = depthParameter->DepthOffset;
	auto isDepthOffsetScaledWithCamera = depthParameter->IsDepthOffsetScaledWithCamera;
	auto isDepthOffsetScaledWithEffect = depthParameter->IsDepthOffsetScaledWithParticleScale;

	if (depthOffset != 0)
	{
		auto offset = depthOffset;

		if (isDepthOffsetScaledWithEffect)
		{
			auto scales = mat.GetScale();
			auto scale = (scales.GetX() + scales.GetY() + scales.GetZ()) / 3.0f;

			offset *= scale;
		}

		if (isDepthOffsetScaledWithCamera)
		{
			auto t = mat.GetTranslation();
			auto c = t - cameraPos;
			auto cl = c.GetLength();

			if (cl != 0.0)
			{
				auto scale = (cl - offset) / cl;
				FastScale(mat, scale);
			}
		}

		auto objPos = mat.GetTranslation();
		auto dir = cameraPos - objPos;
		dir = dir.Normalize();

		if (isRightHand)
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
		else
		{
			mat.SetTranslation(mat.GetTranslation() + dir * offset);
		}
	}

	if (depthParameter->SuppressionOfScalingByDepth < 1.0f)
	{
		auto t = mat.GetTranslation();
		auto c = t - cameraPos;
		auto cl = c.GetLength();

		if (cl != 0.0)
		{
			auto scale = cl / 32.0f * (1.0f - depthParameter->SuppressionOfScalingByDepth) + depthParameter->SuppressionOfScalingByDepth;
			FastScale(mat, scale);
		}
	}
}

void ApplyViewOffset(::Effekseer::SIMD::Mat43f& mat,
					 const ::Effekseer::SIMD::Mat44f& camera,
					 float distance)
{
	::Effekseer::Matrix44 cameraMat;
	::Effekseer::Matrix44::Inverse(cameraMat, ToStruct(camera));

	::Effekseer::SIMD::Vec3f ViewOffset = ::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[3]) + -::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[2]) * distance;

	::Effekseer::SIMD::Vec3f localPos = mat.GetTranslation();
	ViewOffset += (::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[0]) * localPos.GetX() +
				   ::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[1]) * localPos.GetY() +
				   -::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[2]) * localPos.GetZ());

	mat.SetTranslation(ViewOffset);
}

void ApplyViewOffset(::Effekseer::SIMD::Mat44f& mat,
					 const ::Effekseer::SIMD::Mat44f& camera,
					 float distance)
{
	::Effekseer::Matrix44 cameraMat;
	::Effekseer::Matrix44::Inverse(cameraMat, ToStruct(camera));

	::Effekseer::SIMD::Vec3f ViewOffset = ::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[3]) + -::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[2]) * distance;

	::Effekseer::SIMD::Vec3f localPos = mat.GetTranslation();
	ViewOffset += (::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[0]) * localPos.GetX() +
				   ::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[1]) * localPos.GetY() +
				   -::Effekseer::SIMD::Vec3f::Load(cameraMat.Values[2]) * localPos.GetZ());

	mat.SetTranslation(ViewOffset);
}

void CalculateAlignedTextureInformation(Effekseer::Backend::TextureFormatType format, const std::array<int, 2>& size, int32_t& sizePerWidth, int32_t& height)
{
	sizePerWidth = 0;
	height = 0;

	const int32_t blockSize = 4;
	auto aligned = [](int32_t size, int32_t alignement) -> int32_t
	{
		return ((size + alignement - 1) / alignement) * alignement;
	};

	if (format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM)
	{
		sizePerWidth = 4 * size[0];
		height = size[1];
	}
	else if (format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
		sizePerWidth = 4 * size[0];
		height = size[1];
	}
	else if (format == Effekseer::Backend::TextureFormatType::R8_UNORM)
	{
		sizePerWidth = 1 * size[0];
		height = size[1];
	}
	else if (format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
	{
		sizePerWidth = sizeof(float) * 2 * size[0];
		height = size[1];
	}
	else if (format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
	{
		sizePerWidth = sizeof(float) * 4 / 2 * size[0];
		height = size[1];
	}
	else if (format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
	{
		sizePerWidth = sizeof(float) * 4 * size[0];
		height = size[1];
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC1)
	{
		sizePerWidth = 8 * aligned(size[0], blockSize) / blockSize;
		height = aligned(size[1], blockSize) / blockSize;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC2)
	{
		sizePerWidth = 16 * aligned(size[0], blockSize) / blockSize;
		height = aligned(size[1], blockSize) / blockSize;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC3)
	{
		sizePerWidth = 16 * aligned(size[0], blockSize) / blockSize;
		height = aligned(size[1], blockSize) / blockSize;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
	{
		sizePerWidth = 8 * aligned(size[0], blockSize) / blockSize;
		height = aligned(size[1], blockSize) / blockSize;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
	{
		sizePerWidth = 16 * aligned(size[0], blockSize) / blockSize;
		height = aligned(size[1], blockSize) / blockSize;
	}
	else if (format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
	{
		sizePerWidth = 16 * aligned(size[0], blockSize) / blockSize;
		height = aligned(size[1], blockSize) / blockSize;
	}
}

Effekseer::Backend::VertexLayoutRef GetVertexLayout(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RendererShaderType type)
{
	if (type == RendererShaderType::Unlit)
	{
		const Effekseer::Backend::VertexLayoutElement vlElemSprite[3] = {
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Pos", "POSITION", 0},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Color", "NORMAL", 0},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV", "TEXCOORD", 0},
		};

		return graphicsDevice->CreateVertexLayout(vlElemSprite, 3);
	}
	else if (type == RendererShaderType::AdvancedUnlit)
	{
		const Effekseer::Backend::VertexLayoutElement vlElemUnlitAd[8] = {
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Pos", "POSITION", 0},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Color", "NORMAL", 0},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV", "TEXCOORD", 0},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT, "Input_Alpha_Dist_UV", "TEXCOORD", 1},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_BlendUV", "TEXCOORD", 2},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT, "Input_Blend_Alpha_Dist_UV", "TEXCOORD", 3},
			{Effekseer::Backend::VertexLayoutFormat::R32_FLOAT, "Input_FlipbookIndex", "TEXCOORD", 4},
			{Effekseer::Backend::VertexLayoutFormat::R32_FLOAT, "Input_AlphaThreshold", "TEXCOORD", 5},
		};

		return graphicsDevice->CreateVertexLayout(vlElemUnlitAd, 8);
	}
	else if (type == RendererShaderType::Lit || type == RendererShaderType::BackDistortion)
	{
		const Effekseer::Backend::VertexLayoutElement vlElemLit[6] = {
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Pos", "POSITION", 0},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Color", "NORMAL", 0},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Normal", "NORMAL", 1},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Tangent", "NORMAL", 2},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV1", "TEXCOORD", 0},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV2", "TEXCOORD", 1},
		};

		return graphicsDevice->CreateVertexLayout(vlElemLit, 6);
	}
	else if (type == RendererShaderType::AdvancedLit || type == RendererShaderType::AdvancedBackDistortion)
	{
		const Effekseer::Backend::VertexLayoutElement vlElemLitAd[11] = {
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Pos", "POSITION", 0},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Color", "NORMAL", 0},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Normal", "NORMAL", 1},
			{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Tangent", "NORMAL", 2},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV1", "TEXCOORD", 0},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV2", "TEXCOORD", 1},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT, "Input_Alpha_Dist_UV", "TEXCOORD", 2},
			{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_BlendUV", "TEXCOORD", 3},
			{Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT, "Input_Blend_Alpha_Dist_UV", "TEXCOORD", 4},
			{Effekseer::Backend::VertexLayoutFormat::R32_FLOAT, "Input_FlipbookIndex", "TEXCOORD", 5},
			{Effekseer::Backend::VertexLayoutFormat::R32_FLOAT, "Input_AlphaThreshold", "TEXCOORD", 6},
		};

		return graphicsDevice->CreateVertexLayout(vlElemLitAd, 11);
	}

	assert(0);

	return {};
}

Effekseer::Backend::VertexLayoutRef GetModelRendererVertexLayout(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	const Effekseer::Backend::VertexLayoutElement vlElem[6] = {
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Pos", "POSITION", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Normal", "NORMAL", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Binormal", "NORMAL", 1},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "Input_Tangent", "NORMAL", 2},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "Input_UV", "TEXCOORD", 0},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "Input_Color", "NORMAL", 3},
	};

	return graphicsDevice->CreateVertexLayout(vlElem, 6);
}

Effekseer::Backend::VertexLayoutRef GetMaterialSimpleVertexLayout(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	const Effekseer::Backend::VertexLayoutElement vlElem[3] = {
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
	};

	return graphicsDevice->CreateVertexLayout(vlElem, 3);
}

Effekseer::Backend::VertexLayoutRef GetMaterialSpriteVertexLayout(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t customData1, int32_t customData2)
{
	Effekseer::Backend::VertexLayoutElement vlElem[8] = {
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "atPosition", "POSITION", 0},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atColor", "NORMAL", 0},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atNormal", "NORMAL", 1},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "atTangent", "NORMAL", 2},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord", "TEXCOORD", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "atTexCoord2", "TEXCOORD", 1},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "", "TEXCOORD", 2},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "", "TEXCOORD", 3},
	};

	auto getFormat = [](int32_t i) -> Effekseer::Backend::VertexLayoutFormat
	{
		if (i == 1)
			return Effekseer::Backend::VertexLayoutFormat::R32_FLOAT;
		if (i == 2)
			return Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT;
		if (i == 3)
			return Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT;
		if (i == 4)
			return Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT;

		assert(0);
		return Effekseer::Backend::VertexLayoutFormat::R32_FLOAT;
	};

	int32_t offset = 40;
	int count = 6;
	int semanticIndex = 2;
	const char* customData1Name = "atCustomData1";
	const char* customData2Name = "atCustomData2";

	if (customData1 > 0)
	{
		vlElem[count].Name = customData1Name;
		vlElem[count].Format = getFormat(customData1);
		vlElem[count].SemanticIndex = semanticIndex;
		semanticIndex++;

		count++;
		offset += sizeof(float) * customData1;
	}

	if (customData2 > 0)
	{
		vlElem[count].Name = customData2Name;
		vlElem[count].Format = getFormat(customData2);
		vlElem[count].SemanticIndex = semanticIndex;
		semanticIndex++;

		count++;
		offset += sizeof(float) * customData2;
	}

	return graphicsDevice->CreateVertexLayout(vlElem, count);
}

Effekseer::Backend::VertexLayoutRef GetMaterialModelVertexLayout(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	const Effekseer::Backend::VertexLayoutElement vlElem[6] = {
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Position", "POSITION", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Normal", "NORMAL", 0},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Binormal", "NORMAL", 1},
		{Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT, "a_Tangent", "NORMAL", 2},
		{Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT, "a_TexCoord", "TEXCOORD", 0},
		{Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM, "a_Color", "NORMAL", 3},
	};

	return graphicsDevice->CreateVertexLayout(vlElem, 6);
}

bool DirtiedBlock::Allocate(int32_t size, int32_t offset)
{
	bool dirtied = false;
	int32_t connected = -1;

	for (size_t i = 0; i < blocks_.size(); i++)
	{
		const auto& block = blocks_[i];
		if ((block.offset <= offset && offset < block.offset + block.size) ||
			(block.offset < offset + size && offset + size <= block.offset + block.size) ||
			(offset <= block.offset && block.offset < offset + size) ||
			(offset < block.offset + block.size && block.offset + block.size <= offset + size))
		{
			dirtied = true;
			break;
		}

		if (offset == block.size + block.offset)
		{
			connected = static_cast<int32_t>(i);
		}
	}

	if (dirtied)
	{
		blocks_.clear();

		Block block;
		block.offset = offset;
		block.size = size;
		blocks_.emplace_back(block);
	}
	else
	{
		if (connected != -1)
		{
			blocks_[connected].size += size;
		}
		else
		{
			Block block;
			block.offset = offset;
			block.size = size;
			blocks_.emplace_back(block);
		}
	}

	return dirtied;
}

} // namespace EffekseerRenderer
