

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.EffectNode.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.Manager.h"

#include "Effekseer.Vector3D.h"
#include "SIMD/Utils.h"

#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"

#include "Effekseer.EffectNodeRibbon.h"
#include "Effekseer.EffectNodeRing.h"
#include "Effekseer.EffectNodeRoot.h"
#include "Effekseer.EffectNodeSprite.h"
#include "Effekseer.Resource.h"
#include "Effekseer.Setting.h"
#include "Sound/Effekseer.SoundPlayer.h"
#include "Utils/Effekseer.BinaryReader.h"

#include "Utils/Compatiblity.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNodeImplemented::EffectNodeImplemented(Effect* effect, unsigned char*& pos)
	: m_effect(effect)
{
}

void EffectNodeImplemented::AdjustSettings(const SettingRef& setting)
{
	auto ef = static_cast<EffectImplemented*>(m_effect);

	if (ef->IsDyanamicMagnificationValid())
	{
		KillParam.Magnify(m_effect->GetMaginification());
		TranslationParam.Magnify(m_effect->GetMaginification(), DynamicFactor);
	}

	if (ef->IsDyanamicMagnificationValid())
	{
		if (GenerationLocation.type == ParameterGenerationLocation::TYPE_POINT)
		{
			GenerationLocation.point.location.min *= m_effect->GetMaginification();
			GenerationLocation.point.location.max *= m_effect->GetMaginification();
		}
		else if (GenerationLocation.type == ParameterGenerationLocation::TYPE_LINE)
		{
			GenerationLocation.line.position_end.min *= m_effect->GetMaginification();
			GenerationLocation.line.position_end.max *= m_effect->GetMaginification();
			GenerationLocation.line.position_start.min *= m_effect->GetMaginification();
			GenerationLocation.line.position_start.max *= m_effect->GetMaginification();
			GenerationLocation.line.position_noize.min *= m_effect->GetMaginification();
			GenerationLocation.line.position_noize.max *= m_effect->GetMaginification();
		}
		else if (GenerationLocation.type == ParameterGenerationLocation::TYPE_SPHERE)
		{
			GenerationLocation.sphere.radius.min *= m_effect->GetMaginification();
			GenerationLocation.sphere.radius.max *= m_effect->GetMaginification();
		}
		else if (GenerationLocation.type == ParameterGenerationLocation::TYPE_CIRCLE)
		{
			GenerationLocation.circle.radius.min *= m_effect->GetMaginification();
			GenerationLocation.circle.radius.max *= m_effect->GetMaginification();
		}
	}

	{
		if (ef->IsDyanamicMagnificationValid())
		{
			DepthValues.DepthOffset *= m_effect->GetMaginification();
			DepthValues.SoftParticle *= m_effect->GetMaginification();

			if (DepthValues.DepthParameter.DepthClipping < FLT_MAX / 10)
			{
				DepthValues.DepthParameter.DepthClipping *= m_effect->GetMaginification();
			}
		}

		DepthValues.DepthParameter.DepthOffset = DepthValues.DepthOffset;
		DepthValues.DepthParameter.IsDepthOffsetScaledWithCamera = DepthValues.IsDepthOffsetScaledWithCamera;
		DepthValues.DepthParameter.IsDepthOffsetScaledWithParticleScale = DepthValues.IsDepthOffsetScaledWithParticleScale;
		DepthValues.DepthParameter.ZSort = DepthValues.ZSort;
	}

	// Convert right handle coordinate system into left handle coordinate system
	if (setting->GetCoordinateSystem() == CoordinateSystem::LH)
	{
		DynamicFactor.Tra[2] *= -1.0f;

		TranslationParam.MakeLeftCoordinate();

		// Rotation
		DynamicFactor.Rot[0] *= -1.0f;
		DynamicFactor.Rot[1] *= -1.0f;

		RotationParam.MakeCoordinateSystemLH();
		GenerationLocation.MakeCoordinateSystemLH();
		KillParam.MakeCoordinateSystemLH();
	}

	// generate inversed parameter
	for (size_t i = 0; i < DynamicFactor.Tra.size(); i++)
	{
		DynamicFactor.TraInv[i] = 1.0f / DynamicFactor.Tra[i];
	}

	for (size_t i = 0; i < DynamicFactor.Rot.size(); i++)
	{
		DynamicFactor.RotInv[i] = 1.0f / DynamicFactor.Rot[i];
	}

	for (size_t i = 0; i < DynamicFactor.Scale.size(); i++)
	{
		DynamicFactor.ScaleInv[i] = 1.0f / DynamicFactor.Scale[i];
	}

	// rescale intensity after 1.5
#ifndef __EFFEKSEER_FOR_UE4__ // Hack for EffekseerForUE4
	RendererCommon.BasicParameter.DistortionIntensity *= m_effect->GetMaginification();
	RendererCommon.DistortionIntensity *= m_effect->GetMaginification();
#endif // !__EFFEKSEER_FOR_UE4__
}

void EffectNodeImplemented::LoadParameter(unsigned char*& pos, EffectNode* parent, const SettingRef& setting)
{
	int size = 0;
	int node_type = 0;
	auto ef = (EffectImplemented*)m_effect;

	if (parent)
	{
		generation_ = parent->GetGeneration() + 1;
	}
	else
	{
		generation_ = 0;
	}

	memcpy(&node_type, pos, sizeof(int));
	pos += sizeof(int);

	if (node_type == -1)
	{
		CommonValues.MaxGeneration = 1;

		GenerationLocation.EffectsRotation = 0;
		GenerationLocation.type = ParameterGenerationLocation::TYPE_POINT;
		GenerationLocation.point.location.reset();

		RenderingPriority = -1;
	}
	else
	{
		if (m_effect->GetVersion() >= 10)
		{
			int32_t rendered = 0;
			memcpy(&rendered, pos, sizeof(int32_t));
			pos += sizeof(int32_t);

			IsRendered = rendered != 0;
		}

		// To render with priority, nodes are assigned a list.
		if (m_effect->GetVersion() >= 13)
		{
			memcpy(&RenderingPriority, pos, sizeof(int32_t));
			pos += sizeof(int32_t);
		}
		else
		{
			RenderingPriority = -1;
		}

		CommonValues.Load(pos, ef);

		if (ef->GetVersion() >= 1600)
		{
			if (CommonValues.TranslationBindType == TranslationParentBindType::NotBind_FollowParent ||
				CommonValues.TranslationBindType == TranslationParentBindType::WhenCreating_FollowParent)
			{
				memcpy(&SteeringBehaviorParam, pos, sizeof(SteeringBehaviorParameter));
				pos += sizeof(SteeringBehaviorParameter);
			}
		}

		if (ef->GetVersion() >= Version17Alpha1)
		{
			uint8_t flags = 0;
			memcpy(&flags, pos, sizeof(uint8_t));
			pos += sizeof(uint8_t);

			if (flags & (1 << 0))
			{
				memcpy(&TriggerParam.ToStartGeneration, pos, sizeof(TriggerValues));
				pos += sizeof(TriggerValues);
			}
			if (flags & (1 << 1))
			{
				memcpy(&TriggerParam.ToStopGeneration, pos, sizeof(TriggerValues));
				pos += sizeof(TriggerValues);
			}
			if (flags & (1 << 2))
			{
				memcpy(&TriggerParam.ToRemove, pos, sizeof(TriggerValues));
				pos += sizeof(TriggerValues);
			}
		}

		LODsParam.Load(pos, ef->GetVersion());
		TranslationParam.Load(pos, ef->GetVersion());
		LocalForceField.Load(pos, ef->GetVersion());
		RotationParam.Load(pos, ef->GetVersion());
		ScalingParam.Load(pos, ef->GetVersion());
		GenerationLocation.load(pos, ef->GetVersion());
		DepthValues.Load(pos, ef->GetVersion());
		KillParam.Load(pos, ef->GetVersion());

		if (m_effect->GetVersion() >= 3)
		{
			RendererCommon.load(pos, m_effect->GetVersion());
		}
		else
		{
			RendererCommon.reset();
		}

		if (m_effect->GetVersion() >= Version16Alpha1)
		{
			bool alphaCutoffEnabled = true;

			if (m_effect->GetVersion() >= Version16Alpha6)
			{
				int32_t AlphaCutoffFlag = 0;
				memcpy(&AlphaCutoffFlag, pos, sizeof(int));
				pos += sizeof(int);
				alphaCutoffEnabled = (AlphaCutoffFlag == 1);
			}
			RendererCommon.BasicParameter.IsAlphaCutoffEnabled = alphaCutoffEnabled;

			if (alphaCutoffEnabled)
			{
				AlphaCutoff.load(pos, m_effect->GetVersion());
				RendererCommon.BasicParameter.EdgeThreshold = AlphaCutoff.EdgeThreshold;
				RendererCommon.BasicParameter.EdgeColor[0] = AlphaCutoff.EdgeColor.R;
				RendererCommon.BasicParameter.EdgeColor[1] = AlphaCutoff.EdgeColor.G;
				RendererCommon.BasicParameter.EdgeColor[2] = AlphaCutoff.EdgeColor.B;
				RendererCommon.BasicParameter.EdgeColor[3] = AlphaCutoff.EdgeColor.A;
				RendererCommon.BasicParameter.EdgeColorScaling = AlphaCutoff.EdgeColorScaling;

				RendererCommon.BasicParameter.IsAlphaCutoffEnabled = AlphaCutoff.Type != ParameterAlphaCutoff::EType::FIXED || AlphaCutoff.Fixed.Threshold != 0.0f;
			}
		}

		if (m_effect->GetVersion() >= Version16Alpha3)
		{
			int FalloffFlag = 0;
			memcpy(&FalloffFlag, pos, sizeof(int));
			pos += sizeof(int);
			EnableFalloff = (FalloffFlag == 1);

			if (EnableFalloff)
			{
				memcpy(&FalloffParam, pos, sizeof(FalloffParameter));
				pos += sizeof(FalloffParameter);
			}
		}

		if (m_effect->GetVersion() >= Version16Alpha4)
		{
			memcpy(&RendererCommon.BasicParameter.SoftParticleDistanceFar, pos, sizeof(float));
			pos += sizeof(float);
		}

		if (m_effect->GetVersion() >= Version16Alpha5)
		{
			memcpy(&RendererCommon.BasicParameter.SoftParticleDistanceNear, pos, sizeof(float));
			pos += sizeof(float);
			memcpy(&RendererCommon.BasicParameter.SoftParticleDistanceNearOffset, pos, sizeof(float));
			pos += sizeof(float);
		}

		LoadRendererParameter(pos, m_effect->GetSetting());

		Sound.Load(pos, ef->GetVersion());

		AdjustSettings(setting);
	}

	int nodeCount = 0;
	memcpy(&nodeCount, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("ChildrenCount : %d\n", nodeCount);
	m_Nodes.resize(nodeCount);
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		m_Nodes[i] = EffectNodeImplemented::Create(m_effect, this, pos);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNodeImplemented::~EffectNodeImplemented()
{
	for (size_t i = 0; i < m_Nodes.size(); i++)
	{
		ES_SAFE_DELETE(m_Nodes[i]);
	}
}

void EffectNodeImplemented::CalcCustomData(const Instance* instance, std::array<float, 4>& customData1, std::array<float, 4>& customData2)
{
	if (this->RendererCommon.BasicParameter.MaterialRenderDataPtr != nullptr)
	{
		if (this->RendererCommon.BasicParameter.MaterialRenderDataPtr->MaterialIndex >= 0)
		{
			auto material = m_effect->GetMaterial(this->RendererCommon.BasicParameter.MaterialRenderDataPtr->MaterialIndex);

			if (material != nullptr)
			{
				if (material->CustomData1 > 0)
				{
					customData1 = instance->GetCustomData(0);
				}
				if (material->CustomData2 > 0)
				{
					customData2 = instance->GetCustomData(1);
				}
			}
		}
	}
}

bool EffectNodeImplemented::Traverse(const std::function<bool(EffectNodeImplemented*)>& visitor)
{
	if (!visitor(this))
		return false; // cancel

	for (EffectNodeImplemented* child : m_Nodes)
	{
		if (!child->Traverse(visitor))
			return false;
	}

	return true; // continue
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* EffectNodeImplemented::GetEffect() const
{
	return m_effect;
}

int EffectNodeImplemented::GetGeneration() const
{
	return generation_;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EffectNodeImplemented::GetChildrenCount() const
{
	return (int)m_Nodes.size();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode* EffectNodeImplemented::GetChild(int index) const
{
	if (index >= GetChildrenCount())
		return nullptr;
	return m_Nodes[index];
}

EffectBasicRenderParameter EffectNodeImplemented::GetBasicRenderParameter() const
{
	EffectBasicRenderParameter param;
	param.MaterialIndex = RendererCommon.MaterialData.MaterialIndex;

	param.TextureIndexes = RendererCommon.TextureIndexes;
	param.TextureFilters = RendererCommon.TextureFilters;
	param.TextureWraps = RendererCommon.TextureWraps;

	if (RendererCommon.UVs[0].Type == UVAnimationType::Animation && RendererCommon.UVs[0].Animation.InterpolationType != 0)
	{
		param.FlipbookParams = UVFunctions::ToFlipbookParameter(RendererCommon.UVs[0]);
	}

	param.MaterialType = RendererCommon.MaterialType;

	param.UVDistortionIntensity = RendererCommon.UVDistortionIntensity;

	param.TextureBlendType = RendererCommon.TextureBlendType;

	param.BlendUVDistortionIntensity = RendererCommon.BlendUVDistortionIntensity;

	if (GetType() == EffectNodeType::Model)
	{
		auto pNodeModel = static_cast<const EffectNodeModel*>(this);
		param.EnableFalloff = pNodeModel->EnableFalloff;
		param.FalloffParam.ColorBlendType = static_cast<int32_t>(pNodeModel->FalloffParam.ColorBlendType);
		param.FalloffParam.BeginColor[0] = static_cast<float>(pNodeModel->FalloffParam.BeginColor.R) / 255.0f;
		param.FalloffParam.BeginColor[1] = static_cast<float>(pNodeModel->FalloffParam.BeginColor.G) / 255.0f;
		param.FalloffParam.BeginColor[2] = static_cast<float>(pNodeModel->FalloffParam.BeginColor.B) / 255.0f;
		param.FalloffParam.BeginColor[3] = static_cast<float>(pNodeModel->FalloffParam.BeginColor.A) / 255.0f;
		param.FalloffParam.EndColor[0] = static_cast<float>(pNodeModel->FalloffParam.EndColor.R / 255.0f);
		param.FalloffParam.EndColor[1] = static_cast<float>(pNodeModel->FalloffParam.EndColor.G / 255.0f);
		param.FalloffParam.EndColor[2] = static_cast<float>(pNodeModel->FalloffParam.EndColor.B / 255.0f);
		param.FalloffParam.EndColor[3] = static_cast<float>(pNodeModel->FalloffParam.EndColor.A / 255.0f);
		param.FalloffParam.Pow = pNodeModel->FalloffParam.Pow;
	}
	else
	{
		param.EnableFalloff = false;
		param.FalloffParam.BeginColor.fill(1.0f);
		param.FalloffParam.EndColor.fill(1.0f);
		param.FalloffParam.Pow = 1.0f;
	}

	param.EmissiveScaling = RendererCommon.EmissiveScaling;

	param.EdgeParam.Color[0] = static_cast<float>(AlphaCutoff.EdgeColor.R) / 255.0f;
	param.EdgeParam.Color[1] = static_cast<float>(AlphaCutoff.EdgeColor.G) / 255.0f;
	param.EdgeParam.Color[2] = static_cast<float>(AlphaCutoff.EdgeColor.B) / 255.0f;
	param.EdgeParam.Color[3] = static_cast<float>(AlphaCutoff.EdgeColor.A) / 255.0f;
	param.EdgeParam.Threshold = AlphaCutoff.EdgeThreshold;
	param.EdgeParam.ColorScaling = AlphaCutoff.EdgeColorScaling;
	param.AlphaBlend = RendererCommon.AlphaBlend;
	param.Distortion = RendererCommon.Distortion;
	param.DistortionIntensity = RendererCommon.DistortionIntensity;
	param.ZTest = RendererCommon.ZTest;
	param.ZWrite = RendererCommon.ZWrite;

	param.SoftParticleDistanceFar = RendererCommon.BasicParameter.SoftParticleDistanceFar;
	param.SoftParticleDistanceNear = RendererCommon.BasicParameter.SoftParticleDistanceNear;
	param.SoftParticleDistanceNearOffset = RendererCommon.BasicParameter.SoftParticleDistanceNearOffset;

	return param;
}

void EffectNodeImplemented::SetBasicRenderParameter(EffectBasicRenderParameter param)
{
	RendererCommon.TextureIndexes = param.TextureIndexes;
	RendererCommon.TextureFilters = param.TextureFilters;
	RendererCommon.TextureWraps = param.TextureWraps;

	if (param.FlipbookParams.EnableInterpolation)
	{
		RendererCommon.UVs[0].Type = UVAnimationType::Animation;
		RendererCommon.UVs[0].Animation.LoopType =
			static_cast<decltype(RendererCommon.UVs[0].Animation.LoopType)>(param.FlipbookParams.InterpolationType);
		RendererCommon.UVs[0].Animation.FrameCountX = param.FlipbookParams.FlipbookDivideX;
		RendererCommon.UVs[0].Animation.FrameCountY = param.FlipbookParams.FlipbookDivideY;
		RendererCommon.UVs[0].Animation.Position.x = param.FlipbookParams.Offset[0];
		RendererCommon.UVs[0].Animation.Position.y = param.FlipbookParams.Offset[1];
		RendererCommon.UVs[0].Animation.Position.w = param.FlipbookParams.OneSize[0];
		RendererCommon.UVs[0].Animation.Position.h = param.FlipbookParams.OneSize[1];
	}

	RendererCommon.UVDistortionIntensity = param.UVDistortionIntensity;

	RendererCommon.TextureBlendType = param.TextureBlendType;

	RendererCommon.AlphaBlend = param.AlphaBlend;
	RendererCommon.Distortion = param.Distortion;
	RendererCommon.DistortionIntensity = param.DistortionIntensity;
	RendererCommon.ZTest = param.ZTest;
	RendererCommon.ZWrite = param.ZWrite;
}

EffectModelParameter EffectNodeImplemented::GetEffectModelParameter()
{
	return {};
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::LoadRendererParameter(unsigned char*& pos, const SettingRef& setting)
{
	EffectNodeType type = EffectNodeType::NoneType;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : None\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::BeginRenderingGroup(InstanceGroup* group, Manager* manager, void* userData)
{
}

void EffectNodeImplemented::EndRenderingGroup(InstanceGroup* group, Manager* manager, void* userData)
{
}

void EffectNodeImplemented::Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::EndRendering(Manager* manager, void* userData)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::InitializeRenderedInstanceGroup(InstanceGroup& instanceGroup, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeImplemented::UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager)
{
}

float EffectNodeImplemented::GetFadeAlpha(const Instance& instance) const
{
	float alpha = 1.0f;

	if (RendererCommon.FadeInType == ParameterRendererCommon::FADEIN_ON && instance.m_LivingTime < RendererCommon.FadeIn.Frame)
	{
		float v = 1.0f;
		RendererCommon.FadeIn.Value.setValueToArg(v, 0.0f, 1.0f, (float)instance.m_LivingTime / (float)RendererCommon.FadeIn.Frame);

		alpha *= v;
	}

	if (RendererCommon.FadeOutType == ParameterRendererCommon::FADEOUT_WITHIN_LIFETIME)
	{
		if (instance.m_LivingTime + RendererCommon.FadeOut.Frame > instance.m_LivedTime)
		{
			float v = 1.0f;
			RendererCommon.FadeOut.Value.setValueToArg(v,
													   1.0f,
													   0.0f,
													   (float)(instance.m_LivingTime + RendererCommon.FadeOut.Frame - instance.m_LivedTime) /
														   (float)RendererCommon.FadeOut.Frame);

			alpha *= v;
		}
	}
	else if (RendererCommon.FadeOutType == ParameterRendererCommon::FADEOUT_AFTER_REMOVED)
	{
		if (instance.IsActive())
		{
			float v = 1.0f;
			RendererCommon.FadeOut.Value.setValueToArg(v,
													   1.0f,
													   0.0f,
													   instance.m_RemovingTime / RendererCommon.FadeOut.Frame);

			alpha *= v;
		}
	}

	return Clamp(alpha, 1.0f, 0.0f);
}

EffectInstanceTerm EffectNodeImplemented::CalculateInstanceTerm(EffectInstanceTerm& parentTerm) const
{
	EffectInstanceTerm ret;

	auto addWithClip = [](int v1, int v2) -> int
	{
		v1 = Max(v1, 0);
		v2 = Max(v2, 0);

		if (v1 >= INT_MAX / 2)
			return INT_MAX;

		if (v2 >= INT_MAX / 2)
			return INT_MAX;

		return v1 + v2;
	};

	int lifeMin = CommonValues.life.min;
	int lifeMax = CommonValues.life.max;

	if (CommonValues.RemoveWhenLifeIsExtinct <= 0)
	{
		lifeMin = INT_MAX;
		lifeMax = INT_MAX;
	}

	auto firstBeginMin = static_cast<int32_t>(CommonValues.GenerationTimeOffset.min);
	auto firstBeginMax = static_cast<int32_t>(CommonValues.GenerationTimeOffset.max);
	auto firstEndMin = addWithClip(firstBeginMin, lifeMin);
	auto firstEndMax = addWithClip(firstBeginMax, lifeMax);

	auto lastBeginMin = 0;
	auto lastBeginMax = 0;
	if (CommonValues.MaxGeneration > INT_MAX / 2)
	{
		lastBeginMin = INT_MAX / 2;
	}
	else
	{
		lastBeginMin = firstBeginMin + static_cast<int32_t>((CommonValues.MaxGeneration - 1) * CommonValues.GenerationTime.min);
	}

	if (CommonValues.MaxGeneration > INT_MAX / 2)
	{
		lastBeginMax = INT_MAX / 2;
	}
	else
	{
		lastBeginMax = firstBeginMax + static_cast<int32_t>((CommonValues.MaxGeneration - 1) * CommonValues.GenerationTime.max);
	}

	auto lastEndMin = addWithClip(lastBeginMin, lifeMin);
	auto lastEndMax = addWithClip(lastBeginMax, lifeMax);

	auto parentFirstTermMin = parentTerm.FirstInstanceEndMin - parentTerm.FirstInstanceStartMin;
	auto parentFirstTermMax = parentTerm.FirstInstanceEndMax - parentTerm.FirstInstanceStartMax;
	auto parentLastTermMin = parentTerm.LastInstanceEndMin - parentTerm.LastInstanceStartMin;
	auto parentLastTermMax = parentTerm.LastInstanceEndMax - parentTerm.LastInstanceStartMax;

	if (CommonValues.RemoveWhenParentIsRemoved > 0)
	{
		if (firstEndMin - firstBeginMin > parentFirstTermMin)
			firstEndMin = firstBeginMin + parentFirstTermMin;

		if (firstEndMax - firstBeginMax > parentFirstTermMax)
			firstEndMax = firstBeginMax + parentFirstTermMax;

		if (lastEndMin > INT_MAX / 2)
		{
			lastBeginMin = parentLastTermMin;
			lastEndMin = parentLastTermMin;
		}
		else if (lastEndMin - lastBeginMin > parentLastTermMin)
		{
			lastEndMin = lastBeginMin + parentLastTermMin;
		}

		if (lastEndMax > INT_MAX / 2)
		{
			lastBeginMax = parentLastTermMax;
			lastEndMax = parentLastTermMax;
		}
		else if (lastEndMax - lastBeginMax > parentLastTermMax)
		{
			lastEndMax = lastBeginMax + parentLastTermMax;
		}
	}

	ret.FirstInstanceStartMin = addWithClip(parentTerm.FirstInstanceStartMin, firstBeginMin);
	ret.FirstInstanceStartMax = addWithClip(parentTerm.FirstInstanceStartMax, firstBeginMax);
	ret.FirstInstanceEndMin = addWithClip(parentTerm.FirstInstanceStartMin, firstEndMin);
	ret.FirstInstanceEndMax = addWithClip(parentTerm.FirstInstanceStartMax, firstEndMax);

	ret.LastInstanceStartMin = addWithClip(parentTerm.LastInstanceStartMin, lastBeginMin);
	ret.LastInstanceStartMax = addWithClip(parentTerm.LastInstanceStartMax, lastBeginMax);
	ret.LastInstanceEndMin = addWithClip(parentTerm.LastInstanceStartMin, lastEndMin);
	ret.LastInstanceEndMax = addWithClip(parentTerm.LastInstanceStartMax, lastEndMax);

	// check children
	if (CommonValues.RemoveWhenChildrenIsExtinct > 0)
	{
		int childFirstEndMin = 0;
		int childFirstEndMax = 0;
		int childLastEndMin = 0;
		int childLastEndMax = 0;

		for (int32_t i = 0; i < GetChildrenCount(); i++)
		{
			auto child = static_cast<EffectNodeImplemented*>(GetChild(i));
			auto childTerm = child->CalculateInstanceTerm(ret);
			childFirstEndMin = Max(childTerm.FirstInstanceEndMin, childFirstEndMin);
			childFirstEndMax = Max(childTerm.FirstInstanceEndMax, childFirstEndMax);
			childLastEndMin = Max(childTerm.LastInstanceEndMin, childLastEndMin);
			childLastEndMax = Max(childTerm.LastInstanceEndMax, childLastEndMax);
		}

		ret.FirstInstanceEndMin = Min(ret.FirstInstanceEndMin, childFirstEndMin);
		ret.FirstInstanceEndMax = Min(ret.FirstInstanceEndMax, childFirstEndMax);
		ret.LastInstanceEndMin = Min(ret.LastInstanceEndMin, childLastEndMin);
		ret.LastInstanceEndMax = Min(ret.LastInstanceEndMax, childLastEndMax);
	}

	return ret;
}

EffectNodeImplemented* EffectNodeImplemented::Create(Effect* effect, EffectNode* parent, unsigned char*& pos)
{
	EffectNodeImplemented* effectnode = nullptr;

	EffectNodeType node_type = EffectNodeType::NoneType;
	memcpy(&node_type, pos, sizeof(int));

	if (node_type == EffectNodeType::Root)
	{
		EffekseerPrintDebug("* Create : EffectNodeRoot\n");
		effectnode = new EffectNodeRoot(effect, pos);
	}
	else if (node_type == EffectNodeType::NoneType)
	{
		EffekseerPrintDebug("* Create : EffectNodeNone\n");
		effectnode = new EffectNodeImplemented(effect, pos);
	}
	else if (node_type == EffectNodeType::Sprite)
	{
		EffekseerPrintDebug("* Create : EffectNodeSprite\n");
		effectnode = new EffectNodeSprite(effect, pos);
	}
	else if (node_type == EffectNodeType::Ribbon)
	{
		EffekseerPrintDebug("* Create : EffectNodeRibbon\n");
		effectnode = new EffectNodeRibbon(effect, pos);
	}
	else if (node_type == EffectNodeType::Ring)
	{
		EffekseerPrintDebug("* Create : EffectNodeRing\n");
		effectnode = new EffectNodeRing(effect, pos);
	}
	else if (node_type == EffectNodeType::Model)
	{
		EffekseerPrintDebug("* Create : EffectNodeModel\n");
		effectnode = new EffectNodeModel(effect, pos);
	}
	else if (node_type == EffectNodeType::Track)
	{
		EffekseerPrintDebug("* Create : EffectNodeTrack\n");
		effectnode = new EffectNodeTrack(effect, pos);
	}
	else
	{
		assert(0);
	}

	effectnode->LoadParameter(pos, parent, effect->GetSetting());

	return effectnode;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------