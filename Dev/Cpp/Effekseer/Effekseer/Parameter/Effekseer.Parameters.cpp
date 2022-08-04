#include "Effekseer.Parameters.h"
#include "../Effekseer.EffectImplemented.h"
#include "../Effekseer.Instance.h"
#include "../Effekseer.InstanceGlobal.h"
#include "../Effekseer.InternalScript.h"
#include "DynamicParameter.h"
#include <algorithm>

namespace Effekseer
{

void LoadGradient(Gradient& gradient, uint8_t*& pos, int32_t version)
{
	BinaryReader<true> reader(pos, std::numeric_limits<int>::max());
	reader.Read(gradient.ColorCount);
	for (int i = 0; i < gradient.ColorCount; i++)
	{
		reader.Read(gradient.Colors[i]);
	}

	reader.Read(gradient.AlphaCount);
	for (int i = 0; i < gradient.AlphaCount; i++)
	{
		reader.Read(gradient.Alphas[i]);
	}

	pos += reader.GetOffset();
}

void NodeRendererTextureUVTypeParameter::Load(uint8_t*& pos, int32_t version)
{
	memcpy(&Type, pos, sizeof(int));
	pos += sizeof(int);

	if (Type == TextureUVType::Strech)
	{
	}
	else if (Type == TextureUVType::Tile)
	{
		memcpy(&TileEdgeHead, pos, sizeof(TileEdgeHead));
		pos += sizeof(TileEdgeHead);

		memcpy(&TileEdgeTail, pos, sizeof(TileEdgeTail));
		pos += sizeof(TileEdgeTail);

		memcpy(&TileLoopAreaBegin, pos, sizeof(TileLoopAreaBegin));
		pos += sizeof(TileLoopAreaBegin);

		memcpy(&TileLoopAreaEnd, pos, sizeof(TileLoopAreaEnd));
		pos += sizeof(TileLoopAreaEnd);
	}
}

template <typename T, typename U>
void ApplyEq_(T& dstParam, const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, int dpInd, const U& originalParam)
{
	static_assert(sizeof(T) == sizeof(U), "size is not mismatched");
	const int count = sizeof(T) / 4;

	EFK_ASSERT(e != nullptr);
	EFK_ASSERT(0 <= dpInd && dpInd < static_cast<int>(instg->dynamicEqResults.size()));

	auto dst = reinterpret_cast<float*>(&(dstParam));
	auto src = reinterpret_cast<const float*>(&(originalParam));

	auto eqresult = instg->dynamicEqResults[dpInd];
	std::array<float, 1> globals;
	globals[0] = instg->GetUpdatedFrame() / 60.0f;

	std::array<float, 5> locals;

	for (int i = 0; i < count; i++)
	{
		locals[i] = src[i];
	}

	for (int i = count; i < 4; i++)
	{
		locals[i] = 0.0f;
	}

	locals[4] = parrentInstance != nullptr ? parrentInstance->m_LivingTime / 60.0f : 0.0f;

	auto e_ = static_cast<const EffectImplemented*>(e);
	auto& dp = e_->GetDynamicEquation()[dpInd];

	if (dp.GetRunningPhase() == InternalScript::RunningPhaseType::Local)
	{
		eqresult = dp.Execute(instg->GetDynamicInputParameters(), globals, locals, RandCallback::Rand, RandCallback::RandSeed, rand);
	}

	for (int i = 0; i < count; i++)
	{
		dst[i] = eqresult[i];
	}
}

void ApplyEq(float& dstParam, const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, int dpInd, const float& originalParam)
{
	ApplyEq_(dstParam, e, instg, parrentInstance, rand, dpInd, originalParam);
}

template <typename S>
SIMD::Vec3f ApplyEq_(
	const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, const int& dpInd, const SIMD::Vec3f& originalParam, const S& scale, const S& scaleInv)
{
	SIMD::Vec3f param = originalParam;
	if (dpInd >= 0)
	{
		param *= SIMD::Vec3f(scaleInv[0], scaleInv[1], scaleInv[2]);

		ApplyEq_(param, e, instg, parrentInstance, rand, dpInd, param);

		param *= SIMD::Vec3f(scale[0], scale[1], scale[2]);
	}
	return param;
}

SIMD::Vec3f ApplyEq(
	const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, const int& dpInd, const SIMD::Vec3f& originalParam, const std::array<float, 3>& scale, const std::array<float, 3>& scaleInv)
{
	return ApplyEq_(e, instg, parrentInstance, rand, dpInd, originalParam, scale, scaleInv);
}

random_float ApplyEq(const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, const RefMinMax& dpInd, random_float originalParam)
{
	if (dpInd.Max >= 0)
	{
		ApplyEq_(originalParam.max, e, instg, parrentInstance, rand, dpInd.Max, originalParam.max);
	}

	if (dpInd.Min >= 0)
	{
		ApplyEq_(originalParam.min, e, instg, parrentInstance, rand, dpInd.Min, originalParam.min);
	}

	return originalParam;
}

template <typename S>
random_vector3d ApplyEq_(const Effect* e,
						 const InstanceGlobal* instg,
						 const Instance* parrentInstance,
						 IRandObject* rand,
						 const RefMinMax& dpInd,
						 random_vector3d originalParam,
						 const S& scale,
						 const S& scaleInv)
{
	if (dpInd.Max >= 0)
	{
		originalParam.max.x *= scaleInv[0];
		originalParam.max.y *= scaleInv[1];
		originalParam.max.z *= scaleInv[2];

		ApplyEq_(originalParam.max, e, instg, parrentInstance, rand, dpInd.Max, originalParam.max);

		originalParam.max.x *= scale[0];
		originalParam.max.y *= scale[1];
		originalParam.max.z *= scale[2];
	}

	if (dpInd.Min >= 0)
	{
		originalParam.min.x *= scaleInv[0];
		originalParam.min.y *= scaleInv[1];
		originalParam.min.z *= scaleInv[2];

		ApplyEq_(originalParam.min, e, instg, parrentInstance, rand, dpInd.Min, originalParam.min);

		originalParam.min.x *= scale[0];
		originalParam.min.y *= scale[1];
		originalParam.min.z *= scale[2];
	}

	return originalParam;
}

random_vector3d ApplyEq(const Effect* e,
						const InstanceGlobal* instg,
						const Instance* parrentInstance,
						IRandObject* rand,
						const RefMinMax& dpInd,
						random_vector3d originalParam,
						const std::array<float, 3>& scale,
						const std::array<float, 3>& scaleInv)
{
	return ApplyEq_(e,
					instg,
					parrentInstance,
					rand,
					dpInd,
					originalParam,
					scale,
					scaleInv);
}

random_int ApplyEq(const Effect* e, const InstanceGlobal* instg, const Instance* parrentInstance, IRandObject* rand, const RefMinMax& dpInd, random_int originalParam)
{
	if (dpInd.Max >= 0)
	{
		float value = static_cast<float>(originalParam.max);
		ApplyEq_(value, e, instg, parrentInstance, rand, dpInd.Max, value);
		originalParam.max = static_cast<int32_t>(value);
	}

	if (dpInd.Min >= 0)
	{
		float value = static_cast<float>(originalParam.min);
		ApplyEq_(value, e, instg, parrentInstance, rand, dpInd.Min, value);
		originalParam.min = static_cast<int32_t>(value);
	}

	return originalParam;
}

std::array<float, 4> Gradient::GetColor(float x) const
{
	const auto c = GetColorAndIntensity(x);
	return std::array<float, 4>{c[0] * c[3], c[1] * c[3], c[2] * c[3], GetAlpha(x)};
}

std::array<float, 4> Gradient::GetColorAndIntensity(float x) const
{
	if (ColorCount == 0)
	{
		return std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.0f};
	}

	if (x < Colors[0].Position)
	{
		const auto c = Colors[0].Color;
		return {c[0], c[1], c[2], Colors[0].Intensity};
	}

	if (Colors[ColorCount - 1].Position <= x)
	{
		const auto c = Colors[ColorCount - 1].Color;
		return {c[0], c[1], c[2], Colors[ColorCount - 1].Intensity};
	}

	auto key = ColorKey();
	key.Position = x;

	auto it = std::lower_bound(Colors.begin(), Colors.begin() + ColorCount, key, [](const ColorKey& a, const ColorKey& b) { return a.Position < b.Position; });
	auto ind = static_cast<int32_t>(std::distance(Colors.begin(), it));

	{
		if (Colors[ind].Position != x)
		{
			ind--;
		}

		if (Colors[ind].Position <= x && x <= Colors[ind + 1].Position)
		{
			const auto area = Colors[ind + 1].Position - Colors[ind].Position;
			if (area == 0)
			{
				return std::array<float, 4>{Colors[ind].Color[0], Colors[ind].Color[1], Colors[ind].Color[2], Colors[ind].Intensity};
			}

			const auto alpha = (x - Colors[ind].Position) / area;
			const auto r = Colors[ind + 1].Color[0] * alpha + Colors[ind].Color[0] * (1.0f - alpha);
			const auto g = Colors[ind + 1].Color[1] * alpha + Colors[ind].Color[1] * (1.0f - alpha);
			const auto b = Colors[ind + 1].Color[2] * alpha + Colors[ind].Color[2] * (1.0f - alpha);
			const auto intensity = Colors[ind + 1].Intensity * alpha + Colors[ind].Intensity * (1.0f - alpha);
			return std::array<float, 4>{r, g, b, intensity};
		}
		else
		{
			assert(0);
		}
	}

	return std::array<float, 4>{1.0f, 1.0f, 1.0f, 1.0f};
}

float Gradient::GetAlpha(float x) const
{
	if (AlphaCount == 0)
	{
		return 1.0f;
	}

	if (x < Alphas[0].Position)
	{
		return Alphas[0].Alpha;
	}

	if (Alphas[AlphaCount - 1].Position <= x)
	{
		return Alphas[AlphaCount - 1].Alpha;
	}

	auto key = AlphaKey();
	key.Position = x;

	auto it = std::lower_bound(Alphas.begin(), Alphas.begin() + AlphaCount, key, [](const AlphaKey& a, const AlphaKey& b) { return a.Position < b.Position; });
	auto ind = static_cast<int32_t>(std::distance(Alphas.begin(), it));

	{
		if (Alphas[ind].Position != x)
		{
			ind--;
		}

		if (Alphas[ind].Position <= x && x <= Alphas[ind + 1].Position)
		{
			const auto area = Alphas[ind + 1].Position - Alphas[ind].Position;
			if (area == 0)
			{
				return Alphas[ind].Alpha;
			}

			const auto alpha = (x - Alphas[ind].Position) / area;
			return Alphas[ind + 1].Alpha * alpha + Alphas[ind].Alpha * (1.0f - alpha);
		}
		else
		{
			assert(0);
		}
	}

	return 1.0f;
}

} // namespace Effekseer