#include "Effekseer.EffectNodeModel.h"

#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Vector3D.h"
#include "SIMD/Utils.h"

#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"
#include "Renderer/Effekseer.ModelRenderer.h"

#include "Effekseer.Setting.h"

namespace Effekseer
{

void EffectNodeModel::LoadRendererParameter(unsigned char*& pos, const SettingRef& setting)
{
	eEffectNodeType type = eEffectNodeType::NoneType;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : Model\n");

	if (m_effect->GetVersion() >= Version16Alpha3)
	{
		memcpy(&Mode, pos, sizeof(int));
		pos += sizeof(int);
	}
	else
	{
		Mode = ModelReferenceType::File;
	}

	if (Mode == ModelReferenceType::File)
	{
		AlphaBlend = RendererCommon.AlphaBlend;

		if (m_effect->GetVersion() >= 7)
		{
			float Magnification;
			memcpy(&Magnification, pos, sizeof(float));
			pos += sizeof(float);
		}

		memcpy(&ModelIndex, pos, sizeof(int));
		pos += sizeof(int);

		if (m_effect->GetVersion() < 15)
		{
			int NormalTextureIndex = 0;
			memcpy(&NormalTextureIndex, pos, sizeof(int));
			pos += sizeof(int);
			EffekseerPrintDebug("NormalTextureIndex : %d\n", NormalTextureIndex);
			RendererCommon.Texture2Index = NormalTextureIndex;
			RendererCommon.BasicParameter.TextureIndexes[1] = NormalTextureIndex;
			RendererCommon.BasicParameter.TextureFilters[1] = RendererCommon.BasicParameter.TextureFilters[0];
			RendererCommon.BasicParameter.TextureWraps[1] = RendererCommon.BasicParameter.TextureWraps[0];
		}
	}
	else if (Mode == ModelReferenceType::Procedural)
	{
		memcpy(&ModelIndex, pos, sizeof(int));
		pos += sizeof(int);
	}

	if (m_effect->GetVersion() >= 12)
	{
		memcpy(&Billboard, pos, sizeof(int));
		pos += sizeof(int);
	}
	else
	{
		Billboard = BillboardType::Fixed;
	}

	if (m_effect->GetVersion() < 15)
	{
		int32_t lighting;
		memcpy(&lighting, pos, sizeof(int));
		pos += sizeof(int);
		const auto Lighting = lighting > 0;

		if (Lighting && !RendererCommon.Distortion)
		{
			RendererCommon.MaterialType = RendererMaterialType::Lighting;
			RendererCommon.BasicParameter.MaterialType = RendererMaterialType::Lighting;
		}
	}

	memcpy(&Culling, pos, sizeof(int));
	pos += sizeof(int);

	AllColor.load(pos, m_effect->GetVersion());

	if (Version16Alpha3 > m_effect->GetVersion() && m_effect->GetVersion() >= Version16Alpha1)
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
}

void EffectNodeModel::BeginRendering(int32_t count, Manager* manager, const InstanceGlobal* global, void* userData)
{
	ModelRendererRef renderer = manager->GetModelRenderer();
	if (renderer != nullptr)
	{

		nodeParam_ = GetNodeParameter(manager, global);
		renderer->BeginRendering(nodeParam_, count, userData);
	}
}

void EffectNodeModel::Rendering(const Instance& instance, const Instance* next_instance, int index, Manager* manager, void* userData)
{
	const InstanceValues& instValues = instance.rendererValues.model;
	ModelRendererRef renderer = manager->GetModelRenderer();
	if (renderer != nullptr)
	{
		ModelRenderer::InstanceParameter instanceParameter;
		instanceParameter.SRTMatrix43 = instance.GetRenderedGlobalMatrix();
		instanceParameter.Time = (int32_t)instance.m_LivingTime;

		instanceParameter.UV = instance.GetUV(0);
		instanceParameter.AlphaUV = instance.GetUV(1);
		instanceParameter.UVDistortionUV = instance.GetUV(2);
		instanceParameter.BlendUV = instance.GetUV(3);
		instanceParameter.BlendAlphaUV = instance.GetUV(4);
		instanceParameter.BlendUVDistortionUV = instance.GetUV(5);

		instanceParameter.FlipbookIndexAndNextRate = instance.GetFlipbookIndexAndNextRate();

		instanceParameter.AlphaThreshold = instance.m_AlphaThreshold;

		if (nodeParam_.EnableViewOffset)
		{
			instanceParameter.ViewOffsetDistance = instance.translation_values.view_offset.distance;
		}

		CalcCustomData(&instance, instanceParameter.CustomData1, instanceParameter.CustomData2);

		Color _color;
		if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
		{
			_color = Color::Mul(instValues._original, instance.ColorParent);
		}
		else
		{
			_color = instValues._original;
		}
		instanceParameter.AllColor = _color;

		if (instance.m_pContainer->GetRootInstance()->IsGlobalColorSet)
		{
			instanceParameter.AllColor = Color::Mul(instanceParameter.AllColor, instance.m_pContainer->GetRootInstance()->GlobalColor);
		}

		renderer->Rendering(nodeParam_, instanceParameter, userData);
	}
}

void EffectNodeModel::EndRendering(Manager* manager, void* userData)
{
	ModelRendererRef renderer = manager->GetModelRenderer();
	if (renderer != nullptr)
	{
		renderer->EndRendering(nodeParam_, userData);
	}
}

void EffectNodeModel::InitializeRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager)
{
	IRandObject& rand = instance.GetRandObject();
	InstanceValues& instValues = instance.rendererValues.model;

	AllTypeColorFunctions::Init(instValues.allColorValues, rand, AllColor);
	instValues._original = AllTypeColorFunctions::Calculate(instValues.allColorValues, AllColor, instance.m_LivingTime, instance.m_LivedTime);

	// TODO refactor
	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues._color = Color::Mul(instValues._original, instance.ColorParent);
	}
	else
	{
		instValues._color = instValues._original;
	}

	instance.ColorInheritance = instValues._color;
}

void EffectNodeModel::UpdateRenderedInstance(Instance& instance, InstanceGroup& instanceGroup, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.model;

	instValues._original = AllTypeColorFunctions::Calculate(instValues.allColorValues, AllColor, instance.m_LivingTime, instance.m_LivedTime);

	float fadeAlpha = GetFadeAlpha(instance);
	if (fadeAlpha != 1.0f)
	{
		instValues._original.A = (uint8_t)(instValues._original.A * fadeAlpha);
	}

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues._color = Color::Mul(instValues._original, instance.ColorParent);
	}
	else
	{
		instValues._color = instValues._original;
	}

	instance.ColorInheritance = instValues._color;
}

ModelRenderer::NodeParameter EffectNodeModel::GetNodeParameter(const Manager* manager, const InstanceGlobal* global)
{
	ModelRenderer::NodeParameter nodeParameter;
	nodeParameter.ZTest = RendererCommon.ZTest;
	nodeParameter.ZWrite = RendererCommon.ZWrite;
	nodeParameter.EffectPointer = GetEffect();
	nodeParameter.LocalTime = global->GetUpdatedFrame() / 60.0f;
	nodeParameter.ModelIndex = ModelIndex;
	nodeParameter.Culling = Culling;
	nodeParameter.Billboard = Billboard;
	nodeParameter.Magnification = m_effect->GetMaginification();
	nodeParameter.IsRightHand = manager->GetSetting()->GetCoordinateSystem() == CoordinateSystem::RH;
	nodeParameter.Maginification = GetEffect()->GetMaginification();

	nodeParameter.DepthParameterPtr = &DepthValues.DepthParameter;
	nodeParameter.BasicParameterPtr = &RendererCommon.BasicParameter;

	nodeParameter.EnableFalloff = EnableFalloff;
	nodeParameter.FalloffParam = FalloffParam;

	nodeParameter.EnableViewOffset = (TranslationParam.TranslationType == ParameterTranslationType_ViewOffset);

	nodeParameter.IsProceduralMode = Mode == ModelReferenceType::Procedural;

	nodeParameter.UserData = GetRenderingUserData();

	return nodeParameter;
}

} // namespace Effekseer
