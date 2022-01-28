#pragma once

#include <Effekseer.h>
#include <stdint.h>

namespace Effekseer::Tool
{
class Image
{
protected:
	Effekseer::Backend::TextureRef texture_;

public:
	int32_t GetWidth() const
	{
		return texture_->GetParameter().Size[0];
	}
	int32_t GetHeight() const
	{
		return texture_->GetParameter().Size[1];
	}

#if !defined(SWIG)
	Effekseer::Backend::TextureRef GetTexture()
	{
		return texture_;
	}
#endif
};

} // namespace Effekseer::Tool