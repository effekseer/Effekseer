#pragma once

#include "../../Math/Vector2I.h"
#include "../RecorderCallback.h"
#include "../RecordingParameter.h"

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

	RecordingParameter& recordingParameter_;
	Effekseer::Tool::Vector2I imageSize_;
	std::vector<uint8_t> buffer_;

public:
	RecorderCallbackH264(RecordingParameter& recordingParameter, Effekseer::Tool::Vector2I imageSize)
		: recordingParameter_(recordingParameter)
		, imageSize_(imageSize)
	{
	}

	virtual ~RecorderCallbackH264() = default;

	bool OnBeginRecord() override;

	void OnEndRecord() override;

	void OnEndFrameRecord(int index, std::vector<Effekseer::Color>& pixels);
};

} // namespace Tool
} // namespace Effekseer