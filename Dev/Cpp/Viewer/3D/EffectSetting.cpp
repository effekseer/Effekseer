#include "EffectSetting.h"
#include "../Graphics/GraphicsDevice.h"
#include "../Sound/SoundDevice.h"
#include "FileInterface.h"

#ifdef _WIN32
#include "../Graphics/Platform/DX11/efk.GraphicsDX11.h"
#endif
#include "../Graphics/Platform/GL/efk.GraphicsGL.h"

#include <EffekseerSoundOSMixer.h>

namespace Effekseer::Tool
{
std::shared_ptr<EffectSetting> EffectSetting::Create(std::shared_ptr<Effekseer::Tool::GraphicsDevice> graphicsDevice, std::shared_ptr<Effekseer::Tool::SoundDevice> soundDevice)
{
	auto ret = std::make_shared<EffectSetting>();
	auto setting = Effekseer::Setting::Create();
	ret->setting_ = setting;

	auto gd = graphicsDevice->GetGraphics()->GetGraphicsDevice();

	auto fileInterface = Effekseer::MakeRefPtr<Effekseer::Tool::EffekseerFile>();

	auto textureLoader = EffekseerRenderer::CreateTextureLoader(
		gd,
		fileInterface,
		graphicsDevice->GetIsSRGBMode() ? ::Effekseer::ColorSpaceType::Linear : ::Effekseer::ColorSpaceType::Gamma);

	setting->SetTextureLoader(textureLoader);

	auto modelLoader = EffekseerRenderer::CreateModelLoader(gd, fileInterface);

	setting->SetModelLoader(modelLoader);

	setting->SetCurveLoader(Effekseer::CurveLoaderRef(new ::Effekseer::CurveLoader(fileInterface)));

	if (soundDevice != nullptr)
	{
		setting->SetSoundLoader(soundDevice->GetSound()->CreateSoundLoader(fileInterface));
	}

	if (graphicsDevice->GetDeviceType() == DeviceType::DirectX11)
	{
#ifdef _WIN32
		setting->SetMaterialLoader(EffekseerRendererDX11::CreateMaterialLoader(gd, fileInterface));
#endif
	}
	else if (graphicsDevice->GetDeviceType() == DeviceType::OpenGL)
	{
		setting->SetMaterialLoader(EffekseerRendererGL::CreateMaterialLoader(gd, fileInterface));
	}

	return ret;

	return nullptr;
}

} // namespace Effekseer::Tool