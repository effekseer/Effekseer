﻿
#ifndef __EFFEKSEER_RANDOM_H__
#define __EFFEKSEER_RANDOM_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.Pre.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	Random generator.
*/
class RandObject : public IRandObject
{
	int32_t m_seed = 0;

public:
	void SetSeed(int32_t seed);

	int32_t GetRandInt();

	float GetRand();

	float GetRand(float min_, float max_);
};

/**
	@brief	Random generator callback.
*/
class RandCallback
{
public:
	static float Rand(void* userData);

	static float RandSeed(void* userData, float seed);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_VECTOR3D_H__
