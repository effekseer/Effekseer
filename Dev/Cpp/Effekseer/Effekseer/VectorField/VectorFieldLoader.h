#pragma once

#include "VectorField.h"
#include <stdint.h>

namespace Effekseer
{

class VectorFieldLoader : public ReferenceObject
{
public:
	virtual VectorFieldRef Load(const void* data, int32_t size);

	virtual void Unload(VectorFieldRef data);
};

} // namespace Effekseer
