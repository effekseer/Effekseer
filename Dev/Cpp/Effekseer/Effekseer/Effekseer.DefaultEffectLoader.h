
#ifndef __EFFEKSEER_DEFAULTEFFECTLOADER_H__
#define __EFFEKSEER_DEFAULTEFFECTLOADER_H__

#include "Effekseer.Base.h"
#include "Effekseer.DefaultFile.h"
#include "Effekseer.EffectLoader.h"

namespace Effekseer
{

class DefaultEffectLoader : public EffectLoader
{
	FileInterfaceRef m_fileInterface;

public:
	DefaultEffectLoader(FileInterfaceRef fileInterface = nullptr);

	virtual ~DefaultEffectLoader() override;

	bool Load(const char16_t* path, void*& data, int32_t& size) override;

	void Unload(void* data, int32_t size) override;
};

} // namespace Effekseer

#endif // __EFFEKSEER_DEFAULTEFFECTLOADER_H__
