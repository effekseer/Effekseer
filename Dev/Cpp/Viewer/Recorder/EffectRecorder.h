#pragma

#include "../Graphics/efk.Graphics.h"
#include "../dll.h"
#include "RecorderCallback.h"

namespace Effekseer
{
namespace Tool
{
class RecorderCallback;

class EffectRecorder
{
private:
	std::shared_ptr<RecorderCallback> recorderCallback;
	std::shared_ptr<RecorderCallback> recorderCallback2;
	Effekseer::Tool::Vector2I imageSize_;

	bool isRecording = false;
	bool isRecordCompleted = false;
	RecordingParameter recordingParameter_ = {};
	RecordingParameter recordingParameter2_ = {};
	::Effekseer::Handle handle = 0;
	int iteratorCount = 1;
	int32_t currentTime = 0;
	int recordedCount = 0;
	bool completed = false;

	std::shared_ptr<Effekseer::Tool::EffectRenderer> generator_;
	std::shared_ptr<efk::Graphics> graphics_ = nullptr;

public:
#if !SWIG
	bool Begin(int32_t squareMaxCount,
			   Effekseer::Tool::EffectRendererParameter config,
			   Vector2I screenSize,
			   std::shared_ptr<efk::Graphics> graphics,
			   Effekseer::RefPtr<Effekseer::Setting> setting,
			   const RecordingParameter& recordingParameter,
			   Effekseer::Tool::Vector2I imageSize,
			   bool isSRGBMode,
			   Effekseer::Tool::ViewerEffectBehavior behavior,
			   Effekseer::Tool::PostEffectParameter postEffectParameter,
			   Effekseer::EffectRef effect);
#endif
	bool Step(int frames);

	bool End();

	bool IsCompleted() const;

	float GetProgress() const;
};

} // namespace Tool
} // namespace Effekseer