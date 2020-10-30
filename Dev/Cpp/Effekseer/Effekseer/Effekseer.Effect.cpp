
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Effect.h"
#include "Effekseer.DefaultEffectLoader.h"
#include "Effekseer.EffectImplemented.h"
#include "Effekseer.EffectLoader.h"
#include "Effekseer.EffectNode.h"
#include "Effekseer.Manager.h"
#include "Effekseer.ManagerImplemented.h"
#include "Effekseer.MaterialLoader.h"
#include "Effekseer.ModelLoader.h"
#include "Effekseer.Setting.h"
#include "Effekseer.SoundLoader.h"
#include "Effekseer.TextureLoader.h"
#include "Utils/Effekseer.BinaryReader.h"

#include <array>
#include <functional>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

static void PathCombine(EFK_CHAR* dst, const EFK_CHAR* src1, const EFK_CHAR* src2)
{
	int len1 = 0, len2 = 0;
	if (src1 != NULL)
	{
		for (len1 = 0; src1[len1] != L'\0'; len1++)
		{
		}
		memcpy(dst, src1, len1 * sizeof(EFK_CHAR));
		if (len1 > 0 && src1[len1 - 1] != L'/' && src1[len1 - 1] != L'\\')
		{
			dst[len1++] = L'/';
		}
	}
	if (src2 != NULL)
	{
		for (len2 = 0; src2[len2] != L'\0'; len2++)
		{
		}
		memcpy(&dst[len1], src2, len2 * sizeof(EFK_CHAR));
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

static void GetParentDir(EFK_CHAR* dst, const EFK_CHAR* src)
{
	int i, last = -1;
	for (i = 0; src[i] != L'\0'; i++)
	{
		if (src[i] == L'/' || src[i] == L'\\')
			last = i;
	}
	if (last >= 0)
	{
		memcpy(dst, src, last * sizeof(EFK_CHAR));
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

bool EffectFactory::LoadBody(Effect* effect, const void* data, int32_t size, float magnification, const EFK_CHAR* materialPath)
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

void EffectFactory::SetModel(Effect* effect, int32_t index, void* data)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);
	assert(0 <= index && index < effect_->modelCount_);
	effect_->models_[index] = data;
}

void EffectFactory::SetMaterial(Effect* effect, int32_t index, MaterialData* data)
{
	auto effect_ = static_cast<EffectImplemented*>(effect);
	assert(0 <= index && index < effect_->materialCount_);
	effect_->materials_[index] = data;
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

bool EffectFactory::OnLoading(Effect* effect, const void* data, int32_t size, float magnification, const EFK_CHAR* materialPath)
{
	return this->LoadBody(effect, data, size, magnification, materialPath);
}

void EffectFactory::OnLoadingResource(Effect* effect, const void* data, int32_t size, const EFK_CHAR* materialPath)
{
	auto textureLoader = effect->GetSetting()->GetTextureLoader();
	auto soundLoader = effect->GetSetting()->GetSoundLoader();
	auto modelLoader = effect->GetSetting()->GetModelLoader();
	auto materialLoader = effect->GetSetting()->GetMaterialLoader();

	if (textureLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetColorImageCount(); i++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetColorImagePath(i));

			auto resource = textureLoader->Load(fullPath, TextureType::Color);
			SetTexture(effect, i, TextureType::Color, resource);
		}

		for (auto i = 0; i < effect->GetNormalImageCount(); i++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetNormalImagePath(i));

			auto resource = textureLoader->Load(fullPath, TextureType::Normal);
			SetTexture(effect, i, TextureType::Normal, resource);
		}

		for (auto i = 0; i < effect->GetDistortionImageCount(); i++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetDistortionImagePath(i));

			auto resource = textureLoader->Load(fullPath, TextureType::Distortion);
			SetTexture(effect, i, TextureType::Distortion, resource);
		}
	}

	if (soundLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetWaveCount(); i++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetWavePath(i));

			auto resource = soundLoader->Load(fullPath);
			SetSound(effect, i, resource);
		}
	}

	if (modelLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetModelCount(); i++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetModelPath(i));

			auto resource = modelLoader->Load(fullPath);
			SetModel(effect, i, resource);
		}
	}

	if (materialLoader != nullptr)
	{
		for (auto i = 0; i < effect->GetMaterialCount(); i++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, materialPath, effect->GetMaterialPath(i));

			auto resource = materialLoader->Load(fullPath);
			SetMaterial(effect, i, resource);
		}
	}
}

void EffectFactory::OnUnloadingResource(Effect* effect)
{
	auto textureLoader = effect->GetSetting()->GetTextureLoader();
	auto soundLoader = effect->GetSetting()->GetSoundLoader();
	auto modelLoader = effect->GetSetting()->GetModelLoader();
	auto materialLoader = effect->GetSetting()->GetMaterialLoader();

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

Effect* Effect::Create(Manager* manager, void* data, int32_t size, float magnification, const EFK_CHAR* materialPath)
{
	return EffectImplemented::Create(manager, data, size, magnification, materialPath);
}

Effect* Effect::Create(Manager* manager, const EFK_CHAR* path, float magnification, const EFK_CHAR* materialPath)
{
	Setting* setting = manager->GetSetting();

	EffectLoader* eLoader = setting->GetEffectLoader();

	if (setting == NULL)
		return NULL;

	void* data = NULL;
	int32_t size = 0;

	if (!eLoader->Load(path, data, size))
		return NULL;

	EFK_CHAR parentDir[512];
	if (materialPath == NULL)
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	Effect* effect = EffectImplemented::Create(manager, data, size, magnification, materialPath);

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
		m_ImagePaths = new EFK_CHAR*[m_ImageCount];
		m_pImages = new TextureData*[m_ImageCount];

		for (int i = 0; i < m_ImageCount; i++)
		{
			int length = 0;
			binaryReader.Read(length, 0, elementCountMax);

			m_ImagePaths[i] = new EFK_CHAR[length];
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
			m_normalImagePaths = new EFK_CHAR*[m_normalImageCount];
			m_normalImages = new TextureData*[m_normalImageCount];

			for (int i = 0; i < m_normalImageCount; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				m_normalImagePaths[i] = new EFK_CHAR[length];
				binaryReader.Read(m_normalImagePaths[i], length);

				m_normalImages[i] = nullptr;
			}
		}

		// Image
		binaryReader.Read(m_distortionImageCount, 0, elementCountMax);

		if (m_distortionImageCount > 0)
		{
			m_distortionImagePaths = new EFK_CHAR*[m_distortionImageCount];
			m_distortionImages = new TextureData*[m_distortionImageCount];

			for (int i = 0; i < m_distortionImageCount; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				m_distortionImagePaths[i] = new EFK_CHAR[length];
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
			m_WavePaths = new EFK_CHAR*[m_WaveCount];
			m_pWaves = new void*[m_WaveCount];

			for (int i = 0; i < m_WaveCount; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				m_WavePaths[i] = new EFK_CHAR[length];
				binaryReader.Read(m_WavePaths[i], length);

				m_pWaves[i] = nullptr;
			}
		}
	}

	if (m_version >= 6)
	{
		// Model
		binaryReader.Read(modelCount_, 0, elementCountMax);

		if (modelCount_ > 0)
		{
			modelPaths_ = new EFK_CHAR*[modelCount_];
			models_ = new void*[modelCount_];

			for (int i = 0; i < modelCount_; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				modelPaths_[i] = new EFK_CHAR[length];
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
			materialPaths_ = new EFK_CHAR*[materialCount_];
			materials_ = new MaterialData*[materialCount_];

			for (int i = 0; i < materialCount_; i++)
			{
				int length = 0;
				binaryReader.Read(length, 0, elementCountMax);

				materialPaths_[i] = new EFK_CHAR[length];
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

	Setting* loader = GetSetting();

	TextureLoader* textureLoader = loader->GetTextureLoader();
	if (textureLoader != NULL)
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
	if (soundLoader != NULL)
	{
		for (auto it : reloadingBackup->sounds.GetCollection())
		{
			soundLoader->Unload(it.second.value);
		}
	}

	{
		ModelLoader* modelLoader = loader->GetModelLoader();
		if (modelLoader != NULL)
		{
			for (auto it : reloadingBackup->models.GetCollection())
			{
				modelLoader->Unload(it.second.value);
			}
		}
	}

	reloadingBackup.reset();
}

Effect* EffectImplemented::Create(Manager* pManager, void* pData, int size, float magnification, const EFK_CHAR* materialPath)
{
	if (pData == NULL || size == 0)
		return NULL;

	EffectImplemented* effect = new EffectImplemented(pManager, pData, size);
	if (!effect->Load(pData, size, magnification, materialPath, ReloadingThreadType::Main))
	{
		effect->Release();
		effect = NULL;
	}
	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* Effect::Create(Setting* setting, void* data, int32_t size, float magnification, const EFK_CHAR* materialPath)
{
	return EffectImplemented::Create(setting, data, size, magnification, materialPath);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* Effect::Create(Setting* setting, const EFK_CHAR* path, float magnification, const EFK_CHAR* materialPath)
{
	if (setting == NULL)
		return NULL;
	EffectLoader* eLoader = setting->GetEffectLoader();

	if (setting == NULL)
		return NULL;

	void* data = NULL;
	int32_t size = 0;

	if (!eLoader->Load(path, data, size))
		return NULL;

	EFK_CHAR parentDir[512];
	if (materialPath == NULL)
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	Effect* effect = EffectImplemented::Create(setting, data, size, magnification, materialPath);

	eLoader->Unload(data, size);

	effect->SetName(getFilenameWithoutExt(path).c_str());

	return effect;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effect* EffectImplemented::Create(Setting* setting, void* pData, int size, float magnification, const EFK_CHAR* materialPath)
{
	if (pData == NULL || size == 0)
		return NULL;

	EffectImplemented* effect = new EffectImplemented(setting, pData, size);
	if (!effect->Load(pData, size, magnification, materialPath, ReloadingThreadType::Main))
	{
		effect->Release();
		effect = NULL;
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
	: m_pManager((ManagerImplemented*)pManager)
	, m_setting(NULL)
	, m_reference(1)
	, m_version(0)
	, m_ImageCount(0)
	, m_ImagePaths(NULL)
	, m_pImages(NULL)
	, m_normalImageCount(0)
	, m_normalImagePaths(nullptr)
	, m_normalImages(nullptr)
	, m_distortionImageCount(0)
	, m_distortionImagePaths(nullptr)
	, m_distortionImages(nullptr)
	, m_defaultRandomSeed(-1)

{
	ES_SAFE_ADDREF(m_pManager);

	Culling.Shape = CullingShape::NoneShape;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectImplemented::EffectImplemented(Setting* setting, void* pData, int size)
	: m_pManager(NULL)
	, m_setting(setting)
	, m_reference(1)
	, m_version(0)
	, m_ImageCount(0)
	, m_ImagePaths(NULL)
	, m_pImages(NULL)
	, m_normalImageCount(0)
	, m_normalImagePaths(nullptr)
	, m_normalImages(nullptr)
	, m_distortionImageCount(0)
	, m_distortionImagePaths(nullptr)
	, m_distortionImages(nullptr)
{
	ES_SAFE_ADDREF(m_setting);

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

	ES_SAFE_RELEASE(m_setting);
	ES_SAFE_RELEASE(m_pManager);

	ES_SAFE_RELEASE(factory);
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
bool EffectImplemented::Load(void* pData, int size, float mag, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType)
{
	ES_SAFE_RELEASE(factory);

	if (m_setting != nullptr)
	{
		for (int i = 0; i < m_setting->GetEffectFactoryCount(); i++)
		{
			auto f = m_setting->GetEffectFactory(i);

			if (f->OnCheckIsBinarySupported(pData, size))
			{
				ES_SAFE_ADDREF(f);
				factory = f;
				break;
			}
		}
	}

	if (m_pManager != nullptr)
	{
		for (int i = 0; i < m_pManager->GetSetting()->GetEffectFactoryCount(); i++)
		{
			auto f = m_pManager->GetSetting()->GetEffectFactory(i);

			if (f->OnCheckIsBinarySupported(pData, size))
			{
				ES_SAFE_ADDREF(f);
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
		if (m_ImagePaths[i] != NULL)
			delete[] m_ImagePaths[i];
	}

	m_ImageCount = 0;

	ES_SAFE_DELETE_ARRAY(m_ImagePaths);
	ES_SAFE_DELETE_ARRAY(m_pImages);

	{
		for (int i = 0; i < m_normalImageCount; i++)
		{
			if (m_normalImagePaths[i] != NULL)
				delete[] m_normalImagePaths[i];
		}

		m_normalImageCount = 0;

		ES_SAFE_DELETE_ARRAY(m_normalImagePaths);
		ES_SAFE_DELETE_ARRAY(m_normalImages);
	}

	{
		for (int i = 0; i < m_distortionImageCount; i++)
		{
			if (m_distortionImagePaths[i] != NULL)
				delete[] m_distortionImagePaths[i];
		}

		m_distortionImageCount = 0;

		ES_SAFE_DELETE_ARRAY(m_distortionImagePaths);
		ES_SAFE_DELETE_ARRAY(m_distortionImages);
	}

	for (int i = 0; i < m_WaveCount; i++)
	{
		if (m_WavePaths[i] != NULL)
			delete[] m_WavePaths[i];
	}
	m_WaveCount = 0;

	ES_SAFE_DELETE_ARRAY(m_WavePaths);
	ES_SAFE_DELETE_ARRAY(m_pWaves);

	for (int i = 0; i < modelCount_; i++)
	{
		if (modelPaths_[i] != NULL)
			delete[] modelPaths_[i];
	}
	modelCount_ = 0;

	ES_SAFE_DELETE_ARRAY(modelPaths_);
	ES_SAFE_DELETE_ARRAY(models_);

	for (int i = 0; i < materialCount_; i++)
	{
		if (materialPaths_[i] != NULL)
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

Manager* EffectImplemented::GetManager() const
{
	return m_pManager;
}

const char16_t* EffectImplemented::GetName() const
{
	return name_.c_str();
}

void EffectImplemented::SetName(const char16_t* name)
{
	name_ = name;
}

Setting* EffectImplemented::GetSetting() const
{
	if (m_setting != NULL)
		return m_setting;
	return m_pManager->GetSetting();
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
	return m_pImages[n];
}

int32_t EffectImplemented::GetColorImageCount() const
{
	return m_ImageCount;
}

const EFK_CHAR* EffectImplemented::GetColorImagePath(int n) const
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

	return m_normalImages[n];
}

int32_t EffectImplemented::GetNormalImageCount() const
{
	return m_normalImageCount;
}

const EFK_CHAR* EffectImplemented::GetNormalImagePath(int n) const
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

	return m_distortionImages[n];
}

int32_t EffectImplemented::GetDistortionImageCount() const
{
	return m_distortionImageCount;
}

const EFK_CHAR* EffectImplemented::GetDistortionImagePath(int n) const
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

const EFK_CHAR* EffectImplemented::GetWavePath(int n) const
{
	return m_WavePaths[n];
}

void* EffectImplemented::GetModel(int n) const
{
	return models_[n];
}

int32_t EffectImplemented::GetModelCount() const
{
	return modelCount_;
}

const EFK_CHAR* EffectImplemented::GetModelPath(int n) const
{
	return modelPaths_[n];
}

MaterialData* EffectImplemented::GetMaterial(int n) const
{
	return materials_[n];
}

int32_t EffectImplemented::GetMaterialCount() const
{
	return materialCount_;
}

const EFK_CHAR* EffectImplemented::GetMaterialPath(int n) const
{
	return materialPaths_[n];
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

void EffectImplemented::SetModel(int32_t index, void* data)
{
	auto modelLoader = GetSetting()->GetModelLoader();
	assert(0 <= index && index < modelCount_);

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

bool EffectImplemented::Reload(void* data, int32_t size, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType)
{
	if (m_pManager == NULL)
		return false;

	std::array<Manager*, 1> managers;
	managers[0] = m_pManager;

	return Reload(managers.data(), static_cast<int32_t>(managers.size()), data, size, materialPath, reloadingThreadType);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Reload(const EFK_CHAR* path, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType)
{
	if (m_pManager == NULL)
		return false;

	std::array<Manager*, 1> managers;
	managers[0] = m_pManager;

	return Reload(managers.data(), static_cast<int32_t>(managers.size()), path, materialPath, reloadingThreadType);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Reload(Manager** managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType)
{
	if (!factory->OnCheckIsReloadSupported())
		return false;

	const EFK_CHAR* matPath = materialPath != NULL ? materialPath : m_materialPath.c_str();

	for (int32_t i = 0; i < managersCount; i++)
	{
		// to call only once
		for (int32_t j = 0; j < i; j++)
		{
			if (managers[i] == managers[j])
				continue;
		}

		auto manager = static_cast<ManagerImplemented*>(managers[i]);
		manager->BeginReloadEffect(this, true);
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
		manager->EndReloadEffect(this, true);
	}

	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool EffectImplemented::Reload(Manager** managers, int32_t managersCount, const EFK_CHAR* path, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType)
{
	if (!factory->OnCheckIsReloadSupported())
		return false;

	Setting* loader = GetSetting();

	EffectLoader* eLoader = loader->GetEffectLoader();
	if (loader == NULL)
		return false;

	void* data = NULL;
	int32_t size = 0;

	if (!eLoader->Load(path, data, size))
		return false;

	EFK_CHAR parentDir[512];
	if (materialPath == NULL)
	{
		GetParentDir(parentDir, path);
		materialPath = parentDir;
	}

	int lockCount = 0;

	for (int32_t i = 0; i < managersCount; i++)
	{
		auto manager = static_cast<ManagerImplemented*>(managers[i]);
		manager->BeginReloadEffect(this, lockCount == 0);
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
		manager->EndReloadEffect(this, lockCount == 0);
	}

	eLoader->Unload(data, size);

	return false;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void EffectImplemented::ReloadResources(const void* data, int32_t size, const EFK_CHAR* materialPath)
{
	UnloadResources();

	const EFK_CHAR* matPath = materialPath != NULL ? materialPath : m_materialPath.c_str();

	Setting* loader = GetSetting();

	// reloading on render thread
	if (isReloadingOnRenderingThread)
	{
		assert(reloadingBackup != nullptr);

		for (int32_t ind = 0; ind < m_ImageCount; ind++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, m_ImagePaths[ind]);

			TextureData* value = nullptr;
			if (reloadingBackup->images.Pop(fullPath, value))
			{
				m_pImages[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < m_normalImageCount; ind++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, m_normalImagePaths[ind]);

			TextureData* value = nullptr;
			if (reloadingBackup->normalImages.Pop(fullPath, value))
			{
				m_normalImages[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < m_distortionImageCount; ind++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, m_distortionImagePaths[ind]);

			TextureData* value = nullptr;
			if (reloadingBackup->distortionImages.Pop(fullPath, value))
			{
				m_distortionImages[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < m_WaveCount; ind++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, m_WavePaths[ind]);

			void* value = nullptr;
			if (reloadingBackup->sounds.Pop(fullPath, value))
			{
				m_pWaves[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < modelCount_; ind++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, modelPaths_[ind]);

			void* value = nullptr;
			if (reloadingBackup->models.Pop(fullPath, value))
			{
				models_[ind] = value;
			}
		}

		for (int32_t ind = 0; ind < materialCount_; ind++)
		{
			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, materialPaths_[ind]);

			MaterialData* value = nullptr;
			if (reloadingBackup->materials.Pop(fullPath, value))
			{
				materials_[ind] = value;
			}
		}

		return;
	}

	factory->OnLoadingResource(this, data, size, matPath);
}

void EffectImplemented::UnloadResources(const EFK_CHAR* materialPath)
{
	Setting* loader = GetSetting();

	// reloading on render thread
	if (isReloadingOnRenderingThread)
	{
		if (reloadingBackup == nullptr)
		{
			reloadingBackup = std::unique_ptr<EffectReloadingBackup>(new EffectReloadingBackup());
		}

		const EFK_CHAR* matPath = materialPath != nullptr ? materialPath : m_materialPath.c_str();

		for (int32_t ind = 0; ind < m_ImageCount; ind++)
		{
			if (m_pImages[ind] == nullptr)
				continue;

			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, m_ImagePaths[ind]);
			reloadingBackup->images.Push(fullPath, m_pImages[ind]);
		}

		for (int32_t ind = 0; ind < m_normalImageCount; ind++)
		{
			if (m_normalImages[ind] == nullptr)
				continue;

			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, m_normalImagePaths[ind]);
			reloadingBackup->normalImages.Push(fullPath, m_normalImages[ind]);
		}

		for (int32_t ind = 0; ind < m_distortionImageCount; ind++)
		{
			if (m_distortionImagePaths[ind] == nullptr)
				continue;

			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, m_distortionImagePaths[ind]);
			reloadingBackup->distortionImages.Push(fullPath, m_distortionImages[ind]);
		}

		for (int32_t ind = 0; ind < m_WaveCount; ind++)
		{
			if (m_pWaves[ind] == nullptr)
				continue;

			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, m_WavePaths[ind]);
			reloadingBackup->sounds.Push(fullPath, m_pWaves[ind]);
		}

		for (int32_t ind = 0; ind < modelCount_; ind++)
		{
			if (models_[ind] == nullptr)
				continue;

			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, modelPaths_[ind]);
			reloadingBackup->models.Push(fullPath, models_[ind]);
		}

		for (int32_t ind = 0; ind < materialCount_; ind++)
		{
			if (materials_[ind] == nullptr)
				continue;

			EFK_CHAR fullPath[512];
			PathCombine(fullPath, matPath, materialPaths_[ind]);
			reloadingBackup->materials.Push(fullPath, materials_[ind]);
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
