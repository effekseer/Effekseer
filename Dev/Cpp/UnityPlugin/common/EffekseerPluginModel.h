
#ifndef	__EFFEKSEER_PLUGIN_MODEL_H__
#define __EFFEKSEER_PLUGIN_MODEL_H__

#include <string>
#include <map>
#include <memory>
#include <Effekseer.h>
#include "EffekseerPluginCommon.h"

namespace EffekseerPlugin
{
	using ModelLoaderLoad = int (UNITY_API*)(const char16_t* path, void* buffer, int bufferSize);
	using ModelLoaderUnload = void (UNITY_API*)(const char16_t* path);

	class ModelLoader : public Effekseer::ModelLoader
	{
		ModelLoaderLoad load;
		ModelLoaderUnload unload;
		
		class MemoryFileReader : public Effekseer::FileReader {
			uint8_t* data;
			size_t length;
			int position;
		public:
			MemoryFileReader(uint8_t* data, size_t length);
			size_t Read( void* buffer, size_t size );
			void Seek( int position );
			int GetPosition();
			size_t GetLength();
		};

		class MemoryFile : public Effekseer::FileInterface {
		public:
			std::vector<uint8_t> loadbuffer;
			size_t loadsize = 0;
			explicit MemoryFile( size_t bufferSize );
			Effekseer::FileReader* OpenRead( const EFK_CHAR* path );
			Effekseer::FileWriter* OpenWrite( const EFK_CHAR* path );
		};

		struct ModelResource {
			int referenceCount = 1;
			void* internalData;
		};
		std::map<std::u16string, ModelResource> resources;
		MemoryFile memoryFile;
		std::unique_ptr<Effekseer::ModelLoader> internalLoader;

	private:
		ModelLoader(
			ModelLoaderLoad load,
			ModelLoaderUnload unload );
		
	public:
		static ModelLoader* Create(
			ModelLoaderLoad load,
			ModelLoaderUnload unload);
		
		virtual ~ModelLoader() = default;
		virtual void* Load( const EFK_CHAR* path );
		virtual void Unload( void* source );

		Effekseer::FileInterface* GetFileInterface() {
			return &memoryFile;
		}
		void SetInternalLoader( Effekseer::ModelLoader* loader ) {
			internalLoader.reset( loader );
		}
	};
	
}

#endif
