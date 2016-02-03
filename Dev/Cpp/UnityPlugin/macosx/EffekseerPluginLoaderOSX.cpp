
#include <memory>
#include <string>
#include <map>
#include <algorithm>

#include "Effekseer.h"
#include "EffekseerRendererGL.h"

#include "../common/IUnityGraphics.h"
#include "../common/EffekseerPluginLoader.h"

using namespace Effekseer;

extern UnityGfxRenderer					g_RendererType;
extern EffekseerRenderer::Renderer*		g_EffekseerRenderer;

namespace EffekseerPlugin
{
	class TextureLoaderOSX : public TextureLoader
	{
		struct TextureResource {
			int referenceCount = 1;
			void* texture = nullptr;
		};
		std::map<std::u16string, TextureResource> resources;
	public:
		TextureLoaderOSX(
			TextureLoaderLoad load,
			TextureLoaderUnload unload) 
			: TextureLoader(load, unload) {}
		virtual ~TextureLoaderOSX() {}
		virtual void* Load( const EFK_CHAR* path, Effekseer::TextureType textureType ){
			auto it = resources.find((const char16_t*)path);
			if (it != resources.end()) {
				it->second.referenceCount++;
				return it->second.texture;
			} else {
				TextureResource res;
				res.texture = load( (const char16_t*)path );
				resources.insert( std::make_pair(
					(const char16_t*)path, res ) );
				return res.texture;
			}
			return nullptr;
		}
		virtual void Unload( void* source ){
			if (source == nullptr) {
				return;
			}
			for (auto it = resources.begin(); it != resources.end(); it++) {
				if (it->second.texture == source) {
					it->second.referenceCount--;
					if (it->second.referenceCount <= 0) {
						unload( it->first.c_str() );
						resources.erase(it);
					}
				}
			}
		}
	};

	TextureLoader* TextureLoader::Create(
		TextureLoaderLoad load,
		TextureLoaderUnload unload)
	{
		return new TextureLoaderOSX( load, unload );
	}

	class ModelLoaderOSX : public ModelLoader
	{
		ModelLoaderLoad load;
		ModelLoaderUnload unload;
		
		struct ModelResource {
			int referenceCount = 1;
			void* internalData;
		};
		std::map<std::u16string, ModelResource> resources;
		
		class MemoryFileReader : public Effekseer::FileReader {
			uint8_t* data;
			size_t length;
			int position;
		public:
			MemoryFileReader(uint8_t* data, size_t length): data(data), length(length) {}
			size_t Read( void* buffer, size_t size ) {
				if (size >= length - position) {
					size = length - position;
				}
				memcpy(buffer, &data[position], size);
				position += size;
				return size;
			}
			void Seek(int position) {
				this->position = position;
			}
			int GetPosition() {
				return position;
			}
			size_t GetLength() {
				return length;
			}
		};
		class MemoryFile : public Effekseer::FileInterface {
		public:
			std::vector<uint8_t> loadbuffer;
			size_t loadsize;
			FileReader* OpenRead( const EFK_CHAR* path ) {
				return new MemoryFileReader(&loadbuffer[0], loadsize);
			}
			FileWriter* OpenWrite( const EFK_CHAR* path ) {
				return nullptr;
			}
		};

		MemoryFile memoryFile;
		std::unique_ptr<Effekseer::ModelLoader> internalLoader;

	public:
		ModelLoaderOSX(
			ModelLoaderLoad load,
			ModelLoaderUnload unload ) 
			: ModelLoader( load, unload )
			, internalLoader( g_EffekseerRenderer->CreateModelLoader( &memoryFile ) ) {}
		virtual ~ModelLoaderOSX() {}
		virtual void* Load( const EFK_CHAR* path ){
			auto it = resources.find((const char16_t*)path);
			if (it != resources.end()) {
				it->second.referenceCount++;
				return it->second.internalData;
			} else {
				ModelResource res;
				int size = load( (const char16_t*)path, 
					&memoryFile.loadbuffer[0], (int)memoryFile.loadbuffer.size() );
				if (size <= 0) {
					return nullptr;
				}
				memoryFile.loadsize = (size_t)size;
				res.internalData = internalLoader->Load( path );
				resources.insert( std::make_pair(
					(const char16_t*)path, res ) );
				return res.internalData;
			}
			return nullptr;
		}
		virtual void Unload( void* source ){
			for (auto it = resources.begin(); it != resources.end(); it++) {
				if (it->second.internalData == source) {
					it->second.referenceCount--;
					if (it->second.referenceCount <= 0) {
						unload( it->first.c_str() );
						resources.erase(it);
					}
				}
			}
		}
	};
	
	ModelLoader* ModelLoader::Create(
		ModelLoaderLoad load,
		ModelLoaderUnload unload)
	{
		return new ModelLoaderOSX( load, unload );
	}
};