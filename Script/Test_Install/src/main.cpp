#include <Effekseer.h>

#ifdef _WIN32
#include <EffekseerRendererDX11/EffekseerRendererDX11.h>
#endif

int main()
{
	auto efkManager = ::Effekseer::Manager::Create(8000);

	return 0;
}