
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.RenderState.h"

#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.RendererImplemented.h"

#include "EffekseerRendererGL.GLExtension.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::RenderState(RendererImplemented* renderer)
	: renderer_(renderer)
{
	if (renderer_->GetDeviceType() == OpenGLDeviceType::OpenGL3 || renderer_->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		GLExt::glGenSamplers(Effekseer::TextureSlotMax, samplers_.data());
	}

	GLint frontFace = 0;
	glGetIntegerv(GL_FRONT_FACE, &frontFace);

	if (GL_CW == frontFace)
	{
		isCCW_ = false;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::~RenderState()
{
	if (renderer_->GetDeviceType() == OpenGLDeviceType::OpenGL3 || renderer_->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		GLExt::glDeleteSamplers(Effekseer::TextureSlotMax, samplers_.data());
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderState::Update(bool forced)
{
	GLCheckError();

	if (active_.DepthTest != next_.DepthTest || forced)
	{
		if (next_.DepthTest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}

	GLCheckError();

	if (active_.DepthWrite != next_.DepthWrite || forced)
	{
		glDepthMask(next_.DepthWrite);
	}

	GLCheckError();

	if (active_.CullingType != next_.CullingType || forced)
	{
		if (isCCW_)
		{
			if (next_.CullingType == Effekseer::CullingType::Front)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
			}
			else if (next_.CullingType == Effekseer::CullingType::Back)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
			else if (next_.CullingType == Effekseer::CullingType::Double)
			{
				glDisable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
			}
		}
		else
		{
			if (next_.CullingType == Effekseer::CullingType::Front)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
			else if (next_.CullingType == Effekseer::CullingType::Back)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
			}
			else if (next_.CullingType == Effekseer::CullingType::Double)
			{
				glDisable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
			}
		}
	}

	GLCheckError();

	if (active_.AlphaBlend != next_.AlphaBlend || forced)
	{
		{
			glEnable(GL_BLEND);

			if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Opacity)
			{
				GLExt::glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);
				GLExt::glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ONE);
			}
			else if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Sub)
			{
				GLExt::glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
				GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
			}
			else
			{
				GLExt::glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Blend)
				{
					if (renderer_->GetImpl()->IsPremultipliedAlphaEnabled)
					{
						GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
					}
					else
					{
						GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
					}
				}
				else if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Add)
				{
					if (renderer_->GetImpl()->IsPremultipliedAlphaEnabled)
					{
						GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
					}
					else
					{
						GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
					}
				}
				else if (next_.AlphaBlend == ::Effekseer::AlphaBlendType::Mul)
				{
					GLExt::glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ZERO, GL_ONE);
				}
			}
		}
	}

	GLCheckError();

	static const GLint glfilterMin[] = {GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR};
	static const GLint glfilterMin_NoneMipmap[] = {GL_NEAREST, GL_LINEAR};
	static const GLint glfilterMag[] = {GL_NEAREST, GL_LINEAR};
	static const GLint glwrap[] = {GL_REPEAT, GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT};

	if (renderer_->GetDeviceType() == OpenGLDeviceType::OpenGL3 || renderer_->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		for (int32_t i = 0; i < (int32_t)renderer_->GetCurrentTextures().size(); i++)
		{
			// If a texture is not assigned, skip it.
			const auto& texture = renderer_->GetCurrentTextures()[i];
			if (texture == nullptr)
				continue;

			if (active_.TextureFilterTypes[i] != next_.TextureFilterTypes[i] || forced || active_.TextureIDs[i] != next_.TextureIDs[i])
			{
				GLExt::glActiveTexture(GL_TEXTURE0 + i);

				// for webngl
#ifndef NDEBUG
				// GLint bound = 0;
				// glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
				// assert(bound > 0);
#endif

				int32_t filter_ = (int32_t)next_.TextureFilterTypes[i];

				GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_MAG_FILTER, glfilterMag[filter_]);

				if (texture->GetParameter().MipLevelCount != 1)
				{
					GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_MIN_FILTER, glfilterMin[filter_]);
				}
				else
				{
					GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_MIN_FILTER, glfilterMin_NoneMipmap[filter_]);
				}

				// glSamplerParameteri( samplers_[i],  GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				// glSamplerParameteri( samplers_[i],  GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				GLExt::glBindSampler(i, samplers_[i]);
			}

			if (active_.TextureWrapTypes[i] != next_.TextureWrapTypes[i] || forced || active_.TextureIDs[i] != next_.TextureIDs[i])
			{
				GLExt::glActiveTexture(GL_TEXTURE0 + i);

				int32_t wrap = static_cast<int32_t>(next_.TextureWrapTypes[i]);
				GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_WRAP_S, glwrap[wrap]);
				GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_WRAP_T, glwrap[wrap]);

				GLExt::glBindSampler(i, samplers_[i]);
			}
		}
	}
	else
	{
		GLCheckError();
		for (int32_t i = 0; i < (int32_t)renderer_->GetCurrentTextures().size(); i++)
		{
			// If a texture is not assigned, skip it.
			const auto& texture = renderer_->GetCurrentTextures()[i];
			if (texture == nullptr)
				continue;

			// always changes because a flag is assigned into a texture
			// if (active_.TextureFilterTypes[i] != next_.TextureFilterTypes[i] || forced)
			{
				GLExt::glActiveTexture(GL_TEXTURE0 + i);
				GLCheckError();

				// for webngl
#ifndef NDEBUG
				GLint bound = 0;
				glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
				assert(bound > 0);
#endif

				int32_t filter_ = (int32_t)next_.TextureFilterTypes[i];

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glfilterMag[filter_]);
				GLCheckError();

				if (texture->GetParameter().MipLevelCount != 1)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glfilterMin[filter_]);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glfilterMin_NoneMipmap[filter_]);
				}
			}

			// always changes because a flag is assigned into a texture
			// if (active_.TextureWrapTypes[i] != next_.TextureWrapTypes[i] || forced)
			{
				GLExt::glActiveTexture(GL_TEXTURE0 + i);
				GLCheckError();

				int32_t wrap_ = (int32_t)next_.TextureWrapTypes[i];

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glwrap[wrap_]);
				GLCheckError();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glwrap[wrap_]);
				GLCheckError();
			}
		}
		GLCheckError();
	}

	GLExt::glActiveTexture(GL_TEXTURE0);

	active_ = next_;

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
