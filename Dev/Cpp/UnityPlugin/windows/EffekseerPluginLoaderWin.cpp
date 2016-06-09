
#include <memory>
#include <string>
#include <map>
#include <algorithm>

#pragma warning (disable : 4005)
#include "Effekseer.h"

#include "EffekseerRendererGL.h"
#include "EffekseerRendererDX9.h"
#include "EffekseerRendererDX11.h"

#include "../common/IUnityGraphics.h"
#include "../common/EffekseerPluginTexture.h"
#include "../common/EffekseerPluginModel.h"

#include "../opengl/EffekseerPluginLoaderGL.h"

using namespace Effekseer;

namespace EffekseerPlugin
{
	extern UnityGfxRenderer					g_UnityRendererType;
	extern ID3D11Device*					g_D3d11Device;
	extern EffekseerRenderer::Renderer*		g_EffekseerRenderer;

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
			// リソーステーブルを検索して存在したらそれを使う
			auto it = resources.find((const char16_t*)path);
			if (it != resources.end()) {
				it->second.referenceCount++;
				return it->second.texture;
			}

			// Unityでテクスチャをロード
			TextureResource res;
			res.texture = load( (const char16_t*)path );
			if (res.texture == nullptr)
			{
				return nullptr;
			}
			if (g_UnityRendererType == kUnityGfxRendererD3D11)
			{
				// DX11の場合、UnityがロードするのはID3D11Texture2Dなので、
				// ID3D11ShaderResourceViewを作成する
				HRESULT hr;
				ID3D11Texture2D* textureDX11 = (ID3D11Texture2D*)res.texture;
			
				D3D11_TEXTURE2D_DESC texDesc;
				textureDX11->GetDesc(&texDesc);
			
				ID3D11ShaderResourceView* srv = nullptr;
				D3D11_SHADER_RESOURCE_VIEW_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Format = texDesc.Format;
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MostDetailedMip = 0;
				desc.Texture2D.MipLevels = texDesc.MipLevels;
				hr = g_D3d11Device->CreateShaderResourceView(textureDX11, &desc, &srv);
				if (FAILED(hr))
				{
					return nullptr;
				}

				res.texture = srv;
			}
			// リソーステーブルに追加
			resources.insert( std::make_pair((const char16_t*)path, res ) );

			return res.texture;
		}
		virtual void Unload( void* source ){
			if (source == nullptr) {
				return;
			}

			// アンロードするテクスチャを検索
			auto it = std::find_if(resources.begin(), resources.end(), 
				[source](const std::pair<std::u16string, TextureResource>& pair){
					return pair.second.texture == source;
				});
			if (it == resources.end()) {
				return;
			}

			// 参照カウンタが0になったら実際にアンロード
			it->second.referenceCount--;
			if (it->second.referenceCount <= 0) {
				if (g_UnityRendererType == kUnityGfxRendererD3D11)
				{
					// 作成したID3D11ShaderResourceViewを解放する
					ID3D11ShaderResourceView* srv = (ID3D11ShaderResourceView*)source;
					srv->Release();
				}
				// Unity側でアンロード
				unload(it->first.c_str());
				resources.erase(it);
			}
		}
	};

	TextureLoader* TextureLoader::Create(
		TextureLoaderLoad load,
		TextureLoaderUnload unload)
	{
		return new TextureLoaderWin( load, unload );
	}

	ModelLoader* ModelLoader::Create(
		ModelLoaderLoad load,
		ModelLoaderUnload unload)
	{
		auto loader = new ModelLoader( load, unload );
		auto internalLoader = g_EffekseerRenderer->CreateModelLoader( loader->GetFileInterface() );
		loader->SetInternalLoader( internalLoader );
		return loader;
	}
};