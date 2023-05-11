#include "CustomAsyncLoader.h"

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

template <typename T, typename K>
void RemoveFinished(std::map<K, std::shared_ptr<CustomAsyncLoadingValue<T>>>& kv)
{
	for (auto it = kv.begin(); it != kv.end();)
	{
		if (it->second->State == AsyncLoadingState::Finished || it->second->State == AsyncLoadingState::Failed)
		{
			it = kv.erase(it);
		}
		else
		{
			it++;
		}
	}
}

template <typename T>
bool GetIsAllFinishedOrFailed(const std::vector<CustomAsyncValueHandle<T>>& values)
{
	for (size_t i = 0; i < values.size(); i++)
	{
		if (!values[i].GetIsValid())
		{
			continue;
		}

		if (!(values[i].GetState() == AsyncLoadingState::Finished || values[i].GetState() == AsyncLoadingState::Failed))
		{
			return false;
		}
	}

	return true;
}

CustomAsyncLoadingTexture::CustomAsyncLoadingTexture(const char16_t* path, Effekseer::TextureType textureType, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting)
	: path_(path)
	, textureType_(textureType)
	, fi_(fi)
	, setting_(setting)
{
}

bool CustomAsyncLoadingTexture::StartLoading()
{
	State = AsyncLoadingState::Loading;
	return true;
}

void CustomAsyncLoadingTexture::LoadAsync()
{
	auto reader = fi_->OpenRead(path_.c_str());
	if (reader == nullptr)
	{
		State = AsyncLoadingState::Failed;
		return;
	}

	buffer_.resize(reader->GetLength());
	reader->Read(buffer_.data(), buffer_.size());
	State = AsyncLoadingState::WaitFinished;
}

bool CustomAsyncLoadingTexture::EndLoading()
{
	auto isMipEnabled = Effekseer::TextureLoaderHelper::GetIsMipmapEnabled(path_);

	Value = setting_->GetTextureLoader()->Load(buffer_.data(), buffer_.size(), textureType_, isMipEnabled);
	if (Value != nullptr)
	{
		State = AsyncLoadingState::Finished;
	}
	else
	{
		State = AsyncLoadingState::Failed;
	}

	return true;
}

CustomAsyncLoadingModel::CustomAsyncLoadingModel(const char16_t* path, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting)
	: path_(path)
	, fi_(fi)
	, setting_(setting)
{
}

bool CustomAsyncLoadingModel::StartLoading()
{
	State = AsyncLoadingState::Loading;
	return true;
}

void CustomAsyncLoadingModel::LoadAsync()
{
	auto reader = fi_->OpenRead(path_.c_str());
	if (reader == nullptr)
	{
		State = AsyncLoadingState::Failed;
		return;
	}

	buffer_.resize(reader->GetLength());
	reader->Read(buffer_.data(), buffer_.size());
	State = AsyncLoadingState::WaitFinished;
}

bool CustomAsyncLoadingModel::EndLoading()
{
	Value = setting_->GetModelLoader()->Load(buffer_.data(), buffer_.size());
	if (Value != nullptr)
	{
		State = AsyncLoadingState::Finished;
	}
	else
	{
		State = AsyncLoadingState::Failed;
	}

	return true;
}

CustomAsyncLoadingMaterial::CustomAsyncLoadingMaterial(const char16_t* path, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting)
	: path_(path)
	, fi_(fi)
	, setting_(setting)
{
}

bool CustomAsyncLoadingMaterial::StartLoading()
{
	State = AsyncLoadingState::Loading;
	return true;
}

void CustomAsyncLoadingMaterial::LoadAsync()
{
	auto reader = fi_->OpenRead(path_.c_str());
	if (reader == nullptr)
	{
		State = AsyncLoadingState::Failed;
		return;
	}

	buffer_.resize(reader->GetLength());
	reader->Read(buffer_.data(), buffer_.size());
	State = AsyncLoadingState::WaitFinished;
}

bool CustomAsyncLoadingMaterial::EndLoading()
{
	Value = setting_->GetMaterialLoader()->Load(buffer_.data(), buffer_.size(), Effekseer::MaterialFileType::Code);
	if (Value != nullptr)
	{
		State = AsyncLoadingState::Finished;
	}
	else
	{
		State = AsyncLoadingState::Failed;
	}

	return true;
}

CustomAsyncLoadingCurve::CustomAsyncLoadingCurve(const char16_t* path, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting)
	: path_(path)
	, fi_(fi)
	, setting_(setting)
{
}

bool CustomAsyncLoadingCurve::StartLoading()
{
	State = AsyncLoadingState::Loading;
	return true;
}

void CustomAsyncLoadingCurve::LoadAsync()
{
	auto reader = fi_->OpenRead(path_.c_str());
	if (reader == nullptr)
	{
		State = AsyncLoadingState::Failed;
		return;
	}

	buffer_.resize(reader->GetLength());
	reader->Read(buffer_.data(), buffer_.size());
	State = AsyncLoadingState::WaitFinished;
}

bool CustomAsyncLoadingCurve::EndLoading()
{
	Value = setting_->GetCurveLoader()->Load(buffer_.data(), buffer_.size());
	if (Value != nullptr)
	{
		State = AsyncLoadingState::Finished;
	}
	else
	{
		State = AsyncLoadingState::Failed;
	}

	return true;
}

CustomAsyncLoadingProceduralModel::CustomAsyncLoadingProceduralModel(Effekseer::ProceduralModelParameter param, Effekseer::SettingRef setting)
	: param_(param)
	, setting_(setting)
{
}

bool CustomAsyncLoadingProceduralModel::StartLoading()
{
	State = AsyncLoadingState::Loading;
	return true;
}

void CustomAsyncLoadingProceduralModel::LoadAsync()
{
	State = AsyncLoadingState::WaitFinished;
}

bool CustomAsyncLoadingProceduralModel::EndLoading()
{
	Value = setting_->GetProceduralMeshGenerator()->Generate(param_);
	if (Value != nullptr)
	{
		State = AsyncLoadingState::Finished;
	}
	else
	{
		State = AsyncLoadingState::Failed;
	}

	return true;
}

CustomAsyncLoadingEffect::CustomAsyncLoadingEffect(const char16_t* path, Effekseer::FileInterfaceRef fi, Effekseer::SettingRef setting, std::weak_ptr<CustomAsyncLoader> async_loader)
	: path_(path)
	, fi_(fi)
	, setting_(setting)
	, async_loader_(async_loader)
{
}

bool CustomAsyncLoadingEffect::StartLoading()
{
	State = AsyncLoadingState::Loading;
	return true;
}

void CustomAsyncLoadingEffect::LoadAsync()
{
	auto reader = fi_->OpenRead(path_.c_str());
	if (reader == nullptr)
	{
		State = AsyncLoadingState::Failed;
		return;
	}

	Effekseer::CustomAlignedVector<uint8_t> buffer;
	buffer.resize(reader->GetLength());
	reader->Read(buffer.data(), buffer.size());

	Value = Effekseer::Effect::Create(setting_, buffer.data(), buffer.size());
	State = AsyncLoadingState::WaitFinished;
}

bool CustomAsyncLoadingEffect::EndLoading()
{
	auto loader = async_loader_.lock();

	std::array<int, 3> textureCounts;
	textureCounts[0] = Value->GetColorImageCount();
	textureCounts[1] = Value->GetNormalImageCount();
	textureCounts[2] = Value->GetDistortionImageCount();

	const auto getTexturePath = [&](int32_t type, int32_t ind)
	{
		if (type == 0)
		{
			return Value->GetColorImagePath(ind);
		}
		else if (type == 1)
		{
			return Value->GetNormalImagePath(ind);
		}
		if (type == 2)
		{
			return Value->GetDistortionImagePath(ind);
		}
		return u"";
	};

	if (isLoadingDependencies_)
	{
		for (int texType = 0; texType < 3; texType++)
		{
			auto& loadingTextures = loadingTextures_[texType];

			for (size_t i = 0; i < textureCounts[texType]; i++)
			{
				if (!GetIsAllFinishedOrFailed(loadingTextures))
				{
					return false;
				}
			}
		}

		if (!GetIsAllFinishedOrFailed(loadingModels_))
		{
			return false;
		}

		if (!GetIsAllFinishedOrFailed(loadingMaterials_))
		{
			return false;
		}

		if (!GetIsAllFinishedOrFailed(loadingCurves_))
		{
			return false;
		}

		// register to a resource manager
		auto rm = setting_->GetResourceManager();

		for (int texType = 0; texType < 3; texType++)
		{
			for (size_t i = 0; i < textureCounts[texType]; i++)
			{
				char16_t directoryPath[512];
				GetParentDir(directoryPath, path_.c_str());
				char16_t fullPath[512];
				PathCombine(fullPath, directoryPath, getTexturePath(texType, i));

				const auto type = static_cast<Effekseer::TextureType>(texType);
				auto& loadingTextures = loadingTextures_[texType];

				if (!loadingTextures[i].GetIsValid())
				{
					continue;
				}

				if (loadingTextures[i].GetState() == AsyncLoadingState::Finished)
				{
					Value->SetTexture(i, type, loadingTextures[i].GetValue());
					rm->CachedTextures.Register(fullPath, loadingTextures[i].GetValue());
				}
			}
		}

		for (size_t i = 0; i < loadingModels_.size(); i++)
		{
			char16_t directoryPath[512];
			GetParentDir(directoryPath, path_.c_str());
			char16_t fullPath[512];
			PathCombine(fullPath, directoryPath, Value->GetModelPath(i));

			if (!loadingModels_[i].GetIsValid())
			{
				continue;
			}

			if (loadingModels_[i].GetState() == AsyncLoadingState::Finished)
			{
				Value->SetModel(i, loadingModels_[i].GetValue());
				rm->CachedModels.Register(fullPath, loadingModels_[i].GetValue());
			}
		}

		for (size_t i = 0; i < loadingMaterials_.size(); i++)
		{
			char16_t directoryPath[512];
			GetParentDir(directoryPath, path_.c_str());
			char16_t fullPath[512];
			PathCombine(fullPath, directoryPath, Value->GetMaterialPath(i));

			if (!loadingMaterials_[i].GetIsValid())
			{
				continue;
			}

			if (loadingMaterials_[i].GetState() == AsyncLoadingState::Finished)
			{
				Value->SetMaterial(i, loadingMaterials_[i].GetValue());
				rm->CachedMaterials.Register(fullPath, loadingMaterials_[i].GetValue());
			}
		}

		for (size_t i = 0; i < loadingCurves_.size(); i++)
		{
			char16_t directoryPath[512];
			GetParentDir(directoryPath, path_.c_str());
			char16_t fullPath[512];
			PathCombine(fullPath, directoryPath, Value->GetCurvePath(i));

			if (!loadingCurves_[i].GetIsValid())
			{
				continue;
			}

			if (loadingCurves_[i].GetState() == AsyncLoadingState::Finished)
			{
				Value->SetCurve(i, loadingCurves_[i].GetValue());
				rm->CachedCurves.Register(fullPath, loadingCurves_[i].GetValue());
			}
		}

		for (size_t i = 0; i < loadingProceduralModels_.size(); i++)
		{
			if (!loadingProceduralModels_[i].GetIsValid())
			{
				continue;
			}

			const auto& param = *Value->GetProceduralModelParameter(i);

			if (loadingProceduralModels_[i].GetState() == AsyncLoadingState::Finished)
			{
				Value->SetProceduralModel(i, loadingProceduralModels_[i].GetValue());
				rm->CachedProceduralModels.Register(param, loadingProceduralModels_[i].GetValue());
			}
		}
	}
	else
	{
		if (loader != nullptr)
		{
			auto rm = setting_->GetResourceManager();

			for (int texType = 0; texType < 3; texType++)
			{
				for (size_t i = 0; i < textureCounts[texType]; i++)
				{
					char16_t directoryPath[512];
					GetParentDir(directoryPath, path_.c_str());
					char16_t fullPath[512];
					PathCombine(fullPath, directoryPath, getTexturePath(texType, i));

					const auto type = static_cast<Effekseer::TextureType>(texType);
					auto& loadingTextures = loadingTextures_[texType];

					if (rm->CachedTextures.IsCached(getTexturePath(texType, i)))
					{
						auto value = rm->CachedTextures.Load(fullPath, type);
						Value->SetTexture(i, type, value);

						loadingTextures.emplace_back(CustomAsyncValueHandle<Effekseer::Texture>(nullptr));
					}
					else
					{
						loadingTextures.emplace_back(loader->LoadTextureAsync(fullPath, type));
					}
				}
			}

			for (size_t i = 0; i < Value->GetModelCount(); i++)
			{
				char16_t directoryPath[512];
				GetParentDir(directoryPath, path_.c_str());
				char16_t fullPath[512];
				PathCombine(fullPath, directoryPath, Value->GetModelPath(i));

				if (rm->CachedModels.IsCached(Value->GetModelPath(i)))
				{
					auto value = rm->CachedModels.Load(fullPath);
					Value->SetModel(i, value);

					loadingModels_.emplace_back(CustomAsyncValueHandle<Effekseer::Model>(nullptr));
				}
				else
				{
					loadingModels_.emplace_back(loader->LoadModelAsync(fullPath));
				}
			}

			for (size_t i = 0; i < Value->GetModelCount(); i++)
			{
				char16_t directoryPath[512];
				GetParentDir(directoryPath, path_.c_str());
				char16_t fullPath[512];
				PathCombine(fullPath, directoryPath, Value->GetMaterialPath(i));

				if (rm->CachedMaterials.IsCached(Value->GetMaterialPath(i)))
				{
					auto value = rm->CachedMaterials.Load(fullPath);
					Value->SetMaterial(i, value);

					loadingMaterials_.emplace_back(CustomAsyncValueHandle<Effekseer::Material>(nullptr));
				}
				else
				{
					loadingMaterials_.emplace_back(loader->LoadMaterialAsync(fullPath));
				}
			}

			for (size_t i = 0; i < Value->GetCurveCount(); i++)
			{
				char16_t directoryPath[512];
				GetParentDir(directoryPath, path_.c_str());
				char16_t fullPath[512];
				PathCombine(fullPath, directoryPath, Value->GetCurvePath(i));

				if (rm->CachedCurves.IsCached(Value->GetCurvePath(i)))
				{
					auto value = rm->CachedCurves.Load(fullPath);
					Value->SetCurve(i, value);

					loadingCurves_.emplace_back(CustomAsyncValueHandle<Effekseer::Curve>(nullptr));
				}
				else
				{
					loadingCurves_.emplace_back(loader->LoadCurveAsync(fullPath));
				}
			}

			for (size_t i = 0; i < Value->GetProceduralModelCount(); i++)
			{
				const auto& param = *Value->GetProceduralModelParameter(i);
				if (rm->CachedProceduralModels.IsCached(param))
				{
					auto value = rm->CachedProceduralModels.Load(param);
					Value->SetProceduralModel(i, value);

					loadingProceduralModels_.emplace_back(CustomAsyncValueHandle<Effekseer::Model>(nullptr));
				}
				else
				{
					loadingProceduralModels_.emplace_back(loader->LoadProceduralModelAsync(param));
				}
			}
		}

		isLoadingDependencies_ = true;
		return false;
	}

	State = AsyncLoadingState::Finished;
	return true;
}

CustomAsyncLoader::CustomAsyncLoader(Effekseer::FileInterfaceRef fileInterface, Effekseer::SettingRef setting)
	: fi_(fileInterface)
	, setting_(setting)
{
	wt_.Launch();
}

std::shared_ptr<CustomAsyncLoader> CustomAsyncLoader::Create(Effekseer::FileInterfaceRef fileInterface, Effekseer::SettingRef setting)
{
	return std::make_shared<CustomAsyncLoader>(fileInterface, setting);
}

CustomAsyncValueHandle<Effekseer::Texture> CustomAsyncLoader::LoadTextureAsync(const char16_t* path, Effekseer::TextureType textureType)
{
	auto& loadingTextures = loadingTextures_[static_cast<int>(textureType)];

	const auto key = std::u16string(path);
	if (loadingTextures.find(key) != loadingTextures.end())
	{
		return CustomAsyncValueHandle<Effekseer::Texture>(loadingTextures[key]);
	}

	auto loading = std::make_shared<CustomAsyncLoadingTexture>(path, textureType, fi_, setting_);

	loadings_.emplace_back(loading);
	loadingTextures[key] = loading;
	return CustomAsyncValueHandle<Effekseer::Texture>(loading);
}

CustomAsyncValueHandle<Effekseer::Model> CustomAsyncLoader::LoadModelAsync(const char16_t* path)
{
	const auto key = std::u16string(path);
	if (loadingModels_.find(key) != loadingModels_.end())
	{
		return CustomAsyncValueHandle<Effekseer::Model>(loadingModels_[key]);
	}

	auto loading = std::make_shared<CustomAsyncLoadingModel>(path, fi_, setting_);

	loadings_.emplace_back(loading);
	loadingModels_[key] = loading;
	return CustomAsyncValueHandle<Effekseer::Model>(loading);
}

CustomAsyncValueHandle<Effekseer::Material> CustomAsyncLoader::LoadMaterialAsync(const char16_t* path)
{
	const auto key = std::u16string(path);
	if (loadingMaterials_.find(key) != loadingMaterials_.end())
	{
		return CustomAsyncValueHandle<Effekseer::Material>(loadingMaterials_[key]);
	}

	auto loading = std::make_shared<CustomAsyncLoadingMaterial>(path, fi_, setting_);

	loadings_.emplace_back(loading);
	loadingMaterials_[key] = loading;
	return CustomAsyncValueHandle<Effekseer::Material>(loading);
}

CustomAsyncValueHandle<Effekseer::Curve> CustomAsyncLoader::LoadCurveAsync(const char16_t* path)
{
	const auto key = std::u16string(path);
	if (loadingCurves_.find(key) != loadingCurves_.end())
	{
		return CustomAsyncValueHandle<Effekseer::Curve>(loadingCurves_[key]);
	}

	auto loading = std::make_shared<CustomAsyncLoadingCurve>(path, fi_, setting_);

	loadings_.emplace_back(loading);
	loadingCurves_[key] = loading;
	return CustomAsyncValueHandle<Effekseer::Curve>(loading);
}

CustomAsyncValueHandle<Effekseer::Model> CustomAsyncLoader::LoadProceduralModelAsync(Effekseer::ProceduralModelParameter param)
{
	const auto key = param;
	if (loadingProceduralModels_.find(key) != loadingProceduralModels_.end())
	{
		return CustomAsyncValueHandle<Effekseer::Model>(loadingProceduralModels_[key]);
	}

	auto loading = std::make_shared<CustomAsyncLoadingProceduralModel>(param, setting_);

	loadings_.emplace_back(loading);
	loadingProceduralModels_[key] = loading;
	return CustomAsyncValueHandle<Effekseer::Model>(loading);
}

CustomAsyncValueHandle<Effekseer::Effect> CustomAsyncLoader::LoadEffectAsync(const char16_t* path)
{
	const auto key = std::u16string(path);
	if (loadingEffects_.find(key) != loadingEffects_.end())
	{
		return CustomAsyncValueHandle<Effekseer::Effect>(loadingEffects_[key]);
	}

	auto loading = std::make_shared<CustomAsyncLoadingEffect>(path, fi_, setting_, shared_from_this());

	loadings_.emplace_back(loading);
	loadingEffects_[key] = loading;
	return CustomAsyncValueHandle<Effekseer::Effect>(loading);
}

void CustomAsyncLoader::Update()
{
	if (!loadings_.empty())
	{
		for (size_t ind = 0; ind < loadings_.size();)
		{
			auto loading = loadings_[ind];

			if (loading->State == AsyncLoadingState::None)
			{
				if (!loading->StartLoading())
				{
					loadings_.erase(loadings_.begin() + ind, loadings_.begin() + ind + 1);
				}

				wt_.RunAsync([loading]()
							 { loading->LoadAsync(); });

				break;
			}
			else if (loading->State == AsyncLoadingState::Loading)
			{
				break;
			}
			else if (loading->State == AsyncLoadingState::WaitFinished)
			{
				if (loading->EndLoading())
				{
					loadings_.erase(loadings_.begin() + ind, loadings_.begin() + ind + 1);
				}
				else
				{
					ind++;
				}
			}
			else if (loading->State == AsyncLoadingState::Finished || loading->State == AsyncLoadingState::Failed)
			{
				loadings_.erase(loadings_.begin() + ind, loadings_.begin() + ind + 1);
			}
			else
			{
				abort();
			}
		}
	}

	RemoveFinished(loadingEffects_);
	RemoveFinished(loadingTextures_[0]);
	RemoveFinished(loadingTextures_[1]);
	RemoveFinished(loadingTextures_[2]);
	RemoveFinished(loadingModels_);
	RemoveFinished(loadingMaterials_);
	RemoveFinished(loadingCurves_);
	RemoveFinished(loadingProceduralModels_);
}