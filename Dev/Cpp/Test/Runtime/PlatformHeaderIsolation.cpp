#define NOMINMAX
#include "TestPlatforms.h"
#include <RenderingEnvironment/RenderingEnvironment.h>

// This translation unit is a compile-time regression test: including platform
// interfaces must not expose X11 types or macros to the rest of the tests.
#ifdef __linux__
#if defined(_X11_XLIB_H_) || defined(None) || defined(Always) || defined(Bool) || defined(Status) || defined(XNegative) || defined(YNegative)
#error Test platform headers must not expose X11 headers or macros
#endif
#endif

// These declarations have previously collided with X11 macros.
#include <EffekseerToolRuntime/PostEffects.h>
#include "../../Effekseer/Effekseer/Effekseer.EffectNode.h"
