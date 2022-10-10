
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
#include "Parameter/CustomData.h"
#include "Parameter/DynamicParameter.h"
#include "Parameter/Easing.h"
#include "Parameter/Effekseer.Parameters.h"
#include "Parameter/Rotation.h"
#include "Parameter/Scaling.h"
#include "Parameter/SpawnMethod.h"
#include "Parameter/Translation.h"
#include "Parameter/UV.h"
#include "SIMD/Utils.h"
#include "Utils/BinaryVersion.h"

namespace Effekseer
{

enum class BindType : int32_t
{
	NotBind = 0,
	NotBind_Root = 3,
	WhenCreating = 1,
	Always = 2,
};

enum class NonMatchingLODBehaviour : int32_t
{
	Hide = 0,
	DontSpawn = 1,
	DontSpawnAndHide = 2
};

enum class TranslationParentBindType : int32_t
{
	NotBind = 0,
	NotBind_Root = 3,
	WhenCreating = 1,
	Always = 2,
	NotBind_FollowParent = 4,
	WhenCreating_FollowParent = 5,
};

bool operator==(const TranslationParentBindType& lhs, const BindType& rhs);

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

struct ParameterCommonValues_8
{
	int MaxGeneration;
	BindType TranslationBindType;
	BindType RotationBindType;
	BindType ScalingBindType;
	int RemoveWhenLifeIsExtinct;
	int RemoveWhenParentIsRemoved;
	int RemoveWhenChildrenIsExtinct;
	random_int life;
	float GenerationTime;
	float GenerationTimeOffset;
};

struct ParameterCommonValues
{
	int32_t RefEqMaxGeneration = -1;
	RefMinMax RefEqLife;
	RefMinMax RefEqGenerationTime;
	RefMinMax RefEqGenerationTimeOffset;

	int MaxGeneration = 1;
	TranslationParentBindType TranslationBindType = TranslationParentBindType::Always;
	BindType RotationBindType = BindType::Always;
	BindType ScalingBindType = BindType::Always;
	int RemoveWhenLifeIsExtinct = 1;
	int RemoveWhenParentIsRemoved = 0;
	int RemoveWhenChildrenIsExtinct = 0;
	random_int life;
	random_float GenerationTime;
	random_float GenerationTimeOffset;

	ParameterCommonValues()
	{
		life.max = 1;
		life.min = 1;
		GenerationTime.max = 1;
		GenerationTime.min = 1;
		GenerationTimeOffset.max = 0;
		GenerationTimeOffset.min = 0;
	}
};

struct ParameterLODs
{
	int MatchingLODs = 0b1111;
	NonMatchingLODBehaviour LODBehaviour = NonMatchingLODBehaviour::Hide;
};

enum class KillType : int32_t
{
	None = 0,
	Box = 1,
	Plane = 2,
	Sphere = 3
};

struct KillRulesParameter
{

	KillType Type = KillType::None;
	int IsScaleAndRotationApplied = 1;

	union
	{
		struct
		{
			vector3d Center; // In local space
			vector3d Size;	 // In local space
			int IsKillInside;
		} Box;

		struct
		{
			vector3d PlaneAxis; // in local space
			float PlaneOffset;	// in the direction of plane axis
		} Plane;

		struct
		{
			vector3d Center; // in local space
			float Radius;
			int IsKillInside;
		} Sphere;
	};

	void MakeCoordinateSystemLH()
	{
		if (Type == KillType::Box)
		{
			Box.Center.z *= -1.0F;
		}
		else if (Type == KillType::Plane)
		{
			Plane.PlaneAxis.z *= -1.0F;
		}
		else if (Type == KillType::Sphere)
		{
			Sphere.Center.z *= -1.0F;
		}
	}
};

struct ParameterDepthValues
{
	float DepthOffset;
	bool IsDepthOffsetScaledWithCamera;
	bool IsDepthOffsetScaledWithParticleScale;
	ZSortType ZSort;
	int32_t DrawingPriority;
	float SoftParticle;

	NodeRendererDepthParameter DepthParameter;

	ParameterDepthValues()
	{
		DepthOffset = 0;
		IsDepthOffsetScaledWithCamera = false;
		IsDepthOffsetScaledWithParticleScale = false;
		ZSort = ZSortType::None;
		DrawingPriority = 0;
		SoftParticle = 0.0f;
	}
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

enum class LocationAbsType : int32_t
{
	None = 0,
	Gravity = 1,
	AttractiveForce = 2,
};

struct LocationAbsParameter
{
	LocationAbsType type = LocationAbsType::None;

	union
	{
		struct
		{

		} none;

		SIMD::Vec3f gravity;

		struct
		{
			float force;
			float control;
			float minRange;
			float maxRange;
		} attractiveForce;
	};
};

struct ParameterRendererCommon
{
	static const int32_t UVParameterNum = 6;

	RendererMaterialType MaterialType = RendererMaterialType::Default;

	//! texture index except a file
	int32_t ColorTextureIndex = -1;

	//! texture index except a file
	int32_t Texture2Index = -1;

	//! texture index except a file
	int32_t AlphaTextureIndex = -1;

	//! texture index except a file
	int32_t UVDistortionTextureIndex = -1;

	//! texture index except a file
	int32_t BlendTextureIndex = -1;

	//! texture index except a file
	int32_t BlendAlphaTextureIndex = -1;

	//! texture index except a file
	int32_t BlendUVDistortionTextureIndex = -1;

	//! material index in MaterialType::File
	MaterialRenderData MaterialData;

	AlphaBlendType AlphaBlend = AlphaBlendType::Opacity;

	std::array<TextureFilterType, TextureSlotMax> FilterTypes;
	std::array<TextureWrapType, TextureSlotMax> WrapTypes;

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
		FilterTypes.fill(TextureFilterType::Nearest);
		WrapTypes.fill(TextureWrapType::Repeat);
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
				memcpy(&ColorTextureIndex, pos, sizeof(int));
				pos += sizeof(int);

				memcpy(&Texture2Index, pos, sizeof(int));
				pos += sizeof(int);

				if (version >= 1600)
				{
					memcpy(&AlphaTextureIndex, pos, sizeof(int));
					pos += sizeof(int);

					memcpy(&UVDistortionTextureIndex, pos, sizeof(int));
					pos += sizeof(int);

					memcpy(&BlendTextureIndex, pos, sizeof(int));
					pos += sizeof(int);

					memcpy(&BlendAlphaTextureIndex, pos, sizeof(int));
					pos += sizeof(int);

					memcpy(&BlendUVDistortionTextureIndex, pos, sizeof(int));
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
			memcpy(&ColorTextureIndex, pos, sizeof(int));
			pos += sizeof(int);
		}

		memcpy(&AlphaBlend, pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&FilterTypes[0], pos, sizeof(int));
		pos += sizeof(int);

		memcpy(&WrapTypes[0], pos, sizeof(int));
		pos += sizeof(int);

		if (version >= 15)
		{
			memcpy(&FilterTypes[1], pos, sizeof(int));
			pos += sizeof(int);

			memcpy(&WrapTypes[1], pos, sizeof(int));
			pos += sizeof(int);
		}
		else
		{
			FilterTypes[1] = FilterTypes[0];
			WrapTypes[1] = WrapTypes[0];
		}

		if (version >= 1600)
		{
			for (size_t i = 2; i < 7; i++)
			{
				memcpy(&FilterTypes[i], pos, sizeof(int));
				pos += sizeof(int);

				memcpy(&WrapTypes[i], pos, sizeof(int));
				pos += sizeof(int);
			}
		}
		else
		{
			for (size_t i = 2; i < 7; i++)
			{
				FilterTypes[i] = FilterTypes[0];
				WrapTypes[i] = WrapTypes[0];
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
		BasicParameter.TextureFilters = FilterTypes;
		BasicParameter.TextureWraps = WrapTypes;

		BasicParameter.DistortionIntensity = DistortionIntensity;
		BasicParameter.MaterialType = MaterialType;
		BasicParameter.TextureIndexes[0] = ColorTextureIndex;
		BasicParameter.TextureIndexes[1] = Texture2Index;
		BasicParameter.TextureIndexes[2] = AlphaTextureIndex;
		BasicParameter.TextureIndexes[3] = UVDistortionTextureIndex;
		BasicParameter.TextureIndexes[4] = BlendTextureIndex;
		BasicParameter.TextureIndexes[5] = BlendAlphaTextureIndex;
		BasicParameter.TextureIndexes[6] = BlendUVDistortionTextureIndex;

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
enum ParameterSoundType
{
	ParameterSoundType_None = 0,
	ParameterSoundType_Use = 1,

	ParameterSoundType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
enum ParameterSoundPanType
{
	ParameterSoundPanType_2D = 0,
	ParameterSoundPanType_3D = 1,

	ParameterSoundPanType_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ParameterSound
{
	int32_t WaveId;
	random_float Volume;
	random_float Pitch;
	ParameterSoundPanType PanType;
	random_float Pan;
	float Distance;
	random_int Delay;
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

	ParameterSoundType SoundType = ParameterSoundType_None;
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
	virtual eEffectNodeType GetType() const
	{
		return eEffectNodeType::NoneType;
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