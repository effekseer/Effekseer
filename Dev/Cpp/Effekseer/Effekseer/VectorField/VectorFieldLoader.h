#pragma once

#include "../Effekseer.File.h"
#include "VectorField.h"
#include <stdint.h>

namespace Effekseer
{

class VectorFieldLoader : public ReferenceObject
{
	::Effekseer::FileInterfaceRef fileInterface_ = nullptr;

public:
	VectorFieldLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr);

	virtual ~VectorFieldLoader() = default;

	virtual VectorFieldRef Load(const char16_t* path);

	virtual VectorFieldRef Load(const void* data, int32_t size);

	virtual void Unload(VectorFieldRef data);
};

} // namespace Effekseer
