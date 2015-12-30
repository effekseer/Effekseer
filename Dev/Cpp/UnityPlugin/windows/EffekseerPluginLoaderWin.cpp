
#include <memory>
#include <string>
#include <map>
#include <algorithm>

#pragma warning (disable : 4005)
#include "Effekseer.h"
#include "EffekseerRendererDX9.h"
#include "EffekseerRendererDX11.h"

#include "../common/IUnityGraphics.h"
#include "../common/EffekseerPluginLoader.h"

using namespace Effekseer;

extern UnityGfxRenderer				g_RendererType;
extern IDirect3DDevice9*				g_D3d9Device;
extern ID3D11Device*					g_D3d11Device;
extern ID3D11DeviceContext*			g_D3d11Context;
extern EffekseerRenderer::Renderer*		g_EffekseerRenderer;

namespace EffekseerPlugin
{
	class TextureLoaderWin : public TextureLoader
	{
		struct TextureResource {
			int referenceCount = 1;
			void* texture = nullptr;
		};
		std::map<std::u16string, TextureResource> resources;
	public:
		TextureLoaderWin(
			TextureLoaderLoad load,
			TextureLoaderUnload unload) 
			: TextureLoader(load, unload) {}
		virtual ~TextureLoaderWin() {}
		virtual void* Load( const EFK_CHAR* path, Effekseer::TextureType textureType ){
			auto it = resources.find((const char16_t*)path);
			if (it != resources.end()) {
				it->second.referenceCount++;
				return it->second.texture;
			} else {
				TextureResource res;
				res.texture = load( (const char16_t*)path );
				if (g_RendererType == kUnityGfxRendererD3D11)
				{
					// UnityがロードするのはID3D11Texture2Dなので、
					// ID3D11ShaderResourceViewに変換する
					HRESULT hr;
					ID3D11Texture2D* textureDX11 = (ID3D11Texture2D*)res.texture;
				
					D3D11_TEXTURE2D_DESC texDesc;
					textureDX11->GetDesc(&texDesc);
				
					ID3D11ShaderResourceView* resView = nullptr;
					D3D11_SHADER_RESOURCE_VIEW_DESC desc;
					ZeroMemory(&desc, sizeof(desc));
					desc.Format = texDesc.Format;
					desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					desc.Texture2D.MostDetailedMip = 0;
					desc.Texture2D.MipLevels = texDesc.MipLevels;
					hr = g_D3d11Device->CreateShaderResourceView(textureDX11, &desc, &resView);
					if (FAILED(hr))
					{
						return nullptr;
					}

					res.texture = resView;
				}
				resources.insert( std::make_pair(
					(const char16_t*)path, res ) );
				return res.texture;
			}
			return nullptr;
		}
		virtual void Unload( void* source ){
			for (auto it = resources.begin(); it != resources.end(); it++) {
				if (it->second.texture == source) {
					it->second.referenceCount--;
					if (it->second.referenceCount <= 0) {
						if (g_RendererType == kUnityGfxRendererD3D11)
						{
							// ID3D11Texture2Dに戻してUnityにアンロードしてもらう
							ID3D11Resource* resourceDX11 = nullptr;
							ID3D11ShaderResourceView* resView = (ID3D11ShaderResourceView*)source;
							resView->GetResource(&resourceDX11);
							resView->Release();
						}
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
		return new TextureLoaderWin( load, unload );
	}

	class ModelLoaderWin : public ModelLoader
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
		ModelLoaderWin(
			ModelLoaderLoad load,
			ModelLoaderUnload unload ) 
			: ModelLoader( load, unload )
			, internalLoader( g_EffekseerRenderer->CreateModelLoader( &memoryFile ) ) {}
		virtual ~ModelLoaderWin() {}
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
		return new ModelLoaderWin( load, unload );
	}
};