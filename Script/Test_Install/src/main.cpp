#include <Effekseer.h>
#include <EffekseerRenderer.Renderer.h>

#ifdef _WIN32
#include <EffekseerRendererDX11.h>
#include <EffekseerSoundXAudio2.h>
#endif

int main() {
  auto efkManager = ::Effekseer::Manager::Create(8000);

#ifdef _WIN32
  auto efkRenderer =
      ::EffekseerRendererDX11::CreateGraphicsDevice(nullptr, nullptr);
  auto efkSound = ::EffekseerSound::Sound::Create(nullptr, 0, 0);
#endif

  return 0;
}
