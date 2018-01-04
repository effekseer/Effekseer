
#include "fbxToEfkMdl.Base.h"

namespace fbxToEfkMdl
{
	class VertexAnimationSetting
	{
	public:
		int32_t	FrameCount;
	};

	class VertexAnimation
	{
	private:

	public:
		VertexAnimation() = default;
		virtual ~VertexAnimation() = default;

		void Export(const char* path, std::shared_ptr<Scene> scene);
	};
}