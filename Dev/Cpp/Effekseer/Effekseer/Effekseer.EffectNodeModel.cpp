

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Instance.h"
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

	AlphaBlend = Texture.AlphaBlend;

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
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.ColorTextureIndex = Texture.ColorTextureIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Lighting = Lighting;
		nodeParameter.NormalTextureIndex = NormalTextureIndex;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetCoordinateSystem() ==
			CoordinateSystem::RH;

		nodeParameter.Distortion = Texture.Distortion;
		nodeParameter.DistortionIntensity = Texture.DistortionIntensity;

		renderer->BeginRendering(nodeParameter, count, m_userData);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::Rendering(const Instance& instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.model;
	ModelRenderer* renderer = manager->GetModelRenderer();
	if( renderer != NULL )
	{
		ModelRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.ColorTextureIndex = Texture.ColorTextureIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Lighting = Lighting;
		nodeParameter.NormalTextureIndex = NormalTextureIndex;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetCoordinateSystem() ==
			CoordinateSystem::RH;

		nodeParameter.Distortion = Texture.Distortion;
		nodeParameter.DistortionIntensity = Texture.DistortionIntensity;


		ModelRenderer::InstanceParameter instanceParameter;
		instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		instanceParameter.UV = instance.GetUV();
		
		instValues._color.setValueToArg( instanceParameter.AllColor );
		float fadeAlpha = GetFadeAlpha( instance );
		if( fadeAlpha != 1.0f )
		{
			instanceParameter.AllColor.A = (uint8_t)(instanceParameter.AllColor.A * fadeAlpha);
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
		nodeParameter.TextureFilter = Texture.FilterType;
		nodeParameter.TextureWrap = Texture.WrapType;
		nodeParameter.ZTest = Texture.ZTest;
		nodeParameter.ZWrite = Texture.ZWrite;
		nodeParameter.EffectPointer = GetEffect();
		nodeParameter.ModelIndex = ModelIndex;
		nodeParameter.ColorTextureIndex = Texture.ColorTextureIndex;
		nodeParameter.Culling = Culling;
		nodeParameter.Lighting = Lighting;
		nodeParameter.NormalTextureIndex = NormalTextureIndex;
		nodeParameter.Magnification = m_effect->GetMaginification();
		nodeParameter.IsRightHand = manager->GetSetting()->GetCoordinateSystem() ==
			CoordinateSystem::RH;

		nodeParameter.Distortion = Texture.Distortion;
		nodeParameter.DistortionIntensity = Texture.DistortionIntensity;

		renderer->EndRendering( nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.model;

	if( AllColor.type == StandardColorParameter::Fixed )
	{
		instValues._color = AllColor.fixed.all;
	}
	else if( AllColor.type == StandardColorParameter::Random )
	{
		instValues._color = AllColor.random.all.getValue(*(manager));
	}
	else if( AllColor.type == StandardColorParameter::Easing )
	{
		instValues.allColorValues.easing.start = AllColor.easing.all.getStartValue(*(manager));
		instValues.allColorValues.easing.end = AllColor.easing.all.getEndValue(*(manager));

		float t = instance.m_LivingTime / instance.m_LivedTime;

		AllColor.easing.all.setValueToArg(
			instValues._color, 
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
	else if( AllColor.type == StandardColorParameter::FCurve_RGBA )
	{
		instValues.allColorValues.fcurve_rgba.offset[0] = AllColor.fcurve_rgba.FCurve->R.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[1] = AllColor.fcurve_rgba.FCurve->G.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[2] = AllColor.fcurve_rgba.FCurve->B.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[3] = AllColor.fcurve_rgba.FCurve->A.GetOffset(*(manager));
		
		instValues._color.r = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[0] + AllColor.fcurve_rgba.FCurve->R.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.g = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[1] + AllColor.fcurve_rgba.FCurve->G.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.b = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[2] + AllColor.fcurve_rgba.FCurve->B.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.a = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[3] + AllColor.fcurve_rgba.FCurve->A.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeModel::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.model;

	if( AllColor.type == StandardColorParameter::Easing )
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		AllColor.easing.all.setValueToArg(
			instValues._color, 
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
	else if( AllColor.type == StandardColorParameter::FCurve_RGBA )
	{
		instValues._color.r = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[0] + AllColor.fcurve_rgba.FCurve->R.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.g = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[1] + AllColor.fcurve_rgba.FCurve->G.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.b = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[2] + AllColor.fcurve_rgba.FCurve->B.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._color.a = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[3] + AllColor.fcurve_rgba.FCurve->A.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
