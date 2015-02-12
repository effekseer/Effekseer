
#ifndef	__GRAPHICS_H__
#define	__GRAPHICS_H__

#include "../Effekseer/Effekseer.h"

void InitGraphics( int width, int height );

void TermGraphics();

void Rendering();

bool DoEvent();

void SetCameraMatrix( const ::Effekseer::Matrix44& matrix );

#endif
