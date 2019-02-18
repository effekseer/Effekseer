
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
	pngTextureLoader.Initialize();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::~TextureLoader()
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	pngTextureLoader.Finalize();
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
			pngTextureLoader.Load(data_texture, size_texture, false);
			delete [] data_texture;

			GLuint texture = 0;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				pngTextureLoader.GetWidth(),
				pngTextureLoader.GetHeight(),
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				pngTextureLoader.GetData().data());

			// Generate mipmap
			GLExt::glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
			pngTextureLoader.Unload();

			auto textureData = new Effekseer::TextureData();
			textureData->UserPtr = nullptr;
			textureData->UserID = texture;
			textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
			textureData->Width = pngTextureLoader.GetWidth();
			textureData->Height = pngTextureLoader.GetHeight();
			return textureData;
		}
		else if (data_texture[0] == 'D' &&
			data_texture[1] == 'D' &&
			data_texture[2] == 'S' &&
			data_texture[3] == ' ')
		{
			if (ddsTextureLoader.Load(data_texture, size_texture))
			{
				delete[] data_texture;

				if (ddsTextureLoader.GetTextureFormat() == Effekseer::TextureFormatType::ABGR8)
				{
					GLuint texture = 0;

					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);

					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RGBA,
						ddsTextureLoader.GetWidth(),
						ddsTextureLoader.GetHeight(),
						0,
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						ddsTextureLoader.GetData().data());

					// Generate mipmap
					GLExt::glGenerateMipmap(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, 0);

					delete[] data_texture;

					auto textureData = new Effekseer::TextureData();
					textureData->UserPtr = nullptr;
					textureData->UserID = texture;
					textureData->TextureFormat = ddsTextureLoader.GetTextureFormat();
					textureData->Width = ddsTextureLoader.GetWidth();
					textureData->Height = ddsTextureLoader.GetHeight();
					return textureData;
				}
				else
				{
					uint32_t format = 0;

					if (ddsTextureLoader.GetTextureFormat() == Effekseer::TextureFormatType::BC1)
					{
						format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
					}
					else if (ddsTextureLoader.GetTextureFormat() == Effekseer::TextureFormatType::BC2)
					{
						format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
					}
					else if (ddsTextureLoader.GetTextureFormat() == Effekseer::TextureFormatType::BC3)
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
						ddsTextureLoader.GetWidth(),
						ddsTextureLoader.GetHeight(),
						0,
						ddsTextureLoader.GetData().size(),
						ddsTextureLoader.GetData().data());

					// Generate mipmap
					GLExt::glGenerateMipmap(GL_TEXTURE_2D);

					glBindTexture(GL_TEXTURE_2D, 0);

					auto textureData = new Effekseer::TextureData();
					textureData->UserPtr = nullptr;
					textureData->UserID = texture;
					textureData->TextureFormat = ddsTextureLoader.GetTextureFormat();
					textureData->Width = ddsTextureLoader.GetWidth();
					textureData->Height = ddsTextureLoader.GetHeight();
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
		GLuint texture = (GLuint)data->UserID;
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
