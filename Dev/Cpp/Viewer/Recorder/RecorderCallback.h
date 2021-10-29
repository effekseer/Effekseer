#pragma once

#include <Effekseer.h>
#include <vector>

namespace Effekseer
{
namespace Tool
{
class RecorderCallback
{
private:
public:
	RecorderCallback() = default;
	virtual ~RecorderCallback() = default;

	virtual bool OnBeginRecord()
	{
		return false;
	}

	virtual void OnEndRecord()
	{
	}

	virtual void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels)
	{
	}
};

} // namespace Tool
} // namespace Effekseer