
#ifndef __EFFEKSEER_EFFECTNODE_H__
#define __EFFEKSEER_EFFECTNODE_H__

#include "Utils/Effekseer.BinaryReader.h"
#include "Effekseer.Base.h"
#include "Effekseer.Color.h"
#include "Effekseer.FCurves.h"
#include "Effekseer.InternalStruct.h"
#include "Effekseer.RectF.h"
#include "Effekseer.Vector3D.h"
#include "Sound/Effekseer.SoundPlayer.h"

#include "Effekseer.Effect.h"
#include "ForceField/Effekseer.ForceFields.h"
#include "Noise/Effekseer.CurlNoise.h"
#include "Parameter/Effekseer.AllTypeColor.h"
#include "Parameter/Effekseer.AlphaCutoff.h"
#include "Parameter/Effekseer.BasicSettings.h"
#include "Parameter/Effekseer.Collisions.h"
#include "Parameter/Effekseer.CustomData.h"
#include "Parameter/Effekseer.DepthParameter.h"
#include "Parameter/Effekseer.DynamicParameter.h"
#include "Parameter/Effekseer.Easing.h"
#include "Parameter/Effekseer.KillRules.h"
#include "Parameter/Effekseer.LOD.h"
#include "Parameter/Effekseer.Parameters.h"
#include "Parameter/Effekseer.Rotation.h"
#include "Parameter/Effekseer.Scaling.h"
#include "Parameter/Effekseer.Sound.h"
#include "Parameter/Effekseer.SpawnMethod.h"
#include "Parameter/Effekseer.Translation.h"
#include "Parameter/Effekseer.Trigger.h"
#include "Parameter/Effekseer.UV.h"
#include "Renderer/Effekseer.GpuParticles.h"
#include "SIMD/Utils.h"
#include "Utils/Effekseer.BinaryVersion.h"

namespace Effekseer
{

struct SteeringBehaviorParameter
{
	random_float MaxFollowSpeed;
	random_float SteeringSpeed;
};

struct ParameterRendererCommon
{
	static const int32_t UVParameterNum = 6;

	RendererMaterialType MaterialType = RendererMaterialType::Default;

	//! texture index except a MaterialType::File
	std::array<int32_t, TextureSlotMax> TextureIndexes{-1, -1, -1, -1, -1, -1, -1, -1};
	std::array<TextureFilterType, TextureSlotMax> TextureFilters{};
	std::array<TextureWrapType, TextureSlotMax> TextureWraps{};

	//! material index in MaterialType::File
	MaterialRenderData MaterialData;

	AlphaBlendType AlphaBlend = AlphaBlendType::Opacity;

	float UVDistortionIntensity = 1.0f;

	int32_t TextureBlendType = -1;

	float BlendUVDistortionIntensity = 1.0f;

	int32_t UVHorizontalFlipProbability = 0;

	float EmissiveScaling = 1.0f;

	bool ZWrite = false;

	bool ZTest = false;

	//! this value is not unused
	bool Distortion = false;

	float DistortionIntensity = 0;

	BindType ColorBindType = BindType::NotBind;

	//! pass into a renderer (to make easy to send parameters, it should be refactored)
	NodeRendererBasicParameter BasicParameter;

	ParameterCustomData CustomData1;
	ParameterCustomData CustomData2;

	enum
	{
		FADEIN_OFF = 0,
		FADEIN_ON = 1,

		FADEIN_DWORD = 0x7fffffff,
	} FadeInType;

	struct
	{
		float Frame;
		easing_float_without_random Value;
	} FadeIn;

	enum
	{
		FADEOUT_NONE = 0,
		FADEOUT_WITHIN_LIFETIME = 1,
		FADEOUT_AFTER_REMOVED = 2,

		FADEOUT_DWORD = 0x7fffffff,
	} FadeOutType;

	struct
	{
		float Frame;
		easing_float_without_random Value;
	} FadeOut;

	std::array<UVParameter, UVParameterNum> UVs;

	ParameterRendererCommon()
	{
		FadeInType = FADEIN_OFF;
		FadeOutType = FADEOUT_NONE;
	}

	void reset()
	{
		// with constructor
		// memset(this, 0, sizeof(ParameterRendererCommon));
	}

	void load(BinaryReader<true>& pos, int32_t version)
	{
		// memset(this, 0, sizeof(ParameterRendererCommon));

		if (version >= 15)
		{
			if (!pos.Peek(&MaterialType, sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));

			Distortion = MaterialType == RendererMaterialType::BackDistortion;

			if (MaterialType == RendererMaterialType::Default || MaterialType == RendererMaterialType::Lighting)
			{
				if (version >= 1600)
				{
					if (!pos.Peek(&EmissiveScaling, sizeof(float)))
					{
						return pos.MarkFailed();
					}
					pos.Skip(sizeof(float));
				}
				else
				{
					EmissiveScaling = 1.0f;
				}
			}

			if (MaterialType == RendererMaterialType::Default || MaterialType == RendererMaterialType::BackDistortion ||
				MaterialType == RendererMaterialType::Lighting)
			{
				if (!pos.Peek(&TextureIndexes[static_cast<size_t>(RendererTextureType::Color)], sizeof(int)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int));

				if (!pos.Peek(&TextureIndexes[static_cast<size_t>(RendererTextureType::Normal)], sizeof(int)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int));

				if (version >= 1600)
				{
					if (!pos.Peek(&TextureIndexes[static_cast<size_t>(RendererTextureType::Alpha)], sizeof(int)))
					{
						return pos.MarkFailed();
					}
					pos.Skip(sizeof(int));

					if (!pos.Peek(&TextureIndexes[static_cast<size_t>(RendererTextureType::UVDistortion)], sizeof(int)))
					{
						return pos.MarkFailed();
					}
					pos.Skip(sizeof(int));

					if (!pos.Peek(&TextureIndexes[static_cast<size_t>(RendererTextureType::Blend)], sizeof(int)))
					{
						return pos.MarkFailed();
					}
					pos.Skip(sizeof(int));

					if (!pos.Peek(&TextureIndexes[static_cast<size_t>(RendererTextureType::BlendAlpha)], sizeof(int)))
					{
						return pos.MarkFailed();
					}
					pos.Skip(sizeof(int));

					if (!pos.Peek(&TextureIndexes[static_cast<size_t>(RendererTextureType::BlendUVDistortion)], sizeof(int)))
					{
						return pos.MarkFailed();
					}
					pos.Skip(sizeof(int));
				}
			}
			else
			{
				if (!pos.Peek(&MaterialData.MaterialIndex, sizeof(int)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int));

				int32_t textures = 0;
				int32_t uniforms = 0;

				if (!pos.Peek(&textures, sizeof(int)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int));
				if (textures < 0 || textures > 1024)
					return pos.MarkFailed();

				if (!pos.Read(MaterialData.MaterialTextures, textures))
				{
					return pos.MarkFailed();
				}

				if (!pos.Peek(&uniforms, sizeof(int)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int));
				if (uniforms < 0 || uniforms > 1024)
					return pos.MarkFailed();

				if (!pos.Read(MaterialData.MaterialUniforms, uniforms))
				{
					return pos.MarkFailed();
				}

				if (version >= Version17Alpha4)
				{
					int gradients = 0;
					if (!pos.Peek(&gradients, sizeof(int)))
					{
						return pos.MarkFailed();
					}
					pos.Skip(sizeof(int));
					if (gradients < 0 || gradients > 1024)
						return pos.MarkFailed();

					MaterialData.MaterialGradients.resize(gradients);
					for (size_t i = 0; i < MaterialData.MaterialGradients.size(); i++)
					{
						MaterialData.MaterialGradients[i] = std::make_shared<Gradient>();
						LoadGradient(*MaterialData.MaterialGradients[i], pos, version);
					}
				}
			}
		}
		else
		{
			if (!pos.Peek(&TextureIndexes[static_cast<size_t>(RendererTextureType::Color)], sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));
		}

		if (!pos.Peek(&AlphaBlend, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));

		if (!pos.Peek(&TextureFilters[0], sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));

		if (!pos.Peek(&TextureWraps[0], sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));

		if (version >= 15)
		{
			if (!pos.Peek(&TextureFilters[1], sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));

			if (!pos.Peek(&TextureWraps[1], sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));
		}
		else
		{
			TextureFilters[1] = TextureFilters[0];
			TextureWraps[1] = TextureWraps[0];
		}

		if (version >= 1600)
		{
			for (size_t i = 2; i < 7; i++)
			{
				if (!pos.Peek(&TextureFilters[i], sizeof(int)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int));

				if (!pos.Peek(&TextureWraps[i], sizeof(int)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int));
			}
		}
		else
		{
			for (size_t i = 2; i < 7; i++)
			{
				TextureFilters[i] = TextureFilters[0];
				TextureWraps[i] = TextureWraps[0];
			}
		}

		if (version >= 5)
		{
			int32_t zwrite, ztest = 0;

			if (!pos.Peek(&ztest, sizeof(int32_t)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int32_t));

			if (!pos.Peek(&zwrite, sizeof(int32_t)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int32_t));

			ZWrite = zwrite != 0;
			ZTest = ztest != 0;
		}
		else
		{
			ZWrite = false;
			ZTest = true;
		}

		if (!pos.Peek(&FadeInType, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));

		if (FadeInType != FADEIN_OFF)
		{
			if (!pos.Peek(&FadeIn, sizeof(FadeIn)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(FadeIn));
		}

		if (!pos.Peek(&FadeOutType, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));

		if (FadeOutType != FADEOUT_NONE)
		{
			if (!pos.Peek(&FadeOut, sizeof(FadeOut)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(FadeOut));
		}

		UVs[0].Load(pos, version, 0);

		if (version >= 1600)
		{
			for (int i = 1; i < 3; i++)
			{
				UVs[i].Load(pos, version, i);
			}

			if (!pos.Peek(&UVDistortionIntensity, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));
			if (TextureIndexes[static_cast<size_t>(RendererTextureType::UVDistortion)] < 0)
			{
				UVDistortionIntensity = 0.0f;
			}

			UVs[3].Load(pos, version, 3);

			if (!pos.Peek(&TextureBlendType, sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));

			for (int i = 4; i < 6; i++)
			{
				UVs[i].Load(pos, version, i);
			}

			// blend uv distortion intensity
			if (!pos.Peek(&BlendUVDistortionIntensity, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));
			if (TextureIndexes[static_cast<size_t>(RendererTextureType::BlendUVDistortion)] < 0)
			{
				BlendUVDistortionIntensity = 0.0f;
			}
		}

		if (version >= Version18Alpha2)
		{
			if (!pos.Peek(&UVHorizontalFlipProbability, sizeof(int32_t)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int32_t));
		}
		else
		{
			UVHorizontalFlipProbability = 0;
		}

		if (version >= 10)
		{
			if (!pos.Peek(&ColorBindType, sizeof(int32_t)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int32_t));
		}
		else
		{
			ColorBindType = BindType::NotBind;
		}

		if (version >= 9)
		{
			if (version < 15)
			{
				int32_t distortion = 0;

				if (!pos.Peek(&distortion, sizeof(int32_t)))
				{
					return pos.MarkFailed();
				}
				pos.Skip(sizeof(int32_t));

				Distortion = distortion > 0;

				if (Distortion)
				{
					MaterialType = RendererMaterialType::BackDistortion;
				}
			}

			if (!pos.Peek(&DistortionIntensity, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));
		}

		if (version >= 15)
		{
			CustomData1.load(pos, version);
			CustomData2.load(pos, version);
		}

		// copy to basic parameter
		BasicParameter.AlphaBlend = AlphaBlend;
		BasicParameter.TextureFilters = TextureFilters;
		BasicParameter.TextureWraps = TextureWraps;

		BasicParameter.DistortionIntensity = DistortionIntensity;
		BasicParameter.MaterialType = MaterialType;
		BasicParameter.TextureIndexes = TextureIndexes;

		BasicParameter.UVDistortionIntensity = UVDistortionIntensity;

		BasicParameter.TextureBlendType = TextureBlendType;

		BasicParameter.BlendUVDistortionIntensity = BlendUVDistortionIntensity;

		if (UVs[0].Type == UVAnimationType::Animation)
		{
			BasicParameter.Flipbook = UVFunctions::ToFlipbookParameter(UVs[0]);
		}

		BasicParameter.EmissiveScaling = EmissiveScaling;

		if (BasicParameter.MaterialType == RendererMaterialType::File)
		{
			BasicParameter.MaterialRenderDataPtr = &MaterialData;
		}
		else
		{
			BasicParameter.MaterialRenderDataPtr = nullptr;
		}

		if (BasicParameter.MaterialType != RendererMaterialType::Lighting)
		{
			BasicParameter.TextureFilters[1] = TextureFilterType::Nearest;
			BasicParameter.TextureWraps[1] = TextureWrapType::Clamp;
		}
	}
};

struct TrailUVAnimationCache
{
	float InfiniteUVs[ParameterRendererCommon::UVParameterNum][4];
	bool IsInfiniteUVInitialized[ParameterRendererCommon::UVParameterNum];
	float InfiniteFlipbookIndexAndNextRate;
	bool IsInfiniteFlipbookIndexAndNextRateInitialized;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum eRenderingOrder
{
	RenderingOrder_FirstCreatedInstanceIsFirst = 0,
	RenderingOrder_FirstCreatedInstanceIsLast = 1,

	RenderingOrder_DWORD = 0x7fffffff,
};

/**
@brief	ノードインスタンス生成クラス
@note
エフェクトのノードの実体を生成する。
*/
class EffectNodeImplemented : public EffectNode, public SIMD::AlignedAllocationPolicy<16>
{
	friend class Manager;
	friend class EffectImplemented;
	friend class Instance;

protected:
	// 所属しているパラメーター
	Effect* m_effect;

	//! a generation in the node tree
	int generation_ = 0;

	// 子ノード
	std::vector<EffectNodeImplemented*> m_Nodes;
	bool isLoadingValid_ = true;

	RefPtr<RenderingUserData> renderingUserData_;

	EffectNodeImplemented(Effect* effect, BinaryReader<true>& pos);

	virtual ~EffectNodeImplemented();

	void AdjustSettings(const SettingRef& setting);

	void LoadParameter(BinaryReader<true>& pos, EffectNode* parent, const SettingRef& setting);

	//! calculate custom data
	void CalcCustomData(const Instance* instance, std::array<float, 4>& customData1, std::array<float, 4>& customData2);

	void ApplyRendererCommonUVHorizontalFlip(Instance& instance, IRandObject& rand) const;

	static void InitializeTrailUVAnimationCache(TrailUVAnimationCache& cache);

	static RectF GetTrailUV(TrailUVAnimationCache& cache,
							Instance* groupFirst,
							const ParameterRendererCommon& rendererCommon,
							int32_t index,
							float livingTime,
							float livedTime);

	static float GetTrailFlipbookIndexAndNextRate(TrailUVAnimationCache& cache,
												  Instance* groupFirst,
												  const ParameterRendererCommon& rendererCommon);

public:
	/**
	@brief	\~english Whether to draw the node.
	\~japanese このノードを描画するか?

	@note
	\~english
	普通は描画されないノードは、描画の種類が変更されて、描画しないノードになる。ただし、色の継承をする場合、描画のみを行わないノードになる。
	\~japanese For nodes that are not normally rendered, the rendering type is changed to become a node that does not render. However, when
	color inheritance is done, it becomes a node which does not perform drawing only.
	*/
	bool IsRendered = true;

	ParameterCommonValues CommonValues;
	SteeringBehaviorParameter SteeringBehaviorParam;
	TriggerParameter TriggerParam;
	ParameterLODs LODsParam;
	KillRulesParameter KillParam;

	TranslationParameter TranslationParam;

	LocalForceFieldParameter LocalForceField;

	RotationParameter RotationParam;

	ScalingParameter ScalingParam;

	ParameterGenerationLocation GenerationLocation;

	ParameterDepthValues DepthValues;

	ParameterRendererCommon RendererCommon;

	ParameterAlphaCutoff AlphaCutoff;

	bool EnableFalloff = false;
	FalloffParameter FalloffParam{};

	ParameterSound Sound;

	GpuParticles::ResourceRef GpuParticlesResource;

	eRenderingOrder RenderingOrder = RenderingOrder_FirstCreatedInstanceIsFirst;

	int32_t RenderingPriority = -1;

	DynamicFactorParameter DynamicFactor;

	CollisionsParameter Collisions;

	bool Traverse(const std::function<bool(EffectNodeImplemented*)>& visitor);

	Effect* GetEffect() const override;

	int GetGeneration() const override;

	int GetChildrenCount() const override;

	EffectNode* GetChild(int index) const override;

	EffectBasicRenderParameter GetBasicRenderParameter() const override;

	void SetBasicRenderParameter(EffectBasicRenderParameter param) override;

	EffectModelParameter GetEffectModelParameter() override;

	virtual void LoadRendererParameter(BinaryReader<true>& pos, const SettingRef& setting);

	virtual void BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData);

	/**
	@brief	グループ描画開始
	*/
	virtual void BeginRenderingGroup(InstanceGroup* group, Manager* manager, void* userData);

	virtual void EndRenderingGroup(InstanceGroup* group, Manager* manager, void* userData);

	virtual void Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData);

	/**
	@brief	描画終了
	*/
	virtual void EndRendering(Manager* manager, void* userData);

	/**
	@brief	インスタンスグループ描画時初期化
	*/
	virtual void InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager);

	/**
	@brief	描画部分初期化
	*/
	virtual void InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager);

	/**
	@brief	描画部分更新
	*/
	virtual void UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager);

	float GetFadeAlpha(const Instance& instance) const;

	EffectInstanceTerm CalculateInstanceTerm(EffectInstanceTerm& parentTerm) const override;

	/**
	@brief	エフェクトノード生成
	*/
	static EffectNodeImplemented* Create(Effect* effect, EffectNode* parent, BinaryReader<true>& pos);

	/**
	@brief	ノードの種類取得
	*/
	virtual EffectNodeType GetType() const override
	{
		return EffectNodeType::NoneType;
	}

	RefPtr<RenderingUserData> GetRenderingUserData() override
	{
		return renderingUserData_;
	}

	bool CanDrawWithNonMatchingLOD() const
	{
		return LODsParam.LODBehaviour != NonMatchingLODBehaviour::Hide && LODsParam.LODBehaviour != NonMatchingLODBehaviour::DontSpawnAndHide;
	}

	bool CanSpawnWithNonMatchingLOD() const
	{
		return LODsParam.LODBehaviour != NonMatchingLODBehaviour::DontSpawn && LODsParam.LODBehaviour != NonMatchingLODBehaviour::DontSpawnAndHide;
	}

	void SetRenderingUserData(const RefPtr<RenderingUserData>& renderingUserData) override
	{
		renderingUserData_ = renderingUserData;
	}

	bool IsParticleSpawnedWithDecimal() const
	{
		return m_effect->GetVersion() >= Version17Alpha6;
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_EFFECTNODE_H__
