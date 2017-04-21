
#ifndef	__EFFEKSEER_BASE_H__
#define	__EFFEKSEER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.Pre.h"

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <map>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <queue>
#include <chrono>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
#define	EFK_STDCALL	__stdcall
#else
#define	EFK_STDCALL
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef uint16_t			EFK_CHAR;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct Color;

class Manager;
class ManagerImplemented;
class Effect;
class EffectImplemented;
class EffectNode;
class EffectNodeRoot;
class EffectNodeSprite;
class EffectNodeRibbon;
class EffectNodeModel;

class InstanceGlobal;
class InstanceContainer;
class Instance;
class InstanceGroup;

class ParticleRenderer;
class SpriteRenderer;
class RibbonRenderer;
class ModelRenderer;
class TrackRenderer;

class Setting;
class FileReader;
class FileWriter;
class FileInterface;
class EffectLoader;
class TextureLoader;
class SoundLoader;
class ModelLoader;

class Model;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _DEBUG_EFFEKSEER
#define EffekseerPrintDebug(...)	printf(__VA_ARGS__)
#else
#define EffekseerPrintDebug(...)
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	インスタンスの状態
*/
enum eInstanceState
{
	/**
		@brief	正常動作中
	*/
	INSTANCE_STATE_ACTIVE,

	/**
		@brief	削除中
	*/
	INSTANCE_STATE_REMOVING,
	/**
		@brief	削除
	*/
	INSTANCE_STATE_REMOVED,

	INSTANCE_STATE_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	エフェクトに所属するノードの種類
*/
enum eEffectNodeType
{
	EFFECT_NODE_TYPE_ROOT = -1,
	EFFECT_NODE_TYPE_NONE = 0,
	EFFECT_NODE_TYPE_SPRITE = 2,
	EFFECT_NODE_TYPE_RIBBON = 3,
	EFFECT_NODE_TYPE_RING = 4,
	EFFECT_NODE_TYPE_MODEL = 5,
	EFFECT_NODE_TYPE_TRACK = 6,

	EFFECT_NODE_TYPE_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_BASE_H__