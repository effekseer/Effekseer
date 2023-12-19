#pragma once

#include "VectorField.h"
#include <stdint.h>

namespace Effekseer
{

class VectorFieldLoader : public ReferenceObject
{
public:
	virtual VectorFieldScalarRef LoadAsScalar(const void* data, int32_t size);

	virtual VectorFieldVectorRef LoadAsVector(const void* data, int32_t size);

	virtual void Unload(VectorFieldScalarRef data);

	virtual void Unload(VectorFieldVectorRef data);
};

} // namespace Effekseer
