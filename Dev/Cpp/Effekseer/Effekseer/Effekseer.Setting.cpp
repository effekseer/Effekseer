
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "Effekseer.Setting.h"

#include "Effekseer.RectF.h"

#include "Effekseer.CurveLoader.h"
#include "Effekseer.EffectLoader.h"
#include "Effekseer.MaterialLoader.h"
#include "Effekseer.SoundLoader.h"
#include "Effekseer.TextureLoader.h"
#include "Model/ModelLoader.h"

#include "Renderer/Effekseer.ModelRenderer.h"
#include "Renderer/Effekseer.RibbonRenderer.h"
#include "Renderer/Effekseer.RingRenderer.h"
#include "Renderer/Effekseer.SpriteRenderer.h"
#include "Renderer/Effekseer.TrackRenderer.h"

#include "Effekseer.Effect.h"
#include "IO/Effekseer.EfkEfcFactory.h"
#include "Model/ProcedualModelGenerator.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Setting::Setting()
	: m_coordinateSystem(CoordinateSystem::RH)
{
	auto effectFactory = MakeRefPtr<EffectFactory>();
	AddEffectFactory(effectFactory);

	// this function is for 1.6
	auto efkefcFactory = MakeRefPtr<EfkEfcFactory>();
	AddEffectFactory(efkefcFactory);

	procedualMeshGenerator_ = ProcedualModelGeneratorRef(new ProcedualModelGenerator());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Setting::~Setting()
{
	ClearEffectFactory();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SettingRef Setting::Create()
{
	return SettingRef(new Setting());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
CoordinateSystem Setting::GetCoordinateSystem() const
{
	return m_coordinateSystem;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Setting::SetCoordinateSystem(CoordinateSystem coordinateSystem)
{
	m_coordinateSystem = coordinateSystem;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffectLoaderRef Setting::GetEffectLoader()
{
	return m_effectLoader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Setting::SetEffectLoader(EffectLoaderRef loader)
{
	m_effectLoader = loader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoaderRef Setting::GetTextureLoader()
{
	return m_textureLoader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Setting::SetTextureLoader(TextureLoaderRef loader)
{
	m_textureLoader = loader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoaderRef Setting::GetModelLoader()
{
	return m_modelLoader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Setting::SetModelLoader(ModelLoaderRef loader)
{
	m_modelLoader = loader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoaderRef Setting::GetSoundLoader()
{
	return m_soundLoader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Setting::SetSoundLoader(SoundLoaderRef loader)
{
	m_soundLoader = loader;
}

MaterialLoaderRef Setting::GetMaterialLoader()
{
	return m_materialLoader;
}

void Setting::SetMaterialLoader(MaterialLoaderRef loader)
{
	m_materialLoader = loader;
}

CurveLoaderRef Setting::GetCurveLoader()
{
	return m_curveLoader;
}

void Setting::SetCurveLoader(CurveLoaderRef loader)
{
	m_curveLoader = loader;
}

ProcedualModelGeneratorRef Setting::GetProcedualMeshGenerator() const
{
	return procedualMeshGenerator_;
}

void Setting::SetProcedualMeshGenerator(ProcedualModelGeneratorRef generator)
{
	procedualMeshGenerator_ = generator;
}

void Setting::AddEffectFactory(const RefPtr<EffectFactory>& effectFactory)
{
	if (effectFactory.Get() == nullptr)
	{
		return;
	}

	auto effectFactoryCopied = effectFactory;
	effectFactories.emplace_back(effectFactoryCopied);
}

void Setting::ClearEffectFactory()
{
	effectFactories.clear();
}

const RefPtr<EffectFactory>& Setting::GetEffectFactory(int32_t ind) const
{
	return effectFactories[ind];
}

int32_t Setting::GetEffectFactoryCount() const
{
	return static_cast<int32_t>(effectFactories.size());
}

} // namespace Effekseer
  //----------------------------------------------------------------------------------
  //
  //----------------------------------------------------------------------------------