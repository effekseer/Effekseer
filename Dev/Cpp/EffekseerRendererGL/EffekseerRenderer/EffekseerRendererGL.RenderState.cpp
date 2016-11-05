
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
RenderState::RenderState( RendererImplemented* renderer )
	: m_renderer	( renderer )
{
	if (m_renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3 || m_renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		GLExt::glGenSamplers(4, m_samplers);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
RenderState::~RenderState()
{
	if (m_renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3 || m_renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		GLExt::glDeleteSamplers(4, m_samplers);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void RenderState::Update( bool forced )
{
	GLCheckError();

	if( m_active.DepthTest != m_next.DepthTest || forced )
	{
		if( m_next.DepthTest )
		{
			glEnable( GL_DEPTH_TEST );
		}
		else
		{
			glDisable( GL_DEPTH_TEST );
		}
	}

	GLCheckError();

	if( m_active.DepthWrite != m_next.DepthWrite || forced )
	{
		glDepthMask( m_next.DepthWrite );
	}

	GLCheckError();

	if( m_active.CullingType != m_next.CullingType || forced )
	{
		if( m_next.CullingType == Effekseer::CullingType::Front )
		{
			glEnable( GL_CULL_FACE );
			glCullFace( GL_FRONT );
		}
		else if (m_next.CullingType == Effekseer::CullingType::Back)
		{
			glEnable( GL_CULL_FACE );
			glCullFace( GL_BACK );
		}
		else if( m_next.CullingType == Effekseer::CullingType::Double )
		{
			glDisable( GL_CULL_FACE );
			glCullFace( GL_FRONT_AND_BACK );
		}
	}

	GLCheckError();

	if( m_active.AlphaBlend != m_next.AlphaBlend || forced )
	{
		if(  m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Opacity )
		{
			glDisable( GL_BLEND );
		}
		else
		{
			glEnable( GL_BLEND );

			if( m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Sub )
			{
				GLExt::glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
				GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
			}
			else
			{
				GLExt::glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
				if( m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Blend )
				{
					GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
				}
				else if( m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Add )
				{
					GLExt::glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
				}
				else if( m_next.AlphaBlend == ::Effekseer::AlphaBlendType::Mul )
				{
					GLExt::glBlendFuncSeparate(GL_ZERO, GL_SRC_COLOR, GL_ONE, GL_ONE);
				}
			}
		}
	}

	GLCheckError();
	
	static const GLint glfilterMin[] = { GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR };
	static const GLint glfilterMag[] = { GL_NEAREST, GL_LINEAR };
	static const GLint glwrap[] = { GL_REPEAT, GL_CLAMP_TO_EDGE };

	if (m_renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3 || m_renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3)
	{
		for (int32_t i = 0; i < 4; i++)
		{
			if (m_active.TextureFilterTypes[i] != m_next.TextureFilterTypes[i] || forced)
			{
				GLExt::glActiveTexture(GL_TEXTURE0 + i);

				int32_t filter_ = (int32_t) m_next.TextureFilterTypes[i];

				GLExt::glSamplerParameteri(m_samplers[i], GL_TEXTURE_MAG_FILTER, glfilterMag[filter_]);
				GLExt::glSamplerParameteri(m_samplers[i], GL_TEXTURE_MIN_FILTER, glfilterMin[filter_]);
				//glSamplerParameteri( m_samplers[i],  GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				//glSamplerParameteri( m_samplers[i],  GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				GLExt::glBindSampler(i, m_samplers[i]);
			}

			if (m_active.TextureWrapTypes[i] != m_next.TextureWrapTypes[i] || forced)
			{
				GLExt::glActiveTexture(GL_TEXTURE0 + i);

				int32_t wrap_ = (int32_t) m_next.TextureWrapTypes[i];
				GLExt::glSamplerParameteri(m_samplers[i], GL_TEXTURE_WRAP_S, glwrap[wrap_]);
				GLExt::glSamplerParameteri(m_samplers[i], GL_TEXTURE_WRAP_T, glwrap[wrap_]);

				GLExt::glBindSampler(i, m_samplers[i]);
			}
		}
	}
	else
	{
		GLCheckError();
		for (int32_t i = 0; i < m_renderer->GetCurrentTextures().size(); i++)
		{
			/* テクスチャが設定されていない場合はスキップ */
			if (m_renderer->GetCurrentTextures()[i] == 0) continue;

			if (m_active.TextureFilterTypes[i] != m_next.TextureFilterTypes[i] || forced)
			{
				GLExt::glActiveTexture(GL_TEXTURE0 + i);
				GLCheckError();

				int32_t filter_ = (int32_t) m_next.TextureFilterTypes[i];

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glfilterMag[filter_]);
				GLCheckError();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glfilterMin[filter_]);
				GLCheckError();
			}

			if (m_active.TextureWrapTypes[i] != m_next.TextureWrapTypes[i] || forced)
			{
				GLExt::glActiveTexture(GL_TEXTURE0 + i);
				GLCheckError();

				int32_t wrap_ = (int32_t) m_next.TextureWrapTypes[i];

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glwrap[wrap_]);
				GLCheckError();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glwrap[wrap_]);
				GLCheckError();
			}
		}
		GLCheckError();
	}

	GLExt::glActiveTexture( GL_TEXTURE0 );
	
	m_active = m_next;

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
