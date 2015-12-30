
#ifndef	__EFFEKSEER_PLUGIN_LOADER_H__
#define __EFFEKSEER_PLUGIN_LOADER_H__

#include <Effekseer.h>
#include "EffekseerPluginCommon.h"

namespace EffekseerPlugin
{
	using TextureLoaderLoad = void* (*)(const char16_t* path);
	using TextureLoaderUnload = void (*)(const char16_t* path);

	class TextureLoader : public Effekseer::TextureLoader
	{
	protected:
		TextureLoaderLoad load;
		TextureLoaderUnload unload;

	public:
		static TextureLoader* Create(
			TextureLoaderLoad load,
			TextureLoaderUnload unload);
		TextureLoader(
			TextureLoaderLoad load,
			TextureLoaderUnload unload) 
			: load(load), unload(unload) {}
		virtual ~TextureLoader() {}
		virtual void* Load( const EFK_CHAR* path, Effekseer::TextureType textureType ) = 0;
		virtual void Unload( void* source ) = 0;
	};

	using ModelLoaderLoad = int (*)(const char16_t* path, void* buffer, int bufferSize);
	using ModelLoaderUnload = void (*)(const char16_t* path);

	class ModelLoader : public Effekseer::ModelLoader
	{
	protected:
		ModelLoaderLoad load;
		ModelLoaderUnload unload;
	
	public:
		static ModelLoader* Create(
			ModelLoaderLoad load,
			ModelLoaderUnload unload);
		ModelLoader(
			ModelLoaderLoad load,
			ModelLoaderUnload unload) 
			: load(load), unload(unload) {}
		virtual ~ModelLoader() {}
		virtual void* Load( const EFK_CHAR* path ) = 0;
		virtual void Unload( void* source ) = 0;
	};	
};

#endif
