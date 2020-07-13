#ifdef _WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif

#include "RenderPassGL.h"
#include "efk.GraphicsGL.h"

namespace Effekseer
{
namespace Tool
{

RenderPassGL::RenderPassGL()
{
}

RenderPassGL::~RenderPassGL()
{
	if (frameBuffer_ > 0)
	{
		glDeleteFramebuffers(1, &frameBuffer_);
	}

	if (frameBufferSrc_ > 0)
	{
		glDeleteFramebuffers(1, &frameBufferSrc_);
	}

	if (frameBufferDst_ > 0)
	{
		glDeleteFramebuffers(1, &frameBufferDst_);
	}
}

bool RenderPassGL::Initialize(std::shared_ptr<efk::RenderTexture> colorTexture, std::shared_ptr<efk::DepthTexture> depthTexture, std::shared_ptr<efk::RenderTexture> resolvedColorTexture)
{
	colorTexture_ = colorTexture;
	depthTexture_ = depthTexture;
	resolvedColorTexture_ = resolvedColorTexture;

	glGenFramebuffers(1, &frameBuffer_);

	if (resolvedColorTexture_ != nullptr)
	{
		glGenFramebuffers(1, &frameBufferSrc_);
		glGenFramebuffers(1, &frameBufferDst_);
	}

	return true;
}

void RenderPassGL::Resolve()
{
	auto src = static_cast<efk::RenderTextureGL*>(colorTexture_.get());
	auto dst = static_cast<efk::RenderTextureGL*>(resolvedColorTexture_.get());

	GLint frameBufferBinding = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &frameBufferBinding);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferSrc_);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferDst_);
	GLCheckError();

	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, src->GetBuffer());
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dst->GetTexture(), 0);
	GLCheckError();

	glBlitFramebuffer(0, 0, src->GetSize().X, src->GetSize().Y, 0, 0, dst->GetSize().X, dst->GetSize().Y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLCheckError();

	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	GLCheckError();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferBinding);
}

} // namespace Tool
} // namespace Effekseer