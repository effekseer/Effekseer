#ifndef __EFFEKSEER_PROCEDUAL_MODEL_GENERATOR_H__
#define __EFFEKSEER_PROCEDUAL_MODEL_GENERATOR_H__

#include "../Effekseer.Base.h"
#include "../SIMD/Effekseer.Vec2f.h"
#include "../SIMD/Effekseer.Vec3f.h"
#include "../Utils/Effekseer.CustomAllocator.h"

namespace Effekseer
{

class ProcedualModelGenerator : public ReferenceObject
{
public:
	ProcedualModelGenerator() = default;
	virtual ~ProcedualModelGenerator() = default;

	virtual Model* Generate(const ProcedualModelParameter* parameter);

	virtual void Ungenerate(Model* model);
};

} // namespace Effekseer

#endif