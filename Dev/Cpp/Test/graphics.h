
#ifndef	__GRAPHICS_H__
#define	__GRAPHICS_H__

#include "../Effekseer/Effekseer.h"

void InitGraphics( HWND handle, int width, int height );

void TermGraphics();

void Rendering();

void SetCameraMatrix( const ::Effekseer::Matrix44& matrix );

#endif
