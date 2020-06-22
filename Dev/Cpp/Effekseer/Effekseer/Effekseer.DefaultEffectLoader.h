
#ifndef	__EFFEKSEER_DEFAULTEFFECTLOADER_H__
#define	__EFFEKSEER_DEFAULTEFFECTLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.EffectLoader.h"
#include "Effekseer.DefaultFile.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	標準のエフェクトファイル読み込み破棄関数指定クラス
*/
class DefaultEffectLoader
	: public EffectLoader
{
	DefaultFileInterface m_defaultFileInterface;
	FileInterface* m_fileInterface;
public:

	DefaultEffectLoader( FileInterface* fileInterface = NULL );

	virtual ~DefaultEffectLoader();

	bool Load( const EFK_CHAR* path, void*& data, int32_t& size );

	void Unload( void* data, int32_t size );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_DEFAULTEFFECTLOADER_H__
