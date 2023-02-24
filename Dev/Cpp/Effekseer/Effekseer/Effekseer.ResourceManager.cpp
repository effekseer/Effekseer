

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.ResourceManager.h"
#include "Effekseer.CurveLoader.h"
#include "Effekseer.MaterialLoader.h"
#include "Effekseer.SoundLoader.h"
#include "Effekseer.TextureLoader.h"
#include "Model/ModelLoader.h"
#include "Model/ProceduralModelGenerator.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureRef ResourceManager::LoadTexture(const char16_t* path, TextureType textureType)
{
	return CachedTextures.Load(path, textureType);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ResourceManager::UnloadTexture(TextureRef resource)
{
	CachedTextures.Unload(resource);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRef ResourceManager::LoadModel(const char16_t* path)
{
	return CachedModels.Load(path);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ResourceManager::UnloadModel(ModelRef resource)
{
	CachedModels.Unload(resource);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundDataRef ResourceManager::LoadSoundData(const char16_t* path)
{
	return CachedSounds.Load(path);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ResourceManager::UnloadSoundData(SoundDataRef resource)
{
	CachedSounds.Unload(resource);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
MaterialRef ResourceManager::LoadMaterial(const char16_t* path)
{
	return CachedMaterials.Load(path);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ResourceManager::UnloadMaterial(MaterialRef resource)
{
	CachedMaterials.Unload(resource);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
CurveRef ResourceManager::LoadCurve(const char16_t* path)
{
	return CachedCurves.Load(path);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ResourceManager::UnloadCurve(CurveRef resource)
{
	CachedCurves.Unload(resource);
}

ModelRef ResourceManager::GenerateProceduralModel(const ProceduralModelParameter& param)
{
	return CachedProceduralModels.Load(param);
}

void ResourceManager::UngenerateProceduralModel(ModelRef resource)
{
	CachedProceduralModels.Unload(resource);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------