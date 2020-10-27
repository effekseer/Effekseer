

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Manager.h"
#include "Effekseer.Vector3D.h"
#include "SIMD/Effekseer.SIMDUtils.h"

#include "Effekseer.Instance.h"
#include "Effekseer.InstanceContainer.h"
#include "Effekseer.InstanceGlobal.h"

#include "Effekseer.EffectNodeModel.h"

#include "Renderer/Effekseer.ModelRenderer.h"

#include "Effekseer.Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::LoadRendererParameter(unsigned char*& pos, Setting* setting)
{
	int32_t type = 0;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : Model\n");

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
		memcpy(&NormalTextureIndex, pos, sizeof(int));
		pos += sizeof(int);
		EffekseerPrintDebug("NormalTextureIndex : %d\n", NormalTextureIndex);
		RendererCommon.Texture2Index = NormalTextureIndex;
		RendererCommon.BasicParameter.Texture2Index = NormalTextureIndex;
		RendererCommon.BasicParameter.TextureFilter2 = RendererCommon.BasicParameter.TextureFilter1;
		RendererCommon.BasicParameter.TextureWrap2 = RendererCommon.BasicParameter.TextureWrap1;
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
		Lighting = lighting > 0;

		if (Lighting && !RendererCommon.Distortion)
		{
			RendererCommon.MaterialType = RendererMaterialType::Lighting;
			RendererCommon.BasicParameter.MaterialType = RendererMaterialType::Lighting;
		}
	}

	memcpy(&Culling, pos, sizeof(int));
	pos += sizeof(int);

	AllColor.load(pos, m_effect->GetVersion());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::BeginRendering(int32_t count, Manager* manager)
{
	ModelRenderer* renderer = manager->GetModelRenderer();
	if (renderer != NULL)
	{
		ModelRenderer::NodeParameter nodeParameter;
		// nodeParameter.TextureFilter = RendererCommon.FilterType;
		// nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Billboard = Billboard;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetCoordinateSystem() ==
									CoordinateSystem::RH;

		nodeParameter.DepthParameterPtr = &DepthValues.DepthParameter;
		//nodeParameter.DepthOffset = DepthValues.DepthOffset;
		//nodeParameter.IsDepthOffsetScaledWithCamera = DepthValues.IsDepthOffsetScaledWithCamera;
		//nodeParameter.IsDepthOffsetScaledWithParticleScale = DepthValues.IsDepthOffsetScaledWithParticleScale;

		nodeParameter.BasicParameterPtr = &RendererCommon.BasicParameter;
		renderer->BeginRendering(nodeParameter, count, m_userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::Rendering(const Instance& instance, const Instance* next_instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.model;
	ModelRenderer* renderer = manager->GetModelRenderer();
	if (renderer != NULL)
	{
		ModelRenderer::NodeParameter nodeParameter;
		//nodeParameter.TextureFilter = RendererCommon.FilterType;
		//nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Billboard = Billboard;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetCoordinateSystem() ==
									CoordinateSystem::RH;

		nodeParameter.DepthParameterPtr = &DepthValues.DepthParameter;
		//nodeParameter.DepthOffset = DepthValues.DepthOffset;
		//nodeParameter.IsDepthOffsetScaledWithCamera = DepthValues.IsDepthOffsetScaledWithCamera;
		//nodeParameter.IsDepthOffsetScaledWithParticleScale = DepthValues.IsDepthOffsetScaledWithParticleScale;
		nodeParameter.BasicParameterPtr = &RendererCommon.BasicParameter;

		ModelRenderer::InstanceParameter instanceParameter;
		instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();
		instanceParameter.Time = (int32_t)instance.m_LivingTime;

#ifdef __EFFEKSEER_BUILD_VERSION16__
		instanceParameter.UV = instance.GetUV(0);
		instanceParameter.AlphaUV = instance.GetUV(1);

		instanceParameter.FlipbookIndexAndNextRate = instance.m_flipbookIndexAndNextRate;

		instanceParameter.AlphaThreshold = instance.m_AlphaThreshold;
#else
		instanceParameter.UV = instance.GetUV();
#endif
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

		nodeParameter.BasicParameterPtr = &RendererCommon.BasicParameter;
		renderer->Rendering(nodeParameter, instanceParameter, m_userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::EndRendering(Manager* manager)
{
	ModelRenderer* renderer = manager->GetModelRenderer();
	if (renderer != NULL)
	{
		ModelRenderer::NodeParameter nodeParameter;
		//nodeParameter.TextureFilter = RendererCommon.FilterType;
		//nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Billboard = Billboard;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetSetting()->GetCoordinateSystem() ==
									CoordinateSystem::RH;

		nodeParameter.DepthParameterPtr = &DepthValues.DepthParameter;
		//nodeParameter.DepthOffset = DepthValues.DepthOffset;
		//nodeParameter.IsDepthOffsetScaledWithCamera = DepthValues.IsDepthOffsetScaledWithCamera;
		//nodeParameter.IsDepthOffsetScaledWithParticleScale = DepthValues.IsDepthOffsetScaledWithParticleScale;

		nodeParameter.BasicParameterPtr = &RendererCommon.BasicParameter;
		renderer->EndRendering(nodeParameter, m_userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.model;
	auto instanceGlobal = instance.m_pContainer->GetRootInstance();

	if (AllColor.type == StandardColorParameter::Fixed)
	{
		instValues._original = AllColor.fixed.all;
		instValues.allColorValues.fixed._color = instValues._original;
	}
	else if (AllColor.type == StandardColorParameter::Random)
	{
		instValues._original = AllColor.random.all.getValue(*instanceGlobal);
		instValues.allColorValues.random._color = instValues._original;
	}
	else if (AllColor.type == StandardColorParameter::Easing)
	{
		instValues.allColorValues.easing.start = AllColor.easing.all.getStartValue(*instanceGlobal);
		instValues.allColorValues.easing.end = AllColor.easing.all.getEndValue(*instanceGlobal);

		float t = instance.m_LivingTime / instance.m_LivedTime;

		AllColor.easing.all.setValueToArg(
			instValues._original,
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t);
	}
	else if (AllColor.type == StandardColorParameter::FCurve_RGBA)
	{
		instValues.allColorValues.fcurve_rgba.offset = AllColor.fcurve_rgba.FCurve->GetOffsets(*instanceGlobal);
		auto fcurveColors = AllColor.fcurve_rgba.FCurve->GetValues(instance.m_LivingTime, instance.m_LivedTime);
		instValues._original.R = (uint8_t)Clamp((instValues.allColorValues.fcurve_rgba.offset[0] + fcurveColors[0]), 255, 0);
		instValues._original.G = (uint8_t)Clamp((instValues.allColorValues.fcurve_rgba.offset[1] + fcurveColors[1]), 255, 0);
		instValues._original.B = (uint8_t)Clamp((instValues.allColorValues.fcurve_rgba.offset[2] + fcurveColors[2]), 255, 0);
		instValues._original.A = (uint8_t)Clamp((instValues.allColorValues.fcurve_rgba.offset[3] + fcurveColors[3]), 255, 0);
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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.model;

	if (AllColor.type == StandardColorParameter::Fixed)
	{
		instValues._original = instValues.allColorValues.fixed._color;
	}
	else if (AllColor.type == StandardColorParameter::Random)
	{
		instValues._original = instValues.allColorValues.random._color;
	}
	else if (AllColor.type == StandardColorParameter::Easing)
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		AllColor.easing.all.setValueToArg(
			instValues._original,
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t);
	}
	else if (AllColor.type == StandardColorParameter::FCurve_RGBA)
	{
		auto fcurveColors = AllColor.fcurve_rgba.FCurve->GetValues(instance.m_LivingTime, instance.m_LivedTime);
		instValues._original.R = (uint8_t)Clamp((instValues.allColorValues.fcurve_rgba.offset[0] + fcurveColors[0]), 255, 0);
		instValues._original.G = (uint8_t)Clamp((instValues.allColorValues.fcurve_rgba.offset[1] + fcurveColors[1]), 255, 0);
		instValues._original.B = (uint8_t)Clamp((instValues.allColorValues.fcurve_rgba.offset[2] + fcurveColors[2]), 255, 0);
		instValues._original.A = (uint8_t)Clamp((instValues.allColorValues.fcurve_rgba.offset[3] + fcurveColors[3]), 255, 0);
	}

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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
