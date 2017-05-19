
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
void* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
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

			return (void*) texture;
		}
		else if (data_texture[0] == 'D' &&
			data_texture[1] == 'D' &&
			data_texture[2] == 'S' &&
			data_texture[3] == ' ')
		{
			struct DDS_PIXELFORMAT {
				uint32_t dwSize;
				uint32_t dwFlags;
				uint32_t dwFourCC;
				uint32_t dwRGBBitCount;
				uint32_t dwRBitMask;
				uint32_t dwGBitMask;
				uint32_t dwBBitMask;
				uint32_t dwABitMask;
			};

			struct DDS_HEADER {
				uint32_t dwSize;
				uint32_t dwFlags;
				uint32_t dwHeight;
				uint32_t dwWidth;
				uint32_t dwPitchOrLinearSize;
				uint32_t dwDepth;
				uint32_t dwMipMapCount;
				uint32_t dwReserved1[11];
				DDS_PIXELFORMAT ddspf;
				uint32_t dwCaps1;
				uint32_t dwCaps2;
				uint32_t dwReserved2[3];
			};

			const uint32_t FOURCC_DXT1 = 0x31545844; //(MAKEFOURCC('D','X','T','1'))
			const uint32_t FOURCC_DXT3 = 0x33545844; //(MAKEFOURCC('D','X','T','3'))
			const uint32_t FOURCC_DXT5 = 0x35545844; //(MAKEFOURCC('D','X','T','5'))

			uint8_t* p = (uint8_t*) data_texture;
			p += 4;

			DDS_HEADER dds;
			memcpy(&dds, p, sizeof(DDS_HEADER));
			p += sizeof(DDS_HEADER);

			if (dds.dwMipMapCount != 0)
			{
				delete [] data_texture;
				return nullptr;
			}

			uint32_t format = 0;

			if (dds.ddspf.dwRGBBitCount == 32 &&
				dds.ddspf.dwRBitMask == 0x000000FF &&
				dds.ddspf.dwGBitMask == 0x0000FF00 &&
				dds.ddspf.dwBBitMask == 0x00FF0000 &&
				dds.ddspf.dwABitMask == 0xFF000000)
			{
				GLuint texture = 0;

				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);

				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RGBA,
					dds.dwWidth,
					dds.dwHeight,
					0,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					p);

				// Generate mipmap
				GLExt::glGenerateMipmap(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, 0);

				delete[] data_texture;
				return (void*)texture;
			}

			if (dds.ddspf.dwFourCC == FOURCC_DXT1)
			{
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			}
			else if (dds.ddspf.dwFourCC == FOURCC_DXT3)
			{
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			}
			else if (dds.ddspf.dwFourCC == FOURCC_DXT5)
			{
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			}
			else
			{
				delete [] data_texture;
				return nullptr;
			}

			{
				GLuint texture = 0;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);

				GLExt::glCompressedTexImage2D(
					GL_TEXTURE_2D, 
					0, 
					format,
					dds.dwWidth,
					dds.dwHeight,
					0, 
					size_texture - 4 - sizeof(DDS_HEADER),
					p);

				// Generate mipmap
				GLExt::glGenerateMipmap(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, 0);

				delete [] data_texture;
				return (void*) texture;
			}
		}
	}

	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TextureLoader::Unload( void* data )
{
	if( data != NULL )
	{
		GLuint texture = EffekseerRenderer::TexturePointerToTexture <GLuint> (data);
		glDeleteTextures(1, &texture);
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
