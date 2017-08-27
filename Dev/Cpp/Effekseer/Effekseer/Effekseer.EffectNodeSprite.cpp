

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Manager.h"
#include "Effekseer.Effect.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Vector3D.h"
#include "Effekseer.Instance.h"
#include "Effekseer.EffectNodeSprite.h"



#include "Renderer/Effekseer.SpriteRenderer.h"

#include "Effekseer.Setting.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
	void EffectNodeSprite::LoadRendererParameter(unsigned char*& pos, Setting* setting)
{
	int32_t type = 0;
	memcpy(&type, pos, sizeof(int));
	pos += sizeof(int);
	assert(type == GetType());
	EffekseerPrintDebug("Renderer : Sprite\n");

	auto ef = (EffectImplemented*) m_effect;
	int32_t size = 0;

	memcpy(&RenderingOrder, pos, sizeof(int));
	pos += sizeof(int);

	if (m_effect->GetVersion() >= 3)
	{
		AlphaBlend = RendererCommon.AlphaBlend;
	}
	else
	{
		memcpy(&AlphaBlend, pos, sizeof(int));
		pos += sizeof(int);
		RendererCommon.AlphaBlend = AlphaBlend;
	}

	memcpy(&Billboard, pos, sizeof(int));
	pos += sizeof(int);

	SpriteAllColor.load(pos, m_effect->GetVersion());
	EffekseerPrintDebug("SpriteColorAllType : %d\n", SpriteAllColor.type);

	memcpy(&SpriteColor.type, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("SpriteColorType : %d\n", SpriteColor.type);

	if (SpriteColor.type == SpriteColor.Default)
	{
	}
	else if (SpriteColor.type == SpriteColor.Fixed)
	{
		memcpy(&SpriteColor.fixed, pos, sizeof(SpriteColor.fixed));
		pos += sizeof(SpriteColor.fixed);
	}

	memcpy(&SpritePosition.type, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("SpritePosition : %d\n", SpritePosition.type);

	if (SpritePosition.type == SpritePosition.Default)
	{
		if (m_effect->GetVersion() >= 8)
		{
			memcpy(&SpritePosition.fixed, pos, sizeof(SpritePosition.fixed));
			pos += sizeof(SpritePosition.fixed);
			SpritePosition.type = SpritePosition.Fixed;
		}
		else
		{
			SpritePosition.fixed.ll.x = -0.5f;
			SpritePosition.fixed.ll.y = -0.5f;
			SpritePosition.fixed.lr.x = 0.5f;
			SpritePosition.fixed.lr.y = -0.5f;
			SpritePosition.fixed.ul.x = -0.5f;
			SpritePosition.fixed.ul.y = 0.5f;
			SpritePosition.fixed.ur.x = 0.5f;
			SpritePosition.fixed.ur.y = 0.5f;
			SpritePosition.type = SpritePosition.Fixed;
		}
	}
	else if (SpritePosition.type == SpritePosition.Fixed)
	{
		memcpy(&SpritePosition.fixed, pos, sizeof(SpritePosition.fixed));
		pos += sizeof(SpritePosition.fixed);
	}

	if (m_effect->GetVersion() >= 3)
	{
		SpriteTexture = RendererCommon.ColorTextureIndex;
	}
	else
	{
		memcpy(&SpriteTexture, pos, sizeof(int));
		pos += sizeof(int);
		RendererCommon.ColorTextureIndex = SpriteTexture;
	}

	// 右手系左手系変換
	if (setting->GetCoordinateSystem() == CoordinateSystem::LH)
	{
	}

	/* 位置拡大処理 */
	if (ef->IsDyanamicMagnificationValid())
	{
		if (SpritePosition.type == SpritePosition.Default)
		{
		}
		else if (SpritePosition.type == SpritePosition.Fixed)
		{
			SpritePosition.fixed.ll *= m_effect->GetMaginification();
			SpritePosition.fixed.lr *= m_effect->GetMaginification();
			SpritePosition.fixed.ul *= m_effect->GetMaginification();
			SpritePosition.fixed.ur *= m_effect->GetMaginification();
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::BeginRendering(int32_t count, Manager* manager)
{
	SpriteRenderer* renderer = manager->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = SpriteTexture;
		nodeParameter.EffectPointer = GetEffect();
		
		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		renderer->BeginRendering( nodeParameter, count, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::Rendering(const Instance& instance, Manager* manager)
{
	const InstanceValues& instValues = instance.rendererValues.sprite;
	SpriteRenderer* renderer = manager->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = SpriteTexture;
		nodeParameter.EffectPointer = GetEffect();

		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		SpriteRenderer::InstanceParameter instanceParameter;
		instValues._color.setValueToArg( instanceParameter.AllColor );
		instanceParameter.SRTMatrix43 = instance.GetGlobalMatrix43();

		// Inherit color
		color _color;
		if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
		{
			_color = color::mul(instValues._originalColor, instance.ColorParent);
		}
		else
		{
			_color = instValues._originalColor;
		}

		color color_ll = _color;
		color color_lr = _color;
		color color_ul = _color;
		color color_ur = _color;

		if( SpriteColor.type == SpriteColorParameter::Default )
		{
		}
		else if( SpriteColor.type == SpriteColorParameter::Fixed )
		{
			color_ll = color::mul( color_ll, SpriteColor.fixed.ll );
			color_lr = color::mul( color_lr, SpriteColor.fixed.lr );
			color_ul = color::mul( color_ul, SpriteColor.fixed.ul );
			color_ur = color::mul( color_ur, SpriteColor.fixed.ur );
		}

		color_ll.setValueToArg( instanceParameter.Colors[0] );
		color_lr.setValueToArg( instanceParameter.Colors[1] );
		color_ul.setValueToArg( instanceParameter.Colors[2] );
		color_ur.setValueToArg( instanceParameter.Colors[3] );
		

		if( SpritePosition.type == SpritePosition.Default )
		{
			instanceParameter.Positions[0].X = -0.5f;
			instanceParameter.Positions[0].Y = -0.5f;
			instanceParameter.Positions[1].X = 0.5f;
			instanceParameter.Positions[1].Y = -0.5f;
			instanceParameter.Positions[2].X = -0.5f;
			instanceParameter.Positions[2].Y = 0.5f;
			instanceParameter.Positions[3].X = 0.5f;
			instanceParameter.Positions[3].Y = 0.5f;
		}
		else if( SpritePosition.type == SpritePosition.Fixed )
		{
			SpritePosition.fixed.ll.setValueToArg( instanceParameter.Positions[0] );
			SpritePosition.fixed.lr.setValueToArg( instanceParameter.Positions[1] );
			SpritePosition.fixed.ul.setValueToArg( instanceParameter.Positions[2] );
			SpritePosition.fixed.ur.setValueToArg( instanceParameter.Positions[3] );
		}

		instanceParameter.UV = instance.GetUV();
		renderer->Rendering( nodeParameter, instanceParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::EndRendering(Manager* manager)
{
	SpriteRenderer* renderer = manager->GetSpriteRenderer();
	if( renderer != NULL )
	{
		SpriteRenderer::NodeParameter nodeParameter;
		nodeParameter.AlphaBlend = AlphaBlend;
		nodeParameter.TextureFilter = RendererCommon.FilterType;
		nodeParameter.TextureWrap = RendererCommon.WrapType;
		nodeParameter.ZTest = RendererCommon.ZTest;
		nodeParameter.ZWrite = RendererCommon.ZWrite;
		nodeParameter.Billboard = Billboard;
		nodeParameter.ColorTextureIndex = SpriteTexture;
		nodeParameter.EffectPointer = GetEffect();

		nodeParameter.Distortion = RendererCommon.Distortion;
		nodeParameter.DistortionIntensity = RendererCommon.DistortionIntensity;

		renderer->EndRendering( nodeParameter, m_userData );
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::InitializeRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.sprite;

	if( SpriteAllColor.type == StandardColorParameter::Fixed )
	{
		instValues.allColorValues.fixed._color = SpriteAllColor.fixed.all;
		instValues._originalColor = instValues.allColorValues.fixed._color;
	}
	else if( SpriteAllColor.type == StandardColorParameter::Random )
	{
		instValues.allColorValues.random._color = SpriteAllColor.random.all.getValue(*(manager));
		instValues._originalColor = instValues.allColorValues.random._color;
	}
	else if( SpriteAllColor.type == StandardColorParameter::Easing )
	{
		instValues.allColorValues.easing.start = SpriteAllColor.easing.all.getStartValue(*(manager));
		instValues.allColorValues.easing.end = SpriteAllColor.easing.all.getEndValue(*(manager));

		float t = instance.m_LivingTime / instance.m_LivedTime;

		SpriteAllColor.easing.all.setValueToArg(
			instValues._originalColor,
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
	else if( SpriteAllColor.type == StandardColorParameter::FCurve_RGBA )
	{
		instValues.allColorValues.fcurve_rgba.offset[0] = SpriteAllColor.fcurve_rgba.FCurve->R.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[1] = SpriteAllColor.fcurve_rgba.FCurve->G.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[2] = SpriteAllColor.fcurve_rgba.FCurve->B.GetOffset(*(manager));
		instValues.allColorValues.fcurve_rgba.offset[3] = SpriteAllColor.fcurve_rgba.FCurve->A.GetOffset(*(manager));
		
		instValues._originalColor.r = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[0] + SpriteAllColor.fcurve_rgba.FCurve->R.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._originalColor.g = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[1] + SpriteAllColor.fcurve_rgba.FCurve->G.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._originalColor.b = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[2] + SpriteAllColor.fcurve_rgba.FCurve->B.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._originalColor.a = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[3] + SpriteAllColor.fcurve_rgba.FCurve->A.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
	}

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues._color = color::mul(instValues._originalColor, instance.ColorParent);
	}
	else
	{
		instValues._color = instValues._originalColor;
	}
	
	instance.ColorInheritance = instValues._color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectNodeSprite::UpdateRenderedInstance(Instance& instance, Manager* manager)
{
	InstanceValues& instValues = instance.rendererValues.sprite;

	if (SpriteAllColor.type == StandardColorParameter::Fixed)
	{
		instValues._originalColor = instValues.allColorValues.fixed._color;
	}
	else if (SpriteAllColor.type == StandardColorParameter::Random)
	{
		instValues._originalColor = instValues.allColorValues.random._color;
	}
	if( SpriteAllColor.type == StandardColorParameter::Easing )
	{
		float t = instance.m_LivingTime / instance.m_LivedTime;

		SpriteAllColor.easing.all.setValueToArg(
			instValues._originalColor, 
			instValues.allColorValues.easing.start,
			instValues.allColorValues.easing.end,
			t );
	}
	else if( SpriteAllColor.type == StandardColorParameter::FCurve_RGBA )
	{
		instValues._originalColor.r = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[0] + SpriteAllColor.fcurve_rgba.FCurve->R.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._originalColor.g = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[1] + SpriteAllColor.fcurve_rgba.FCurve->G.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._originalColor.b = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[2] + SpriteAllColor.fcurve_rgba.FCurve->B.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
		instValues._originalColor.a = (uint8_t)Clamp( (instValues.allColorValues.fcurve_rgba.offset[3] + SpriteAllColor.fcurve_rgba.FCurve->A.GetValue( (int32_t)instance.m_LivingTime )), 255, 0);
	}

	float fadeAlpha = GetFadeAlpha(instance);
	if (fadeAlpha != 1.0f)
	{
		instValues._originalColor.a = (uint8_t)(instValues._originalColor.a * fadeAlpha);
	}

	if (RendererCommon.ColorBindType == BindType::Always || RendererCommon.ColorBindType == BindType::WhenCreating)
	{
		instValues._color = color::mul(instValues._originalColor, instance.ColorParent);
	}
	else
	{
		instValues._color = instValues._originalColor;
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
