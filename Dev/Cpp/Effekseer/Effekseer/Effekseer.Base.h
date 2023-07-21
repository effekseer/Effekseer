
#ifndef __EFFEKSEER_BASE_H__
#define __EFFEKSEER_BASE_H__

#include "Effekseer.Base.Pre.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <array>
#include <cfloat>
#include <chrono>
#include <iterator>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <set>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#define EFK_STDCALL __stdcall
#else
#define EFK_STDCALL
#endif

typedef char16_t EFK_CHAR;

namespace Effekseer
{

struct Color;

class ManagerImplemented;
class EffectImplemented;
class EffectNodeImplemented;
class EffectNodeRoot;
class EffectNodeSprite;
class EffectNodeRibbon;
class EffectNodeModel;

class InstanceGlobal;
class InstanceContainer;
class Instance;
class InstanceChunk;
class InstanceGroup;

class FileReader;
class FileWriter;
class FileInterface;
class InternalScript;

#ifdef _DEBUG_EFFEKSEER
#define EffekseerPrintDebug(...) printf(__VA_ARGS__)
#else
#define EffekseerPrintDebug(...)
#endif

/**
	@brief	A state of instances
*/
enum class eInstanceState : int32_t
{
	/**
		@brief	Active
	*/
	INSTANCE_STATE_ACTIVE,

	/**
		@brief	Removing (Fadeout)
	*/
	INSTANCE_STATE_REMOVING,

	/**
		@brief	Removed
	*/
	INSTANCE_STATE_REMOVED,

	/**
		@brief	Destroyed
	*/
	INSTANCE_STATE_DISPOSING,
};

enum class ModelReferenceType : int32_t
{
	File,
	Procedural,
};

enum class BindType : int32_t
{
	NotBind = 0,
	NotBind_Root = 3,
	WhenCreating = 1,
	Always = 2,
};

} // namespace Effekseer

#endif // __EFFEKSEER_BASE_H__
