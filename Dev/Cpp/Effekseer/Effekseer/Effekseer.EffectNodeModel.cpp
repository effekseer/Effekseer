

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"

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
	memcpy( &type, pos, sizeof(int) );
	pos += sizeof(int);
	assert( type == GetType() );
	EffekseerPrintDebug("Renderer : Model\n");

	int32_t size = 0;

	AlphaBlend = RendererCommon.AlphaBlend;

	if( m_effect->GetVersion() >= 7 )
	{
		float Magnification;
		memcpy( &Magnification, pos, sizeof(float) );
		pos += sizeof(float);
	}

	memcpy( &ModelIndex, pos, sizeof(int) );
	pos += sizeof(int);

	memcpy( &NormalTextureIndex, pos, sizeof(int) );
	pos += sizeof(int);
	EffekseerPrintDebug("NormalTextureIndex : %d\n", NormalTextureIndex );

	if (m_effect->GetVersion() >= 12)
	{
		memcpy(&Billboard, pos, sizeof(int));
		pos += sizeof(int);
	}
	else
	{
		Billboard = BillboardType::Fixed;
	}

	int32_t lighting;
	memcpy( &lighting, pos, sizeof(int) );
	pos += sizeof(int);
	Lighting = lighting > 0;

	memcpy( &Culling, pos, sizeof(int) );
	pos += sizeof(int);

	AllColor.load( pos, m_effect->GetVersion() );
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
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.ColorTextureIndex = RendererCommon.ColorTextureIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Billboard = Billboard;
		nodeParameter.Lighting = Lighting;
		nodeParameter.NormalTextureIndex = NormalTextureIndex;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetCoordinateSystem() ==
			CoordinateSystem::RH;

		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		nodeParameter.DepthOffset = DepthValues.DepthOffset;
		nodeParameter.IsDepthOffsetScaledWithCamera = DepthValues.IsDepthOffsetScaledWithCamera;
		nodeParameter.IsDepthOffsetScaledWithParticleScale = DepthValues.IsDepthOffsetScaledWithParticleScale;

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
	if( renderer != NULL )
	{
		ModelRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.ColorTextureIndex = RendererCommon.ColorTextureIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Billboard = Billboard;
		nodeParameter.Lighting = Lighting;
		nodeParameter.NormalTextureIndex = NormalTextureIndex;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetCoordinateSystem() ==
			CoordinateSystem::RH;

		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		nodeParameter.DepthOffset = DepthValues.DepthOffset;
		nodeParameter.IsDepthOffsetScaledWithCamera = DepthValues.IsDepthOffsetScaledWithCamera;
		nodeParameter.IsDepthOffsetScaledWithParticleScale = DepthValues.IsDepthOffsetScaledWithParticleScale;

		ModelRenderer::InstanceParameter instanceParameter;
		instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();
		instanceParameter.Time = instance.m_LivingTime;

		instanceParameter.UV = instance.GetUV();
		
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

		renderer->Rendering( nodeParameter, instanceParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::EndRendering(Manager* manager)
{
	ModelRenderer* renderer = manager->GetModelRenderer();
	if( renderer != NULL )
	{
		ModelRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.ColorTextureIndex = RendererCommon.ColorTextureIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Billboard = Billboard;
		nodeParameter.Lighting = Lighting;
		nodeParameter.NormalTextureIndex = NormalTextureIndex;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetSetting()->GetCoordinateSystem() ==
			CoordinateSystem::RH;

		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		nodeParameter.DepthOffset = DepthValues.DepthOffset;
		nodeParameter.IsDepthOffsetScaledWithCamera = DepthValues.IsDepthOffsetScaledWithCamera;
		nodeParameter.IsDepthOffsetScaledWithParticleScale = DepthValues.IsDepthOffsetScaledWithParticleScale;

		renderer->EndRendering( nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.model;
	auto instanceGlobal = instance.m_pContainer->GetRootInstance();

	if( AllColor.type == StandardColorParameter::Fixed )
	{
		instValues._original = AllColor.fixed.all;
		instValues.allColorValues.fixed._color = instValues._original;
	}
	else if( AllColor.type == StandardColorParameter::Random )
	{
		instValues._original = AllColor.random.all.getValue(*instanceGlobal);
		instValues.allColorValues.random._color = instValues._original;
	}
	else if( AllColor.type == StandardColorParameter::Easing )
	{
		instValues.allColorValues.easing.start = AllColor.easing.all.getStartValue(*instanceGlobal);
		instValues.allColorValues.easing.end = AllColor.easing.all.getEndValue(*instanceGlobal);

		float t = instance.m_LivingTime / instance.m_LivedTime;

		AllColor.easing.all.setValueToArg(
			instValues._original,
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
	else if( AllColor.type == StandardColorParameter::FCurve_RGBA )
	{
		instValues.allColorValues.fcurve_rgba.offset[0] = AllColor.fcurve_rgba.FCurve->R.GetOffset(*instanceGlobal);
		instValues.allColorValues.fcurve_rgba.offset[1] = AllColor.fcurve_rgba.FCurve->G.GetOffset(*instanceGlobal);
		instValues.allColorValues.fcurve_rgba.offset[2] = AllColor.fcurve_rgba.FCurve->B.GetOffset(*instanceGlobal);
		instValues.allColorValues.fcurve_rgba.offset[3] = AllColor.fcurve_rgba.FCurve->A.GetOffset(*instanceGlobal);
		
		instValues._original.R = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[0] + AllColor.fcurve_rgba.FCurve->R.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._original.G = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[1] + AllColor.fcurve_rgba.FCurve->G.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._original.B = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[2] + AllColor.fcurve_rgba.FCurve->B.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._original.A = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[3] + AllColor.fcurve_rgba.FCurve->A.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
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
	else if( AllColor.type == StandardColorParameter::Easing )
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		AllColor.easing.all.setValueToArg(
			instValues._original,
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
	else if( AllColor.type == StandardColorParameter::FCurve_RGBA )
	{
		instValues._original.R = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[0] + AllColor.fcurve_rgba.FCurve->R.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._original.G = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[1] + AllColor.fcurve_rgba.FCurve->G.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._original.B = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[2] + AllColor.fcurve_rgba.FCurve->B.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._original.A = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[3] + AllColor.fcurve_rgba.FCurve->A.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
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
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
