
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <memory>
#include <png.h>
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.TextureLoader.h"
#include "EffekseerRenderer.GLExtension.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{

static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
{
	::Effekseer::FileReader* reader = (::Effekseer::FileReader*)png_get_io_ptr(png_ptr);
	reader->Read( data, length );
}

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
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::~TextureLoader()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* TextureLoader::Load( const EFK_CHAR* path )
{
	std::auto_ptr<Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	
	if( reader.get() != NULL )
	{
		// pngアクセス構造体を作成
		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
		// リードコールバック関数指定
		png_set_read_fn(png, reader.get(), &PngReadData);
	
		// png画像情報構造体を作成
		png_infop png_info = png_create_info_struct(png);

		// IHDRチャンク情報を取得
		png_read_info(png, png_info);
		
		// RGBA8888フォーマットに変換する
		if (png_info->bit_depth < 8) {
			png_set_packing(png);
		} else if (png_info->bit_depth == 16) {
			png_set_strip_16(png);
		}
		
		uint32_t pixelBytes = 4;
		GLint glFormat = GL_RGBA;
		switch (png_info->color_type) {
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(png);
			glFormat = GL_RGB;
			break;
		case PNG_COLOR_TYPE_GRAY:
			png_set_expand_gray_1_2_4_to_8(png);
			glFormat = GL_RGB;
			break;
		case PNG_COLOR_TYPE_RGB:
			pixelBytes = 3;
			glFormat = GL_RGB;
			break;
		case PNG_COLOR_TYPE_RGBA:
			break;
		}

		uint8_t* image = new uint8_t[png_info->width * png_info->height * pixelBytes];
		uint32_t pitch = png_info->width * pixelBytes;

		/* イメージデータを読み込む */
		for (uint32_t i = 0; i < png_info->height; i++) {
			png_read_row(png, &image[i * pitch], NULL);
		}
		
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, glFormat, 
			png_info->width, png_info->height, 0, glFormat, GL_UNSIGNED_BYTE, image);
		
		/* ミップマップの生成 */
		GLExt::glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		delete [] image;
		png_destroy_read_struct(&png, &png_info, NULL);

		return (void*)texture;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TextureLoader::Unload( void* data )
{
	if( data != NULL )
	{
		GLuint texture = (GLuint)data;
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