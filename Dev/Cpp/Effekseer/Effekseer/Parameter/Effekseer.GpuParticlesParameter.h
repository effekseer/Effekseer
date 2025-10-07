
#ifndef __EFFEKSEER_GPU_PARTICLES_PARAMETER_H__
#define __EFFEKSEER_GPU_PARTICLES_PARAMETER_H__

#include "../Renderer/Effekseer.GpuParticles.h"

namespace Effekseer
{

GpuParticles::ParamSet LoadGpuParticlesParameter(uint8_t*& pos, int32_t version, float magnification, CoordinateSystem coordinateSystem);

} // namespace Effekseer

#endif // __EFFEKSEER_GPU_PARTICLES_PARAMETER_H__
