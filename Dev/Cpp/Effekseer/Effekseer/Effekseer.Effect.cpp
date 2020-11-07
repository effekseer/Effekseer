﻿
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Effect.h"
#include "Effekseer.CurveLoader.h"
#include "Effekseer.DefaultEffectLoader.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.EffectLoader.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Manager.h"
#include "Effekseer.ManagerImplemented.h"
#include "Effekseer.MaterialLoader.h"
#include "Effekseer.Setting.h"
#include "Effekseer.SoundLoader.h"
#include "Effekseer.TextureLoader.h"
#include "Model/ModelLoader.h"
#include "Model/ProcedualModelGenerator.h"
#include "Model/ProcedualModelParameter.h"
#include "Utils/Effekseer.BinaryReader.h"

#include <array>
#include <functional>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

static void PathCombine(char16_t* dst, const char16_t* src1, const char16_t* src2)
{
	int len1 = 0, len2 = 0;
	if (src1 != nullptr)
	{
		for (len1 = 0; src1[len1] != L'\0'; len1++)
		{
		}
		memcpy(dst, src1, len1 * sizeof(char16_t));
		if (len1 > 0 && src1[len1 - 1] != L'/' && src1[len1 - 1] != L'\\')
		{
			dst[len1++] = L'/';
		}
	}
	if (src2 != nullptr)
	{
		for (len2 = 0; src2[len2] != L'\0'; len2++)
		{
		}
		memcpy(&dst[len1], src2, len2 * sizeof(char16_t));
	}

	for (int i = 0; i < len1 + len2; i++)
	{
		if (dst[i] == u'\\')
		{
			dst[i] = u'/';
		}
	}

	dst[len1 + len2] = L'\0';
}

static void GetParentDir(char16_t* dst, const char16_t* src)
{
	int i, last = -1;
	for (i = 0; src[i] != L'\0'; i++)
	{
		if (src[i] == L'/' || src[i] == L'\\')
			last = i;
	}
	if (last >= 0)
	{
		memcpy(dst, src, last * sizeof(char16_t));
		dst[last] = L'\0';
	}
	else
	{
		dst[0] = L'\0';
	}
}

static std::u16string getFilenameWithoutExt(const char16_t* path)
{
	int start = 0;
	int end = 0;

	for (int i = 0; path[i] != 0; i++)
	{
		if (path[i] == u'/' || path[i] == u'\\')
		{
			start = i;
		}
	}

	for (int i = start; path[i] != 0; i++)
	{
		if (path[i] == u'.')
		{
			end = i;
		}
	}

	std::vector<char16_t> ret;

	for (int i = start; i < end; i++)
	{
		ret.push_back(path[i]);
	}
	ret.push_back(0);

	return std::u16string(ret.data());
}

bool EffectFactory::LoadBody(Effect* effect, const void* data, int32_t size, float magnification, const char16_t* materialPath)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);
	auto data_ = static_cast<const uint8_t*>(data);
	return effect_->LoadBody(data_, size, magnification);
}

void EffectFactory::SetTexture(Effect* effect, int32_t index, TextureType type, TextureData* data)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);

	if (type == TextureType::Color)
	{
		assert(0 <= index && index < effect_->m_ImageCount);
		effect_->m_pImages[index] = data;
	}

	if (type == TextureType::Normal)
	{
		assert(0 <= index && index < effect_->m_normalImageCount);
		effect_->m_normalImages[index] = data;
	}

	if (type == TextureType::Distortion)
	{
		assert(0 <= index && index < effect_->m_distortionImageCount);
		effect_->m_distortionImages[index] = data;
	}
}

void EffectFactory::SetSound(Effect* effect, int32_t index, void* data)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);

	assert(0 <= index && index < effect_->m_WaveCount);
	effect_->m_pWaves[index] = data;
}

void EffectFactory::SetModel(Effect* effect, int32_t index, Model* data)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);
	assert(0 <= index && index < effect_->models_.size());
	effect_->models_[index] = data;
}

void EffectFactory::SetMaterial(Effect* effect, int32_t index, MaterialData* data)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);
	assert(0 <= index && index < effect_->materialCount_);
	effect_->materials_[index] = data;
}

void EffectFactory::SetCurve(Effect* effect, int32_t index, void* data)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);
	assert(0 <= index && index < effect_->curveCount_);
	effect_->curves_[index] = data;
}

void EffectFactory::SetProcedualModel(Effect* effect, int32_t index, Model* data)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);
	assert(0 <= index && index < effect_->procedualModels_.size());
	effect_->procedualModels_[index] = data;
}

void EffectFactory::SetLoadingParameter(Effect* effect, ReferenceObject* parameter)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);
	effect_->SetLoadingParameter(parameter);
}

bool EffectFactory::OnCheckIsBinarySupported(const void* data, int32_t size)
{
	// EFKS
	int head = 0;
	memcpy(&head, data, sizeof(int));
	if (memcmp(&head, "SKFE", 4) != 0)
		return false;
	return true;
}

bool EffectFactory::OnCheckIsReloadSupported()
{
	return true;
}

bool EffectFactory::OnLoading(Effect* effect, const void* data, int32_t size, float magnification, const char16_t* materialPath)
{
	return this->LoadBody(effect, data, size, magnification, materialPath);
}

void EffectFactory::OnLoadingResource(Effect* effect, const void* data, int32_t size, const char16_t* materialPath)
{
	auto textureLoader = effect->GetSetting()->GetTextureLoader();
	auto soundLoader = effect->GetSetting()->GetSoundLoader();
	auto modelLoader = effect->GetSetting()->GetModelLoader();
	auto materialLoader = effect->GetSetting()->GetMaterialLoader();
	auto curveLoader = effect->GetSetting()->GetCurveLoader();
	auto pmGenerator = effect->GetSetting()->GetProcedualMeshGenerator();

	if (textureLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetColorImageCount(); i++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetColorImagePath(i));

			auto resource = textureLoader->Load(fullPath, TextureType::Color);
			SetTexture(effect, i, TextureType::Color, resource);
		}

		for (auto i = 0; i < effect->GetNormalImageCount(); i++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetNormalImagePath(i));

			auto resource = textureLoader->Load(fullPath, TextureType::Normal);
			SetTexture(effect, i, TextureType::Normal, resource);
		}

		for (auto i = 0; i < effect->GetDistortionImageCount(); i++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetDistortionImagePath(i));

			auto resource = textureLoader->Load(fullPath, TextureType::Distortion);
			SetTexture(effect, i, TextureType::Distortion, resource);
		}
	}

	if (soundLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetWaveCount(); i++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetWavePath(i));

			auto resource = soundLoader->Load(fullPath);
			SetSound(effect, i, resource);
		}
	}

	if (modelLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetModelCount(); i++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetModelPath(i));

			auto resource = modelLoader->Load(fullPath);
			SetModel(effect, i, resource);
		}
	}

	if (materialLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetMaterialCount(); i++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetMaterialPath(i));

			auto resource = materialLoader->Load(fullPath);
			SetMaterial(effect, i, resource);
		}
	}

	if (curveLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetCurveCount(); i++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetCurvePath(i));

			auto resource = curveLoader->Load(fullPath);
			SetCurve(effect, i, resource);
		}
	}

	if (pmGenerator != nullptr)
	{
		for (int32_t ind = 0; ind < effect->GetProcedualModelCount(); ind++)
		{
			auto model = pmGenerator->Generate(effect->GetProcedualModelParameter(ind));
			SetProcedualModel(effect, ind, model);
		}
	}
}

void EffectFactory::OnUnloadingResource(Effect* effect)
{
	auto textureLoader = effect->GetSetting()->GetTextureLoader();
	auto soundLoader = effect->GetSetting()->GetSoundLoader();
	auto modelLoader = effect->GetSetting()->GetModelLoader();
	auto materialLoader = effect->GetSetting()->GetMaterialLoader();
	auto curveLoader = effect->GetSetting()->GetCurveLoader();
	auto pmGenerator = effect->GetSetting()->GetProcedualMeshGenerator();

	if (textureLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetColorImageCount(); i++)
		{
			textureLoader->Unload(effect->GetColorImage(i));
			SetTexture(effect, i, TextureType::Color, nullptr);
		}

		for (auto i = 0; i < effect->GetNormalImageCount(); i++)
		{
			textureLoader->Unload(effect->GetNormalImage(i));
			SetTexture(effect, i, TextureType::Normal, nullptr);
		}

		for (auto i = 0; i < effect->GetDistortionImageCount(); i++)
		{
			textureLoader->Unload(effect->GetDistortionImage(i));
			SetTexture(effect, i, TextureType::Distortion, nullptr);
		}
	}

	if (soundLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetWaveCount(); i++)
		{
			soundLoader->Unload(effect->GetWave(i));
			SetSound(effect, i, nullptr);
		}
	}

	if (modelLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetModelCount(); i++)
		{
			modelLoader->Unload(effect->GetModel(i));
			SetModel(effect, i, nullptr);
		}
	}

	if (materialLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetMaterialCount(); i++)
		{
			materialLoader->Unload(effect->GetMaterial(i));
			SetMaterial(effect, i, nullptr);
		}
	}

	if (curveLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetCurveCount(); i++)
		{
			curveLoader->Unload(effect->GetCurve(i));
			SetCurve(effect, i, nullptr);
		}
	}

	if (pmGenerator != nullptr)
	{
		for (int32_t ind = 0; ind < effect->GetProcedualModelCount(); ind++)
		{
			pmGenerator->Ungenerate(effect->GetProcedualModel(ind));
			SetProcedualModel(effect, ind, nullptr);
		}
	}
}

const char* EffectFactory::GetName() const
{
	static const char* name = "Default";
	return name;
}

bool EffectFactory::GetIsResourcesLoadedAutomatically() const
{
	return true;
}

EffectFactory::EffectFactory()
{
}

EffectFactory::~EffectFactory()
{
}

EffectRef Effect::Create(Manager* manager, void* data, int32_t size, float magnification, const char16_t* materialPath)
{
	return EffectImplemented::Create(manager, data, size, magnification, materialPath);
}

EffectRef Effect::Create(Manager* manager, const char16_t* path, float magnification, const char16_t* materialPath)
{
	auto setting = manager->GetSetting();

	EffectLoader* eLoader = setting->GetEffectLoader();

	if (setting == nullptr)
		return nullptr;

	void* data = nullptr;
	int32_t size = 0;

	if (!eLoader->Load(path, data, size))
		return nullptr;

	char16_t parentDir[512];
	if (materialPath == nullptr)
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	auto effect = EffectImplemented::Create(manager, data, size, magnification, materialPath);

	eLoader->Unload(data, size);

	effect->SetName(getFilenameWithoutExt(path).c_str());

	return effect;
}

bool EffectImplemented::LoadBody(const uint8_t* data, int32_t size, float mag)
{
	// TODO share with an editor
	const int32_t elementCountMax = 1024;
	const int32_t dynamicBinaryCountMax = 102400;

	uint8_t* pos = const_cast<uint8_t*>(data);

	BinaryReader<true> binaryReader(const_cast<uint8_t*>(data), size);

	// EFKS
	int head = 0;
	binaryReader.Read(head);
	if (memcmp(&head, "SKFE", 4) != 0)
		return false;

	binaryReader.Read(m_version);

	// too new version
	if (m_version > SupportBinaryVersion)
	{
		return false;
	}

	// Image
	binaryReader.Read(m_ImageCount, 0, elementCountMax);

	if (m_ImageCount > 0)
	{
		m_ImagePaths = new char16_t*[m_ImageCount];
		m_pImages = new TextureData*[m_ImageCount];

		for (int i = 0; i < m_ImageCount; i++)
		{
			int length = 0;
			binaryReader.Read(length, 0, elementCountMax);

			m_ImagePaths[i] = new char16_t[length];
			binaryReader.Read(m_ImagePaths[i], length);

			m_pImages[i] = nullptr;
		}
	}

	if (m_version >= 9)
	{
		// Image
		binaryReader.Read(m_normalImageCount, 0, elementCountMax);

		if (m_normalImageCount > 0)
		{
			m_normalImagePaths = new char16_t*[m_normalImageCount];
			m_normalImages = new TextureData*[m_normalImageCount];

			for (int i = 0; i < m_normalImageCount; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				m_normalImagePaths[i] = new char16_t[length];
				binaryReader.Read(m_normalImagePaths[i], length);

				m_normalImages[i] = nullptr;
			}
		}

		// Image
		binaryReader.Read(m_distortionImageCount, 0, elementCountMax);

		if (m_distortionImageCount > 0)
		{
			m_distortionImagePaths = new char16_t*[m_distortionImageCount];
			m_distortionImages = new TextureData*[m_distortionImageCount];

			for (int i = 0; i < m_distortionImageCount; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				m_distortionImagePaths[i] = new char16_t[length];
				binaryReader.Read(m_distortionImagePaths[i], length);

				m_distortionImages[i] = nullptr;
			}
		}
	}

	if (m_version >= 1)
	{
		// Sound
		binaryReader.Read(m_WaveCount, 0, elementCountMax);

		if (m_WaveCount > 0)
		{
			m_WavePaths = new char16_t*[m_WaveCount];
			m_pWaves = new void*[m_WaveCount];

			for (int i = 0; i < m_WaveCount; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				m_WavePaths[i] = new char16_t[length];
				binaryReader.Read(m_WavePaths[i], length);

				m_pWaves[i] = nullptr;
			}
		}
	}

	if (m_version >= 6)
	{
		// Model
		int32_t modelCount = 0;
		binaryReader.Read(modelCount, 0, elementCountMax);

		if (modelCount > 0)
		{
			modelPaths_.resize(modelCount);
			models_.resize(modelCount);

			for (size_t i = 0; i < models_.size(); i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				modelPaths_[i] = new char16_t[length];
				binaryReader.Read(modelPaths_[i], length);

				models_[i] = nullptr;
			}
		}
	}

	if (m_version >= 15)
	{
		// material
		binaryReader.Read(materialCount_, 0, elementCountMax);

		if (materialCount_ > 0)
		{
			materialPaths_ = new char16_t*[materialCount_];
			materials_ = new MaterialData*[materialCount_];

			for (int i = 0; i < materialCount_; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				materialPaths_[i] = new char16_t[length];
				binaryReader.Read(materialPaths_[i], length);

				materials_[i] = nullptr;
			}
		}
	}

	if (m_version >= 14)
	{
		// inputs
		defaultDynamicInputs.fill(0);
		int32_t dynamicInputCount = 0;
		binaryReader.Read(dynamicInputCount, 0, elementCountMax);

		for (size_t i = 0; i < dynamicInputCount; i++)
		{
			float param = 0.0f;
			binaryReader.Read(param);

			if (i < defaultDynamicInputs.size())
			{
				defaultDynamicInputs[i] = param;
			}
		}

		// dynamic parameter
		int32_t dynamicEquationCount = 0;
		binaryReader.Read(dynamicEquationCount, 0, elementCountMax);

		if (dynamicEquationCount > 0)
		{
			dynamicEquation.resize(dynamicEquationCount);

			for (size_t dp = 0; dp < dynamicEquation.size(); dp++)
			{
				int size_ = 0;
				binaryReader.Read(size_, 0, dynamicBinaryCountMax);

				auto data_ = pos + binaryReader.GetOffset();
				dynamicEquation[dp].Load(data_, size_);

				binaryReader.AddOffset(size_);
			}
		}
	}
	else
	{
		defaultDynamicInputs.fill(0);
	}

	if (m_version >= Version16Alpha1)
	{
		// curve
		binaryReader.Read(curveCount_, 0, elementCountMax);

		if (curveCount_ > 0)
		{
			curvePaths_ = new char16_t*[curveCount_];
			curves_ = new void*[curveCount_];

			for (int i = 0; i < curveCount_; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				curvePaths_[i] = new char16_t[length];
				binaryReader.Read(curvePaths_[i], length);

				curves_[i] = nullptr;
			}
		}

		// procedual material
		int32_t pmCount = 0;

		binaryReader.Read(pmCount, 0, elementCountMax);

		procedualModelParameters_.resize(pmCount);
		procedualModels_.resize(pmCount);

		for (int32_t i = 0; i < pmCount; i++)
		{
			procedualModelParameters_[i].Load(binaryReader);
			procedualModels_[i] = nullptr;
		}
	}

	if (m_version >= 13)
	{
		binaryReader.Read(renderingNodesCount, 0, elementCountMax);
		binaryReader.Read(renderingNodesThreshold, 0, elementCountMax);
	}

	// magnification
	if (m_version >= 2)
	{
		binaryReader.Read(m_maginification);
	}

	m_maginification *= mag;
	m_maginificationExternal = mag;

	if (m_version >= 11)
	{
		binaryReader.Read(m_defaultRandomSeed);
	}
	else
	{
		m_defaultRandomSeed = -1;
	}

	// Culling
	if (m_version >= 9)
	{
		binaryReader.Read(Culling.Shape);
		if (Culling.Shape == CullingShape::Sphere)
		{
			binaryReader.Read(Culling.Sphere.Radius);
			binaryReader.Read(Culling.Location.X);
			binaryReader.Read(Culling.Location.Y);
			binaryReader.Read(Culling.Location.Z);

			Culling.Sphere.Radius *= m_maginification;
			Culling.Location.X *= m_maginification;
			Culling.Location.Y *= m_maginification;
			Culling.Location.Z *= m_maginification;
		}
	}

	// Check
	if (binaryReader.GetStatus() == BinaryReaderStatus::Failed)
		return false;

	// Nodes
	auto nodeData = pos + binaryReader.GetOffset();
	m_pRoot = EffectNodeImplemented::Create(this, nullptr, nodeData);

	return true;
}

void EffectImplemented::ResetReloadingBackup()
{
	if (reloadingBackup == nullptr)
		return;

	auto loader = GetSetting();

	TextureLoader* textureLoader = loader->GetTextureLoader();
	if (textureLoader != nullptr)
	{
		for (auto it : reloadingBackup->images.GetCollection())
		{
			textureLoader->Unload(it.second.value);
		}

		for (auto it : reloadingBackup->normalImages.GetCollection())
		{
			textureLoader->Unload(it.second.value);
		}

		for (auto it : reloadingBackup->distortionImages.GetCollection())
		{
			textureLoader->Unload(it.second.value);
		}
	}

	SoundLoader* soundLoader = loader->GetSoundLoader();
	if (soundLoader != nullptr)
	{
		for (auto it : reloadingBackup->sounds.GetCollection())
		{
			soundLoader->Unload(it.second.value);
		}
	}

	{
		ModelLoader* modelLoader = loader->GetModelLoader();
		if (modelLoader != nullptr)
		{
			for (auto it : reloadingBackup->models.GetCollection())
			{
				modelLoader->Unload(it.second.value);
			}
		}
	}

	reloadingBackup.reset();
}

EffectRef EffectImplemented::Create(Manager* pManager, void* pData, int size, float magnification, const char16_t* materialPath)
{
	if (pData == nullptr || size == 0)
		return nullptr;

	auto effect = MakeRefPtr<EffectImplemented>(pManager, pData, size);
	if (!effect->Load(pData, size, magnification, materialPath, ReloadingThreadType::Main))
	{
		effect->Release();
		effect = nullptr;
	}
	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectRef Effect::Create(const RefPtr<Setting>& setting, void* data, int32_t size, float magnification, const char16_t* materialPath)
{
	return EffectImplemented::Create(setting, data, size, magnification, materialPath);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectRef Effect::Create(const RefPtr<Setting>& setting, const char16_t* path, float magnification, const char16_t* materialPath)
{
	if (setting == nullptr)
		return nullptr;
	EffectLoader* eLoader = setting->GetEffectLoader();

	if (setting == nullptr)
		return nullptr;

	void* data = nullptr;
	int32_t size = 0;

	if (!eLoader->Load(path, data, size))
		return nullptr;

	char16_t parentDir[512];
	if (materialPath == nullptr)
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	auto effect = EffectImplemented::Create(setting, data, size, magnification, materialPath);

	eLoader->Unload(data, size);

	effect->SetName(getFilenameWithoutExt(path).c_str());

	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectRef EffectImplemented::Create(const RefPtr<Setting>& setting, void* pData, int size, float magnification, const char16_t* materialPath)
{
	if (pData == nullptr || size == 0)
		return nullptr;

	auto effect = MakeRefPtr<EffectImplemented>(setting, pData, size);
	if (!effect->Load(pData, size, magnification, materialPath, ReloadingThreadType::Main))
	{
		effect = nullptr;
	}
	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::EffectLoader* Effect::CreateEffectLoader(::Effekseer::FileInterface* fileInterface)
{
	return new ::Effekseer::DefaultEffectLoader(fileInterface);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectImplemented::EffectImplemented(Manager* pManager, void* pData, int size)
	: m_setting(pManager->GetSetting())
	, m_reference(1)
	, m_version(0)
	, m_ImageCount(0)
	, m_ImagePaths(nullptr)
	, m_pImages(nullptr)
	, m_normalImageCount(0)
	, m_normalImagePaths(nullptr)
	, m_normalImages(nullptr)
	, m_distortionImageCount(0)
	, m_distortionImagePaths(nullptr)
	, m_distortionImages(nullptr)
	, m_defaultRandomSeed(-1)

{
	Culling.Shape = CullingShape::NoneShape;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectImplemented::EffectImplemented(const RefPtr<Setting>& setting, void* pData, int size)
	: m_setting(setting)
	, m_reference(1)
	, m_version(0)
	, m_ImageCount(0)
	, m_ImagePaths(nullptr)
	, m_pImages(nullptr)
	, m_normalImageCount(0)
	, m_normalImagePaths(nullptr)
	, m_normalImages(nullptr)
	, m_distortionImageCount(0)
	, m_distortionImagePaths(nullptr)
	, m_distortionImages(nullptr)
{
	Culling.Shape = CullingShape::NoneShape;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectImplemented::~EffectImplemented()
{
	ResetReloadingBackup();
	Reset();
	SetLoadingParameter(nullptr);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectNode* EffectImplemented::GetRoot() const
{
	return m_pRoot;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
float EffectImplemented::GetMaginification() const
{
	return m_maginification;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Load(void* pData, int size, float mag, const char16_t* materialPath, ReloadingThreadType reloadingThreadType)
{
	factory.Reset();

	if (m_setting != nullptr)
	{
		for (int i = 0; i < m_setting->GetEffectFactoryCount(); i++)
		{
			auto f = m_setting->GetEffectFactory(i);

			if (f->OnCheckIsBinarySupported(pData, size))
			{
				factory = f;
				break;
			}
		}
	}

	if (factory == nullptr)
		return false;

	// if reladingThreadType == ReloadingThreadType::Main, this function was regarded as loading function actually

	if (!factory->OnCheckIsBinarySupported(pData, size))
	{
		return false;
	}

	EffekseerPrintDebug("** Create : Effect\n");

	if (!factory->OnLoading(this, pData, size, mag, materialPath))
	{
		return false;
	}

	// save materialPath for reloading
	if (materialPath != nullptr)
		m_materialPath = materialPath;

	if (factory->GetIsResourcesLoadedAutomatically())
	{
		ReloadResources(pData, size, materialPath);
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectImplemented::Reset()
{
	UnloadResources();

	for (int i = 0; i < m_ImageCount; i++)
	{
		if (m_ImagePaths[i] != nullptr)
			delete[] m_ImagePaths[i];
	}

	m_ImageCount = 0;

	ES_SAFE_DELETE_ARRAY(m_ImagePaths);
	ES_SAFE_DELETE_ARRAY(m_pImages);

	{
		for (int i = 0; i < m_normalImageCount; i++)
		{
			if (m_normalImagePaths[i] != nullptr)
				delete[] m_normalImagePaths[i];
		}

		m_normalImageCount = 0;

		ES_SAFE_DELETE_ARRAY(m_normalImagePaths);
		ES_SAFE_DELETE_ARRAY(m_normalImages);
	}

	{
		for (int i = 0; i < m_distortionImageCount; i++)
		{
			if (m_distortionImagePaths[i] != nullptr)
				delete[] m_distortionImagePaths[i];
		}

		m_distortionImageCount = 0;

		ES_SAFE_DELETE_ARRAY(m_distortionImagePaths);
		ES_SAFE_DELETE_ARRAY(m_distortionImages);
	}

	for (int i = 0; i < m_WaveCount; i++)
	{
		if (m_WavePaths[i] != nullptr)
			delete[] m_WavePaths[i];
	}
	m_WaveCount = 0;

	ES_SAFE_DELETE_ARRAY(m_WavePaths);
	ES_SAFE_DELETE_ARRAY(m_pWaves);

	for (size_t i = 0; i < models_.size(); i++)
	{
		if (modelPaths_[i] != nullptr)
			delete[] modelPaths_[i];
	}

	models_.clear();
	modelPaths_.clear();

	for (int i = 0; i < materialCount_; i++)
	{
		if (materialPaths_[i] != nullptr)
			delete[] materialPaths_[i];
	}
	materialCount_ = 0;

	ES_SAFE_DELETE_ARRAY(materialPaths_);
	ES_SAFE_DELETE_ARRAY(materials_);

	ES_SAFE_DELETE(m_pRoot);
}

bool EffectImplemented::IsDyanamicMagnificationValid() const
{
	return GetVersion() >= 8 || GetVersion() < 2;
}

ReferenceObject* EffectImplemented::GetLoadingParameter() const
{
	return loadingObject;
}

void EffectImplemented::SetLoadingParameter(ReferenceObject* obj)
{
	ES_SAFE_ADDREF(obj);
	ES_SAFE_RELEASE(loadingObject);
	loadingObject = obj;
}

const char16_t* EffectImplemented::GetName() const
{
	return name_.c_str();
}

void EffectImplemented::SetName(const char16_t* name)
{
	name_ = name;
}

RefPtr<Setting> EffectImplemented::GetSetting() const
{
	return m_setting;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int EffectImplemented::GetVersion() const
{
	return m_version;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureData* EffectImplemented::GetColorImage(int n) const
{
	if (n < 0 || n >= GetColorImageCount())
	{
		return nullptr;
	}

	return m_pImages[n];
}

int32_t EffectImplemented::GetColorImageCount() const
{
	return m_ImageCount;
}

const char16_t* EffectImplemented::GetColorImagePath(int n) const
{
	return m_ImagePaths[n];
}

TextureData* EffectImplemented::GetNormalImage(int n) const
{
	/* 強制的に互換をとる */
	if (this->m_version <= 8)
	{
		return m_pImages[n];
	}

	if (n < 0 || n >= GetNormalImageCount())
	{
		return nullptr;
	}

	return m_normalImages[n];
}

int32_t EffectImplemented::GetNormalImageCount() const
{
	return m_normalImageCount;
}

const char16_t* EffectImplemented::GetNormalImagePath(int n) const
{
	return m_normalImagePaths[n];
}

TextureData* EffectImplemented::GetDistortionImage(int n) const
{
	/* 強制的に互換をとる */
	if (this->m_version <= 8)
	{
		return m_pImages[n];
	}

	if (n < 0 || n >= GetDistortionImageCount())
	{
		return nullptr;
	}

	return m_distortionImages[n];
}

int32_t EffectImplemented::GetDistortionImageCount() const
{
	return m_distortionImageCount;
}

const char16_t* EffectImplemented::GetDistortionImagePath(int n) const
{
	return m_distortionImagePaths[n];
}

void* EffectImplemented::GetWave(int n) const
{
	return m_pWaves[n];
}

int32_t EffectImplemented::GetWaveCount() const
{
	return m_WaveCount;
}

const char16_t* EffectImplemented::GetWavePath(int n) const
{
	return m_WavePaths[n];
}

Model* EffectImplemented::GetModel(int n) const
{
	if (n < 0 || n >= GetModelCount())
	{
		return nullptr;
	}

	return models_[n];
}

int32_t EffectImplemented::GetModelCount() const
{
	return static_cast<int32_t>(models_.size());
}

const char16_t* EffectImplemented::GetModelPath(int n) const
{
	return modelPaths_[n];
}

MaterialData* EffectImplemented::GetMaterial(int n) const
{
	if (n < 0 || n >= GetMaterialCount())
	{
		return nullptr;
	}

	return materials_[n];
}

int32_t EffectImplemented::GetMaterialCount() const
{
	return materialCount_;
}

const char16_t* EffectImplemented::GetMaterialPath(int n) const
{
	return materialPaths_[n];
}

void* EffectImplemented::GetCurve(int n) const
{
	return curves_[n];
}

int32_t EffectImplemented::GetCurveCount() const
{
	return curveCount_;
}

const char16_t* EffectImplemented::GetCurvePath(int n) const
{
	return curvePaths_[n];
}

Model* EffectImplemented::GetProcedualModel(int n) const
{
	if (n < 0 || n >= GetProcedualModelCount())
	{
		return nullptr;
	}

	return procedualModels_[n];
}

int32_t EffectImplemented::GetProcedualModelCount() const
{
	return static_cast<int32_t>(procedualModelParameters_.size());
}

const ProcedualModelParameter* EffectImplemented::GetProcedualModelParameter(int n) const
{
	if (n < 0 || n >= GetProcedualModelCount())
	{
		return nullptr;
	}

	return &procedualModelParameters_[n];
}

void EffectImplemented::SetTexture(int32_t index, TextureType type, TextureData* data)
{
	auto textureLoader = GetSetting()->GetTextureLoader();

	if (type == TextureType::Color)
	{
		assert(0 <= index && index < m_ImageCount);
		if (textureLoader != nullptr)
		{
			textureLoader->Unload(GetColorImage(index));
		}

		m_pImages[index] = data;
	}

	if (type == TextureType::Normal)
	{
		assert(0 <= index && index < m_normalImageCount);
		if (textureLoader != nullptr)
		{
			textureLoader->Unload(GetNormalImage(index));
		}

		m_normalImages[index] = data;
	}

	if (type == TextureType::Distortion)
	{
		assert(0 <= index && index < m_distortionImageCount);
		if (textureLoader != nullptr)
		{
			textureLoader->Unload(GetDistortionImage(index));
		}

		m_distortionImages[index] = data;
	}
}

void EffectImplemented::SetSound(int32_t index, void* data)
{
	auto soundLoader = GetSetting()->GetSoundLoader();
	assert(0 <= index && index < m_WaveCount);

	if (soundLoader != nullptr)
	{
		soundLoader->Unload(GetWave(index));
	}

	m_pWaves[index] = data;
}

void EffectImplemented::SetModel(int32_t index, Model* data)
{
	auto modelLoader = GetSetting()->GetModelLoader();
	assert(0 <= index && index < models_.size());

	if (modelLoader != nullptr)
	{
		modelLoader->Unload(GetModel(index));
	}

	models_[index] = data;
}

void EffectImplemented::SetMaterial(int32_t index, MaterialData* data)
{
	auto materialLoader = GetSetting()->GetMaterialLoader();
	assert(0 <= index && index < materialCount_);

	if (materialLoader != nullptr)
	{
		materialLoader->Unload(GetMaterial(index));
	}

	materials_[index] = data;
}

void EffectImplemented::SetCurve(int32_t index, void* data)
{
	auto curveLoader = GetSetting()->GetCurveLoader();
	assert(0 <= index && index < curveCount_);

	if (curveLoader != nullptr)
	{
		curveLoader->Unload(GetCurve(index));
	}

	curves_[index] = data;
}

bool EffectImplemented::Reload(Manager** managers,
							   int32_t managersCount,
							   void* data,
							   int32_t size,
							   const char16_t* materialPath,
							   ReloadingThreadType reloadingThreadType)
{
	if (!factory->OnCheckIsReloadSupported())
		return false;

	const char16_t* matPath = materialPath != nullptr ? materialPath : m_materialPath.c_str();

	for (int32_t i = 0; i < managersCount; i++)
	{
		// to call only once
		for (int32_t j = 0; j < i; j++)
		{
			if (managers[i] == managers[j])
				continue;
		}

		auto manager = static_cast<ManagerImplemented*>(managers[i]);

		this->AddRef();
		auto temp = EffectRef(this);
		manager->BeginReloadEffect(temp, true);
	}

	// HACK for scale
	auto originalMag = this->GetMaginification() / this->m_maginificationExternal;
	auto originalMagExt = this->m_maginificationExternal;

	isReloadingOnRenderingThread = true;
	Reset();
	Load(data, size, originalMag * originalMagExt, matPath, reloadingThreadType);

	// HACK for scale
	m_maginification = originalMag * originalMagExt;
	m_maginificationExternal = originalMagExt;

	isReloadingOnRenderingThread = false;

	for (int32_t i = 0; i < managersCount; i++)
	{
		// to call only once
		for (int32_t j = 0; j < i; j++)
		{
			if (managers[i] == managers[j])
				continue;
		}

		auto manager = static_cast<ManagerImplemented*>(managers[i]);
		this->AddRef();
		auto temp = EffectRef(this);
		manager->EndReloadEffect(temp, true);
	}

	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Reload(
	Manager** managers, int32_t managersCount, const char16_t* path, const char16_t* materialPath, ReloadingThreadType reloadingThreadType)
{
	if (!factory->OnCheckIsReloadSupported())
		return false;

	auto loader = GetSetting();

	EffectLoader* eLoader = loader->GetEffectLoader();
	if (loader == nullptr)
		return false;

	void* data = nullptr;
	int32_t size = 0;

	if (!eLoader->Load(path, data, size))
		return false;

	char16_t parentDir[512];
	if (materialPath == nullptr)
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	int lockCount = 0;

	for (int32_t i = 0; i < managersCount; i++)
	{
		auto manager = static_cast<ManagerImplemented*>(managers[i]);

		this->AddRef();
		auto temp = EffectRef(this);
		manager->BeginReloadEffect(temp, lockCount == 0);
		lockCount++;
	}

	isReloadingOnRenderingThread = true;
	Reset();
	Load(data, size, m_maginificationExternal, materialPath, reloadingThreadType);
	isReloadingOnRenderingThread = false;

	for (int32_t i = 0; i < managersCount; i++)
	{
		lockCount--;
		auto manager = static_cast<ManagerImplemented*>(managers[i]);

		this->AddRef();
		auto temp = EffectRef(this);
		manager->EndReloadEffect(temp, lockCount == 0);
	}

	eLoader->Unload(data, size);

	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectImplemented::ReloadResources(const void* data, int32_t size, const char16_t* materialPath)
{
	UnloadResources();

	const char16_t* matPath = materialPath != nullptr ? materialPath : m_materialPath.c_str();

	auto loader = GetSetting();

	// reloading on render thread
	if (isReloadingOnRenderingThread)
	{
		assert(reloadingBackup != nullptr);

		for (int32_t ind = 0; ind < m_ImageCount; ind++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, matPath, m_ImagePaths[ind]);

			TextureData* value = nullptr;
			if (reloadingBackup->images.Pop(fullPath, value))
			{
				m_pImages[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < m_normalImageCount; ind++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, matPath, m_normalImagePaths[ind]);

			TextureData* value = nullptr;
			if (reloadingBackup->normalImages.Pop(fullPath, value))
			{
				m_normalImages[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < m_distortionImageCount; ind++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, matPath, m_distortionImagePaths[ind]);

			TextureData* value = nullptr;
			if (reloadingBackup->distortionImages.Pop(fullPath, value))
			{
				m_distortionImages[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < m_WaveCount; ind++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, matPath, m_WavePaths[ind]);

			void* value = nullptr;
			if (reloadingBackup->sounds.Pop(fullPath, value))
			{
				m_pWaves[ind] = value;
			}
		}

		for (size_t ind = 0; ind < models_.size(); ind++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, matPath, modelPaths_[ind]);

			Model* value = nullptr;
			if (reloadingBackup->models.Pop(fullPath, value))
			{
				models_[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < materialCount_; ind++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, matPath, materialPaths_[ind]);

			MaterialData* value = nullptr;
			if (reloadingBackup->materials.Pop(fullPath, value))
			{
				materials_[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < curveCount_; ind++)
		{
			char16_t fullPath[512];
			PathCombine(fullPath, matPath, curvePaths_[ind]);

			void* value = nullptr;
			if (reloadingBackup->curves.Pop(fullPath, value))
			{
				curves_[ind] = value;
			}
		}

		return;
	}

	factory->OnLoadingResource(this, data, size, matPath);
}

void EffectImplemented::UnloadResources(const char16_t* materialPath)
{
	auto loader = GetSetting();

	// reloading on render thread
	if (isReloadingOnRenderingThread)
	{
		if (reloadingBackup == nullptr)
		{
			reloadingBackup = std::unique_ptr<EffectReloadingBackup>(new EffectReloadingBackup());
		}

		const char16_t* matPath = materialPath != nullptr ? materialPath : m_materialPath.c_str();

		for (int32_t ind = 0; ind < m_ImageCount; ind++)
		{
			if (m_pImages[ind] == nullptr)
				continue;

			char16_t fullPath[512];
			PathCombine(fullPath, matPath, m_ImagePaths[ind]);
			reloadingBackup->images.Push(fullPath, m_pImages[ind]);
		}

		for (int32_t ind = 0; ind < m_normalImageCount; ind++)
		{
			if (m_normalImages[ind] == nullptr)
				continue;

			char16_t fullPath[512];
			PathCombine(fullPath, matPath, m_normalImagePaths[ind]);
			reloadingBackup->normalImages.Push(fullPath, m_normalImages[ind]);
		}

		for (int32_t ind = 0; ind < m_distortionImageCount; ind++)
		{
			if (m_distortionImagePaths[ind] == nullptr)
				continue;

			char16_t fullPath[512];
			PathCombine(fullPath, matPath, m_distortionImagePaths[ind]);
			reloadingBackup->distortionImages.Push(fullPath, m_distortionImages[ind]);
		}

		for (int32_t ind = 0; ind < m_WaveCount; ind++)
		{
			if (m_pWaves[ind] == nullptr)
				continue;

			char16_t fullPath[512];
			PathCombine(fullPath, matPath, m_WavePaths[ind]);
			reloadingBackup->sounds.Push(fullPath, m_pWaves[ind]);
		}

		for (size_t ind = 0; ind < models_.size(); ind++)
		{
			if (models_[ind] == nullptr)
				continue;

			char16_t fullPath[512];
			PathCombine(fullPath, matPath, modelPaths_[ind]);
			reloadingBackup->models.Push(fullPath, models_[ind]);
		}

		for (int32_t ind = 0; ind < materialCount_; ind++)
		{
			if (materials_[ind] == nullptr)
				continue;

			char16_t fullPath[512];
			PathCombine(fullPath, matPath, materialPaths_[ind]);
			reloadingBackup->materials.Push(fullPath, materials_[ind]);
		}

		for (int32_t ind = 0; ind < curveCount_; ind++)
		{
			if (curves_[ind] == nullptr)
				continue;

			char16_t fullPath[512];
			PathCombine(fullPath, matPath, curvePaths_[ind]);
			reloadingBackup->curves.Push(fullPath, curves_[ind]);
		}

		return;
	}
	else
	{
		ResetReloadingBackup();
	}

	if (factory != nullptr)
	{
		factory->OnUnloadingResource(this);
	}
}

void EffectImplemented::UnloadResources()
{
	UnloadResources(nullptr);
}

EffectTerm EffectImplemented::CalculateTerm() const
{

	EffectTerm effectTerm;
	effectTerm.TermMin = 0;
	effectTerm.TermMax = 0;

	auto root = GetRoot();
	EffectInstanceTerm rootTerm;

	std::function<void(EffectNode*, EffectInstanceTerm&)> recurse;
	recurse = [&effectTerm, &recurse](EffectNode* node, EffectInstanceTerm& term) -> void {
		for (int i = 0; i < node->GetChildrenCount(); i++)
		{
			auto cterm = node->GetChild(i)->CalculateInstanceTerm(term);
			effectTerm.TermMin = Max(effectTerm.TermMin, cterm.LastInstanceEndMin);
			effectTerm.TermMax = Max(effectTerm.TermMax, cterm.LastInstanceEndMax);

			recurse(node->GetChild(i), cterm);
		}
	};

	recurse(root, rootTerm);

	return effectTerm;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
