
#include "fbxToEfkMdl.Base.h"

namespace fbxToEfkMdl
{
class VertexAnimationSetting
{
public:
	int32_t FrameCount;
};

/**
	@note
	it should be renamed. It uses for static mesh and animation.
*/
class VertexAnimation
{
private:
public:
	VertexAnimation() = default;
	virtual ~VertexAnimation() = default;

	void Export(const char* path, std::shared_ptr<Scene> scene, std::shared_ptr<AnimationClip> anim, float modelScale);
};
} // namespace fbxToEfkMdl