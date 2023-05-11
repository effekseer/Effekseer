#pragma once

#include <Effekseer.Modules.h>
#include <Effekseer.h>

#include "../../Dev/Cpp/Effekseer/Effekseer/Effekseer.ResourceManager.h"
#include "../../Dev/Cpp/Effekseer/Effekseer/Effekseer.WorkerThread.h"
#include "../../Dev/Cpp/Effekseer/Effekseer/IO/Effekseer.EfkEfcFactory.h"

class CustomEfkEfcFactory : public Effekseer::EfkEfcFactory
{
public:
	bool GetIsResourcesLoadedAutomatically() const override
	{
		return false;
	}
};

class CustomAsyncLoader;

enum class AsyncLoadingState
{
	None,
	Loading,
	WaitFinished,
	Finished,
	Failed,
};

class CustomAsyncLoading
{
public:
	AsyncLoadingState State = AsyncLoadingState::None;
	virtual bool StartLoading() = 0;
	virtual void LoadAsync() = 0;
	virtual bool EndLoading() = 0;
};

template <class T>
class CustomAsyncLoadingValue : public CustomAsyncLoading
{
public:
	Effekseer::RefPtr<T> Value;
};

template <class T>
class CustomAsyncValueHandle
{
	std::shared_ptr<CustomAsyncLoadingValue<T>> value_;

public:
	CustomAsyncValueHandle(std::shared_ptr<CustomAsyncLoadingValue<T>> value)
		: value_(value)
	{
	}

	bool GetIsValid() const
	{
		return value_ != nullptr;
	}

	AsyncLoadingState GetState() const
	{
		return value_->State;
	}

	Effekseer::RefPtr<T> GetValue() const
	{
		return value_->Value;
	}
};

class CustomAsyncLoadingTexture : public CustomAsyncLoadingValue<Effekseer::Texture>
{
	std::u16string path_;
	Effekseer::TextureType textureType_;
	Effekseer::FileInterfaceRef fi_;
	Effekseer::SettingRef setting_;
	Effekseer::CustomAlignedVector<uint8_t> buffer_;

public:
	CustomAsyncLoadingTexture(const char16_t* path, Effekseer::TextureType textureType, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting);

	bool StartLoading() override;

	void LoadAsync() override;

	bool EndLoading() override;
};

class CustomAsyncLoadingModel : public CustomAsyncLoadingValue<Effekseer::Model>
{
	std::u16string path_;
	Effekseer::FileInterfaceRef fi_;
	Effekseer::SettingRef setting_;
	Effekseer::CustomAlignedVector<uint8_t> buffer_;

public:
	CustomAsyncLoadingModel(const char16_t* path, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting);

	bool StartLoading() override;

	void LoadAsync() override;

	bool EndLoading() override;
};

class CustomAsyncLoadingMaterial : public CustomAsyncLoadingValue<Effekseer::Material>
{
	std::u16string path_;
	Effekseer::FileInterfaceRef fi_;
	Effekseer::SettingRef setting_;
	Effekseer::CustomAlignedVector<uint8_t> buffer_;

public:
	CustomAsyncLoadingMaterial(const char16_t* path, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting);

	bool StartLoading() override;

	void LoadAsync() override;

	bool EndLoading() override;
};

class CustomAsyncLoadingCurve : public CustomAsyncLoadingValue<Effekseer::Curve>
{
	std::u16string path_;
	Effekseer::FileInterfaceRef fi_;
	Effekseer::SettingRef setting_;
	Effekseer::CustomAlignedVector<uint8_t> buffer_;

public:
	CustomAsyncLoadingCurve(const char16_t* path, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting);

	bool StartLoading() override;

	void LoadAsync() override;

	bool EndLoading() override;
};

class CustomAsyncLoadingProceduralModel : public CustomAsyncLoadingValue<Effekseer::Model>
{
	Effekseer::ProceduralModelParameter param_;
	Effekseer::SettingRef setting_;

public:
	CustomAsyncLoadingProceduralModel(Effekseer::ProceduralModelParameter param, Effekseer::SettingRef setting);

	bool StartLoading() override;

	void LoadAsync() override;

	bool EndLoading() override;
};


class CustomAsyncLoadingEffect : public CustomAsyncLoadingValue<Effekseer::Effect>
{
	std::u16string path_;
	Effekseer::FileInterfaceRef fi_;
	Effekseer::SettingRef setting_;
	std::vector<CustomAsyncValueHandle<Effekseer::Texture>> loadingTextures_[3];
	std::vector<CustomAsyncValueHandle<Effekseer::Model>> loadingModels_;
	std::vector<CustomAsyncValueHandle<Effekseer::Material>> loadingMaterials_;
	std::vector<CustomAsyncValueHandle<Effekseer::Curve>> loadingCurves_;
	std::vector<CustomAsyncValueHandle<Effekseer::Model>> loadingProceduralModels_;
	std::weak_ptr<CustomAsyncLoader> async_loader_;

	bool isLoadingDependencies_ = false;

public:
	CustomAsyncLoadingEffect(const char16_t* path, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting, std::weak_ptr<CustomAsyncLoader> async_loader);

	bool StartLoading() override;

	void LoadAsync() override;

	bool EndLoading() override;
};

class CustomAsyncLoader : public std::enable_shared_from_this<CustomAsyncLoader>
{
	std::map<std::u16string, std::shared_ptr<CustomAsyncLoadingValue<Effekseer::Texture>>> loadingTextures_[3];
	std::map<std::u16string, std::shared_ptr<CustomAsyncLoadingValue<Effekseer::Model>>> loadingModels_;
	std::map<std::u16string, std::shared_ptr<CustomAsyncLoadingValue<Effekseer::Material>>> loadingMaterials_;
	std::map<std::u16string, std::shared_ptr<CustomAsyncLoadingValue<Effekseer::Curve>>> loadingCurves_;
	std::map<Effekseer::ProceduralModelParameter, std::shared_ptr<CustomAsyncLoadingValue<Effekseer::Model>>> loadingProceduralModels_;
	std::map<std::u16string, std::shared_ptr<CustomAsyncLoadingValue<Effekseer::Effect>>> loadingEffects_;

	std::vector<std::shared_ptr<CustomAsyncLoading>> loadings_;

	Effekseer::WorkerThread wt_;

	Effekseer::FileInterfaceRef fi_;
	Effekseer::SettingRef setting_;

public:
	CustomAsyncLoader(Effekseer::FileInterfaceRef fileInterface, Effekseer::SettingRef setting);

	std::shared_ptr<CustomAsyncLoader> static Create(Effekseer::FileInterfaceRef fileInterface, Effekseer::SettingRef setting);

	CustomAsyncValueHandle<Effekseer::Texture> LoadTextureAsync(const char16_t* path, Effekseer::TextureType textureType);

	CustomAsyncValueHandle<Effekseer::Model> LoadModelAsync(const char16_t* path);

	CustomAsyncValueHandle<Effekseer::Material> LoadMaterialAsync(const char16_t* path);

	CustomAsyncValueHandle<Effekseer::Curve> LoadCurveAsync(const char16_t* path);

	CustomAsyncValueHandle<Effekseer::Model> LoadProceduralModelAsync(Effekseer::ProceduralModelParameter param);

	CustomAsyncValueHandle<Effekseer::Effect> LoadEffectAsync(const char16_t* path);

	void Update();
};
