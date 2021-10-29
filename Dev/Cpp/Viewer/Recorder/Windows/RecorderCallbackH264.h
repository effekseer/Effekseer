#pragma once

#include "../RecorderCallback.h"
#include <memory>

namespace Effekseer
{
namespace Tool
{

class VideoWriter;

class RecorderCallbackH264 : public RecorderCallback
{
private:
	std::shared_ptr<VideoWriter> videoWriter_;

public:
	bool OnBeginRecord() override
	{
		return false;
	}

	virtual void OnEndRecord() override
	{
	}

	virtual void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels)
	{
	}
};

} // namespace Tool
} // namespace Effekseer