
#ifndef	__EFFEKSEERRENDERER_COMMON_UTILS_H__
#define	__EFFEKSEERRENDERER_COMMON_UTILS_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

template<typename TEXTURE>
inline TEXTURE TexturePointerToTexture(void* texture)
{
	return (TEXTURE)texture;
}

template <>
inline uint32_t TexturePointerToTexture<uint32_t>(void* texture)
{
	uint64_t texture_ = reinterpret_cast<uint64_t>(texture);
	return static_cast<uint32_t>(texture_);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
#endif // __EFFEKSEERRENDERER_COMMON_UTILS_H__