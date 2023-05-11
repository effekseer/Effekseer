
#ifndef __EFFEKSEER_RESOURCE_MANAGER_H__
#define __EFFEKSEER_RESOURCE_MANAGER_H__

#include "Effekseer.Base.Pre.h"
#include "Effekseer.Resource.h"
#include "Model//ProceduralModelParameter.h"
#include "Model/ProceduralModelGenerator.h"
#include <algorithm>

namespace Effekseer
{

class ResourceManager : public ReferenceObject
{

private:
	template <typename T>
	struct LoadCounted
	{
		T resource;
		int32_t loadCount;
	};

	template <typename PARAMETER, typename T>
	struct GenerateCounted
	{
		PARAMETER param;
		T resource;
		int32_t loadCount;
	};

public:
	template <typename LOADER, typename RESOURCE>
	class CachedResources
	{
		bool isCacheEnabled_ = true;
		LOADER loader_;
		CustomUnorderedMap<StringView<char16_t>, LoadCounted<RESOURCE>, StringView<char16_t>::Hash> cached_;

	public:
		bool GetIsCacheEnabled() const
		{
			return isCacheEnabled_;
		}

		void SetIsCacheEnabled(bool value)
		{
			isCacheEnabled_ = value;
		}

		LOADER GetLoader() const
		{
			return loader_;
		}

		void SetLoader(LOADER value)
		{
			loader_ = value;
		}

		template <typename... Arg>
		RESOURCE Load(const char16_t* path, Arg&&... args)
		{
			if (loader_ != nullptr)
			{
				if (isCacheEnabled_)
				{
					auto it = cached_.find(path);
					if (it != cached_.end())
					{
						it->second.loadCount++;
						return it->second.resource;
					}

					auto resource = loader_->Load(path, args...);
					if (resource != nullptr)
					{
						resource->SetPath(path);
						const StringView<char16_t> view = resource->GetPath();
						cached_.emplace(view, LoadCounted<RESOURCE>{resource, 1});
						return resource;
					}
				}
				else
				{
					return loader_->Load(path, args...);
				}
			}
			return nullptr;
		}

		void Unload(const RESOURCE& resource)
		{
			if (loader_ != nullptr && resource != nullptr)
			{
				if (resource->GetPath() != u"")
				{
					auto it = cached_.find(resource->GetPath());
					if (it != cached_.end())
					{
						if (--it->second.loadCount <= 0)
						{
							cached_.erase(it);
							loader_->Unload(resource);
						}
					}
				}
				else
				{
					loader_->Unload(resource);
				}
			}
		}

		bool IsCached(const char16_t* path) const
		{
			const auto it = cached_.find(path);
			return it != cached_.end();
		}

		void Register(const char16_t* path, RESOURCE resource)
		{
			if (isCacheEnabled_)
			{
				auto it = cached_.find(path);
				if (it != cached_.end())
				{
					it->second.loadCount++;
				}
				else
				{
					resource->SetPath(path);
					const StringView<char16_t> view = resource->GetPath();
					cached_.emplace(view, LoadCounted<RESOURCE>{resource, 1});
				}
			}
		}
	};

	template <typename LOADER, typename PARAMETER, typename RESOURCE>
	struct CachedParameterResources
	{
	private:
		bool isCacheEnabled_ = true;
		LOADER loader_;
		CustomMap<PARAMETER, GenerateCounted<PARAMETER, RESOURCE>> cached_;

	public:
		bool GetIsCacheEnabled() const
		{
			return isCacheEnabled_;
		}

		void SetIsCacheEnabled(bool value)
		{
			isCacheEnabled_ = value;
		}

		LOADER GetLoader() const
		{
			return loader_;
		}

		void SetLoader(LOADER value)
		{
			loader_ = value;
		}

		template <typename... Arg>
		RESOURCE Load(const PARAMETER& parameter)
		{
			if (loader_ != nullptr)
			{
				if (isCacheEnabled_)
				{
					auto it = cached_.find(parameter);
					if (it != cached_.end())
					{
						it->second.loadCount++;
						return it->second.resource;
					}

					auto resource = loader_->Generate(parameter);
					if (resource != nullptr)
					{
						cached_.emplace(parameter, GenerateCounted<PARAMETER, RESOURCE>{parameter, resource, 1});
						return resource;
					}
				}
				else
				{
					return loader_->Generate(parameter);
				}
			}
			return nullptr;
		}

		void Unload(const RESOURCE& resource)
		{
			if (loader_ != nullptr && resource != nullptr)
			{
				auto it = std::find_if(cached_.begin(), cached_.end(), [&](const std::pair<PARAMETER, GenerateCounted<PARAMETER, RESOURCE>>& v)
									   { return v.second.resource == resource; });
				if (it != cached_.end())
				{
					if (--it->second.loadCount <= 0)
					{
						cached_.erase(it);
						loader_->Ungenerate(resource);
					}
				}
			}
		}

		bool IsCached(const PARAMETER& parameter) const
		{
			const auto it = cached_.find(parameter);
			return it != cached_.end();
		}

		void Register(const PARAMETER& parameter, RESOURCE resource)
		{
			if (isCacheEnabled_)
			{
				auto it = cached_.find(parameter);
				if (it != cached_.end())
				{
					it->second.loadCount++;
				}
				else
				{
					cached_.emplace(parameter, GenerateCounted<PARAMETER, RESOURCE>{parameter, resource, 1});
				}
			}
		}
	};

	ResourceManager() = default;

	~ResourceManager() = default;

	TextureLoaderRef GetTextureLoader() const
	{
		return CachedTextures.GetLoader();
	}

	void SetTextureLoader(TextureLoaderRef loader)
	{
		CachedTextures.SetLoader(loader);
	}

	ModelLoaderRef GetModelLoader() const
	{
		return CachedModels.GetLoader();
	}

	void SetModelLoader(ModelLoaderRef loader)
	{
		CachedModels.SetLoader(loader);
	}

	SoundLoaderRef GetSoundLoader() const
	{
		return CachedSounds.GetLoader();
	}

	void SetSoundLoader(SoundLoaderRef loader)
	{
		CachedSounds.SetLoader(loader);
	}

	MaterialLoaderRef GetMaterialLoader() const
	{
		return CachedMaterials.GetLoader();
	}

	void SetMaterialLoader(MaterialLoaderRef loader)
	{
		CachedMaterials.SetLoader(loader);
	}

	CurveLoaderRef GetCurveLoader() const
	{
		return CachedCurves.GetLoader();
	}

	void SetCurveLoader(CurveLoaderRef loader)
	{
		CachedCurves.SetLoader(loader);
	}

	ProceduralModelGeneratorRef GetProceduralMeshGenerator() const
	{
		return CachedProceduralModels.GetLoader();
	}

	void SetProceduralMeshGenerator(ProceduralModelGeneratorRef generator)
	{
		CachedProceduralModels.SetLoader(generator);
	}

	TextureRef LoadTexture(const char16_t* path, TextureType textureType);

	void UnloadTexture(TextureRef resource);

	ModelRef LoadModel(const char16_t* path);

	void UnloadModel(ModelRef resource);

	SoundDataRef LoadSoundData(const char16_t* path);

	void UnloadSoundData(SoundDataRef resource);

	MaterialRef LoadMaterial(const char16_t* path);

	void UnloadMaterial(MaterialRef resource);

	CurveRef LoadCurve(const char16_t* path);

	void UnloadCurve(CurveRef resource);

	ModelRef GenerateProceduralModel(const ProceduralModelParameter& param);

	void UngenerateProceduralModel(ModelRef resource);

	void SetIsCacheEnabled(bool value)
	{
		CachedTextures.SetIsCacheEnabled(value);
		CachedModels.SetIsCacheEnabled(value);
		CachedMaterials.SetIsCacheEnabled(value);
		CachedSounds.SetIsCacheEnabled(value);
		CachedCurves.SetIsCacheEnabled(value);
		CachedProceduralModels.SetIsCacheEnabled(value);
	}

	CachedResources<TextureLoaderRef, TextureRef> CachedTextures;
	CachedResources<ModelLoaderRef, ModelRef> CachedModels;
	CachedResources<SoundLoaderRef, SoundDataRef> CachedSounds;
	CachedResources<MaterialLoaderRef, MaterialRef> CachedMaterials;
	CachedResources<CurveLoaderRef, CurveRef> CachedCurves;
	CachedParameterResources<ProceduralModelGeneratorRef, ProceduralModelParameter, ModelRef> CachedProceduralModels;
};

} // namespace Effekseer

#endif // __EFFEKSEER_RESOURCE_MANAGER_H__
