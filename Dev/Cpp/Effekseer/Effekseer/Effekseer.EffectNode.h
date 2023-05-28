
#ifndef __EFFEKSEER_EFFECTNODE_H__
#define __EFFEKSEER_EFFECTNODE_H__

#include "Effekseer.Base.h"
#include "Effekseer.Color.h"
#include "Effekseer.FCurves.h"
#include "Effekseer.InternalStruct.h"
#include "Effekseer.RectF.h"
#include "Effekseer.Vector3D.h"
#include "Sound/Effekseer.SoundPlayer.h"

#include "Effekseer.Effect.h"
#include "ForceField/ForceFields.h"
#include "Noise/CurlNoise.h"
#include "Parameter/AllTypeColor.h"
#include "Parameter/AlphaCutoff.h"
#include "Parameter/BasicSettings.h"
#include "Parameter/CustomData.h"
#include "Parameter/DepthParameter.h"
#include "Parameter/DynamicParameter.h"
#include "Parameter/Easing.h"
#include "Parameter/Effekseer.Parameters.h"
#include "Parameter/KillRules.h"
#include "Parameter/LOD.h"
#include "Parameter/Rotation.h"
#include "Parameter/Scaling.h"
#include "Parameter/Sound.h"
#include "Parameter/SpawnMethod.h"
#include "Parameter/Translation.h"
#include "Parameter/UV.h"
#include "SIMD/Utils.h"
#include "Utils/BinaryVersion.h"

namespace Effekseer
{

enum class TriggerType : uint8_t
{
	None = 0,
	ExternalTrigger = 1,
};

struct alignas(2) TriggerValues
{
	TriggerType type = TriggerType::None;
	uint8_t index = 0;
};

struct SteeringBehaviorParameter
{
	random_float MaxFollowSpeed;
	random_float SteeringSpeed;
};

struct TriggerParameter
{
	TriggerValues ToStartGeneration;
	TriggerValues ToStopGeneration;
	TriggerValues ToRemove;
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

	void load(uint8_t*& pos, int32_t version)
	{
		// memset(this, 0, sizeof(ParameterRendererCommon));

		if (version >= 15)
		{
			memcpy(&MaterialType, pos, sizeof(int));
			pos += sizeof(int);

			Distortion = MaterialType == RendererMaterialType::BackDistortion;

			if (MaterialType == RendererMaterialType::Default || MaterialType == RendererMaterialType::Lighting)
			{
				if (version >= 1600)
				{
					memcpy(&EmissiveScaling, pos, sizeof(float));
					pos += sizeof(float);
				}
				else
				{
					EmissiveScaling = 1.0f;
				}
			}

			if (MaterialType == RendererMaterialType::Default || MaterialType == RendererMaterialType::BackDistortion ||
				MaterialType == RendererMaterialType::Lighting)
			{
				memcpy(&TextureIndexes[static_cast<size_t>(RendererTextureType::Color)], pos, sizeof(int));
				pos += sizeof(int);

				memcpy(&TextureIndexes[static_cast<size_t>(RendererTextureType::Normal)], pos, sizeof(int));
				pos += sizeof(int);

				if (version >= 1600)
				{
					memcpy(&TextureIndexes[static_cast<size_t>(RendererTextureType::Alpha)], pos, sizeof(int));
					pos += sizeof(int);

					memcpy(&TextureIndexes[static_cast<size_t>(RendererTextureType::UVDistortion)], pos, sizeof(int));
					pos += sizeof(int);

					memcpy(&TextureIndexes[static_cast<size_t>(RendererTextureType::Blend)], pos, sizeof(int));
					pos += sizeof(int);

					memcpy(&TextureIndexes[static_cast<size_t>(RendererTextureType::BlendAlpha)], pos, sizeof(int));
					pos += sizeof(int);

					memcpy(&TextureIndexes[static_cast<size_t>(RendererTextureType::BlendUVDistortion)], pos, sizeof(int));
					pos += sizeof(int);
				}
			}
			else
			{
				memcpy(&MaterialData.MaterialIndex, pos, sizeof(int));
				pos += sizeof(int);

				int32_t textures = 0;
				int32_t uniforms = 0;

				memcpy(&textures, pos, sizeof(int));
				pos += sizeof(int);

				MaterialData.MaterialTextures.resize(textures);
				if (MaterialData.MaterialTextures.size() > 0)
				{
					memcpy(MaterialData.MaterialTextures.data(), pos, sizeof(MaterialTextureParameter) * textures);
				}
				pos += (sizeof(MaterialTextureParameter) * textures);

				memcpy(&uniforms, pos, sizeof(int));
				pos += sizeof(int);

				MaterialData.MaterialUniforms.resize(uniforms);
				if (MaterialData.MaterialUniforms.size() > 0)
				{
					memcpy(MaterialData.MaterialUniforms.data(), pos, sizeof(float) * 4 * uniforms);
				}
				pos += (sizeof(float) * 4 * uniforms);

				if (version >= Version17Alpha4)
				{
					int gradients = 0;
					memcpy(&gradients, pos, sizeof(int));
					pos += sizeof(int);

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
			memcpy(&TextureIndexes[static_cast<size_t>(RendererTextureType::Color)], pos, sizeof(int));
			pos += sizeof(int);
		}

		memcpy(&AlphaBlend, pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&TextureFilters[0], pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&TextureWraps[0], pos, sizeof(int));
		pos += sizeof(int);

		if (version >= 15)
		{
			memcpy(&TextureFilters[1], pos, sizeof(int));
			pos += sizeof(int);

			memcpy(&TextureWraps[1], pos, sizeof(int));
			pos += sizeof(int);
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
				memcpy(&TextureFilters[i], pos, sizeof(int));
				pos += sizeof(int);

				memcpy(&TextureWraps[i], pos, sizeof(int));
				pos += sizeof(int);
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

			memcpy(&ztest, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			memcpy(&zwrite, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			ZWrite = zwrite != 0;
			ZTest = ztest != 0;
		}
		else
		{
			ZWrite = false;
			ZTest = true;
		}

		memcpy(&FadeInType, pos, sizeof(int));
		pos += sizeof(int);

		if (FadeInType != FADEIN_OFF)
		{
			memcpy(&FadeIn, pos, sizeof(FadeIn));
			pos += sizeof(FadeIn);
		}

		memcpy(&FadeOutType, pos, sizeof(int));
		pos += sizeof(int);

		if (FadeOutType != FADEOUT_NONE)
		{
			memcpy(&FadeOut, pos, sizeof(FadeOut));
			pos += sizeof(FadeOut);
		}

		UVs[0].Load(pos, version, 0);

		if (version >= 1600)
		{
			for (int i = 1; i < 3; i++)
			{
				UVs[i].Load(pos, version, i);
			}

			memcpy(&UVDistortionIntensity, pos, sizeof(float));
			pos += sizeof(float);

			UVs[3].Load(pos, version, 3);

			memcpy(&TextureBlendType, pos, sizeof(int));
			pos += sizeof(int);

			for (int i = 4; i < 6; i++)
			{
				UVs[i].Load(pos, version, i);
			}

			// blend uv distortion intensity
			memcpy(&BlendUVDistortionIntensity, pos, sizeof(float));
			pos += sizeof(float);
		}

		if (version >= 10)
		{
			memcpy(&ColorBindType, pos, sizeof(int32_t));
			pos += sizeof(int32_t);
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

				memcpy(&distortion, pos, sizeof(int32_t));
				pos += sizeof(int32_t);

				Distortion = distortion > 0;

				if (Distortion)
				{
					MaterialType = RendererMaterialType::BackDistortion;
				}
			}

			memcpy(&DistortionIntensity, pos, sizeof(float));
			pos += sizeof(float);
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

	RefPtr<RenderingUserData> renderingUserData_;

	EffectNodeImplemented(Effect* effect, unsigned char*& pos);

	virtual ~EffectNodeImplemented();

	void AdjustSettings(const SettingRef& setting);

	void LoadParameter(unsigned char*& pos, EffectNode* parent, const SettingRef& setting);

	//! calculate custom data
	void CalcCustomData(const Instance* instance, std::array<float, 4>& customData1, std::array<float, 4>& customData2);

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

	eRenderingOrder RenderingOrder = RenderingOrder_FirstCreatedInstanceIsFirst;

	int32_t RenderingPriority = -1;

	DynamicFactorParameter DynamicFactor;

	bool Traverse(const std::function<bool(EffectNodeImplemented*)>& visitor);

	Effect* GetEffect() const override;

	int GetGeneration() const override;

	int GetChildrenCount() const override;

	EffectNode* GetChild(int index) const override;

	EffectBasicRenderParameter GetBasicRenderParameter() const override;

	void SetBasicRenderParameter(EffectBasicRenderParameter param) override;

	EffectModelParameter GetEffectModelParameter() override;

	virtual void LoadRendererParameter(unsigned char*& pos, const SettingRef& setting);

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
	static EffectNodeImplemented* Create(Effect* effect, EffectNode* parent, unsigned char*& pos);

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