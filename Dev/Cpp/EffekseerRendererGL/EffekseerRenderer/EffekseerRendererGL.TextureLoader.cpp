
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <memory>
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.TextureLoader.h"
#include "EffekseerRendererGL.GLExtension.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::TextureLoader( ::Effekseer::FileInterface* fileInterface )
	: m_fileInterface	( fileInterface )
{
	if( m_fileInterface == NULL )
	{
		m_fileInterface = &m_defaultFileInterface;
	}

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	EffekseerRenderer::PngTextureLoader::Initialize();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::~TextureLoader()
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	EffekseerRenderer::PngTextureLoader::Finalize();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effekseer::TextureData* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	std::unique_ptr<Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	
	if( reader.get() != NULL )
	{
		size_t size_texture = reader->GetLength();
		char* data_texture = new char[size_texture];
		reader->Read(data_texture, size_texture);

		if (size_texture < 4)
		{
		}
		else if (data_texture[1] == 'P' &&
			data_texture[2] == 'N' &&
			data_texture[3] == 'G')
		{
			EffekseerRenderer::PngTextureLoader::Load(data_texture, size_texture, false);
			delete [] data_texture;

			GLuint texture = 0;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				EffekseerRenderer::PngTextureLoader::GetWidth(),
				EffekseerRenderer::PngTextureLoader::GetHeight(),
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				EffekseerRenderer::PngTextureLoader::GetData().data());

			// Generate mipmap
			GLExt::glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
			EffekseerRenderer::PngTextureLoader::Unload();

			auto textureData = new Effekseer::TextureData();
			textureData->UserPtr = nullptr;
			textureData->UserID = texture;
			textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
			textureData->Width = EffekseerRenderer::PngTextureLoader::GetWidth();
			textureData->Height = EffekseerRenderer::PngTextureLoader::GetHeight();
			return textureData;
		}
		else if (data_texture[0] == 'D' &&
			data_texture[1] == 'D' &&
			data_texture[2] == 'S' &&
			data_texture[3] == ' ')
		{
			if (EffekseerRenderer::DDSTextureLoader::Load(data_texture, size_texture))
			{
				delete[] data_texture;

				if (EffekseerRenderer::DDSTextureLoader::GetTextureFormat() == Effekseer::TextureFormatType::ABGR8)
				{
					GLuint texture = 0;

					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);

					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RGBA,
						EffekseerRenderer::DDSTextureLoader::GetWidth(),
						EffekseerRenderer::DDSTextureLoader::GetHeight(),
						0,
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						EffekseerRenderer::DDSTextureLoader::GetData().data());

					// Generate mipmap
					GLExt::glGenerateMipmap(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, 0);

					delete[] data_texture;

					auto textureData = new Effekseer::TextureData();
					textureData->UserPtr = nullptr;
					textureData->UserID = texture;
					textureData->TextureFormat = EffekseerRenderer::DDSTextureLoader::GetTextureFormat();
					textureData->Width = EffekseerRenderer::DDSTextureLoader::GetWidth();
					textureData->Height = EffekseerRenderer::DDSTextureLoader::GetHeight();
					return textureData;
				}
				else
				{
					uint32_t format = 0;

					if (EffekseerRenderer::DDSTextureLoader::GetTextureFormat() == Effekseer::TextureFormatType::BC1)
					{
						format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
					}
					else if (EffekseerRenderer::DDSTextureLoader::GetTextureFormat() == Effekseer::TextureFormatType::BC2)
					{
						format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
					}
					else if (EffekseerRenderer::DDSTextureLoader::GetTextureFormat() == Effekseer::TextureFormatType::BC3)
					{
						format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					}

					GLuint texture = 0;
					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);

					GLExt::glCompressedTexImage2D(
						GL_TEXTURE_2D,
						0,
						format,
						EffekseerRenderer::DDSTextureLoader::GetWidth(),
						EffekseerRenderer::DDSTextureLoader::GetHeight(),
						0,
						EffekseerRenderer::DDSTextureLoader::GetData().size(),
						EffekseerRenderer::DDSTextureLoader::GetData().data());

					// Generate mipmap
					GLExt::glGenerateMipmap(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, 0);

					auto textureData = new Effekseer::TextureData();
					textureData->UserPtr = nullptr;
					textureData->UserID = texture;
					textureData->TextureFormat = EffekseerRenderer::DDSTextureLoader::GetTextureFormat();
					textureData->Width = EffekseerRenderer::DDSTextureLoader::GetWidth();
					textureData->Height = EffekseerRenderer::DDSTextureLoader::GetHeight();
					return textureData;
				}
			}
			else
			{
				delete[] data_texture;
			}
		}
	}

	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TextureLoader::Unload(Effekseer::TextureData* data )
{
	if( data != NULL )
	{
		GLuint texture = data->UserID;
		glDeleteTextures(1, &texture);
	}

	if (data != nullptr)
	{
		delete data;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif
