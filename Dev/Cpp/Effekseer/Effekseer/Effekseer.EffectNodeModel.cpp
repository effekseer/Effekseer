#include "Utils/Effekseer.BinaryReader.h"
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

void EffectNodeModel::LoadRendererParameter(BinaryReader<true>& pos, const SettingRef& setting)
{
	EffectNodeType type = EffectNodeType::NoneType;
	if (!pos.Peek(&type, sizeof(int)))
	{
		return pos.MarkFailed();
	}
	pos.Skip(sizeof(int));
	if (type != GetType())
	{
		return pos.MarkFailed();
	}
	EffekseerPrintDebug("Renderer : Model\n");

	if (m_effect->GetVersion() >= Version16Alpha3)
	{
		if (!pos.Peek(&Mode, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));
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
			if (!pos.Peek(&Magnification, sizeof(float)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(float));
		}

		if (!pos.Peek(&ModelIndex, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));

		if (m_effect->GetVersion() < 15)
		{
			int NormalTextureIndex = 0;
			if (!pos.Peek(&NormalTextureIndex, sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));
			EffekseerPrintDebug("NormalTextureIndex : %d\n", NormalTextureIndex);
			RendererCommon.TextureIndexes[1] = NormalTextureIndex;
			RendererCommon.BasicParameter.TextureIndexes[1] = NormalTextureIndex;
			RendererCommon.BasicParameter.TextureFilters[1] = RendererCommon.BasicParameter.TextureFilters[0];
			RendererCommon.BasicParameter.TextureWraps[1] = RendererCommon.BasicParameter.TextureWraps[0];
		}
	}
	else if (Mode == ModelReferenceType::Procedural)
	{
		if (!pos.Peek(&ModelIndex, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));
	}
	else if (Mode == ModelReferenceType::External)
	{
		if (m_effect->GetVersion() >= Version18Alpha3)
		{
			if (!pos.Peek(&ModelIndex, sizeof(int)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(int));
		}
	}

	if (m_effect->GetVersion() >= 12)
	{
		if (!pos.Peek(&Billboard, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));
	}
	else
	{
		Billboard = BillboardType::Fixed;
	}

	if (m_effect->GetVersion() < 15)
	{
		int32_t lighting;
		if (!pos.Peek(&lighting, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));
		const auto Lighting = lighting > 0;

		if (Lighting && !RendererCommon.Distortion)
		{
			RendererCommon.MaterialType = RendererMaterialType::Lighting;
			RendererCommon.BasicParameter.MaterialType = RendererMaterialType::Lighting;
		}
	}

	if (!pos.Peek(&Culling, sizeof(int)))
	{
		return pos.MarkFailed();
	}
	pos.Skip(sizeof(int));

	AllColor.load(pos, m_effect->GetVersion());

	if (Version16Alpha3 > m_effect->GetVersion() && m_effect->GetVersion() >= Version16Alpha1)
	{
		int FalloffFlag = 0;
		if (!pos.Peek(&FalloffFlag, sizeof(int)))
		{
			return pos.MarkFailed();
		}
		pos.Skip(sizeof(int));
		EnableFalloff = (FalloffFlag == 1);

		if (EnableFalloff)
		{
			if (!pos.Peek(&FalloffParam, sizeof(FalloffParameter)))
			{
				return pos.MarkFailed();
			}
			pos.Skip(sizeof(FalloffParameter));
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
		instanceParameter.Time = (int32_t)instance.livingTime_;

		instanceParameter.UV = instance.GetUV(0);
		instanceParameter.AlphaUV = instance.GetUV(1);
		instanceParameter.UVDistortionUV = instance.GetUV(2);
		instanceParameter.BlendUV = instance.GetUV(3);
		instanceParameter.BlendAlphaUV = instance.GetUV(4);
		instanceParameter.BlendUVDistortionUV = instance.GetUV(5);

		instanceParameter.FlipbookIndexAndNextRate = instance.GetFlipbookIndexAndNextRate();

		instanceParameter.AlphaThreshold = instance.alphaThreshold_;

		if (nodeParam_.EnableViewOffset)
		{
			instanceParameter.ViewOffsetDistance = instance.translation_state_.view_offset.distance;
		}

		if (nodeParam_.Billboard == BillboardType::DirectionalBillboard)
		{
			instanceParameter.Direction = instance.GetGlobalDirection();
		}

		instanceParameter.ParticleTimes[0] = instance.GetNormalizedLivetime();
		instanceParameter.ParticleTimes[1] = instance.livingTime_ / 60.0f;

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
	instValues._original = AllTypeColorFunctions::Calculate(instValues.allColorValues, AllColor, instance.livingTime_, instance.livedTime_);

	ApplyRendererCommonUVHorizontalFlip(instance, rand);

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

	instValues._original = AllTypeColorFunctions::Calculate(instValues.allColorValues, AllColor, instance.livingTime_, instance.livedTime_);

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

EffectModelParameter EffectNodeModel::GetEffectModelParameter()
{
	EffectModelParameter param = {};

	param.ModelIndex = ModelIndex;
	param.Culling = Culling;

	return param;
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
	nodeParameter.IsExternalMode = Mode == ModelReferenceType::External;

	if (nodeParameter.IsExternalMode && global != nullptr)
	{
		const auto& externalModels = global->GetExternalModels();
		if (0 <= ModelIndex && ModelIndex < static_cast<int32_t>(externalModels.size()))
		{
			nodeParameter.ExternalModel = externalModels[ModelIndex].Model;
			nodeParameter.ExternalModelTransform = externalModels[ModelIndex].Transform;
		}
	}

	nodeParameter.UserData = GetRenderingUserData();
	nodeParameter.RenderingCoordinateTransform = global->RenderingCoordinateTransform;
	nodeParameter.RenderingTransform = global->RenderingTransform;

	return nodeParameter;
}

} // namespace Effekseer
