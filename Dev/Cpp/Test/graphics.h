
#ifndef	__GRAPHICS_H__
#define	__GRAPHICS_H__

#include "../Effekseer/Effekseer.h"

void InitGraphics( void* handle1, void* handle2, int width, int height );

void TermGraphics();

void Rendering();

void SetCameraMatrix( const ::Effekseer::Matrix44& matrix );

#endif
