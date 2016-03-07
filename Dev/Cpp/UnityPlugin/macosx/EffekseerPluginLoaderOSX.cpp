
#include <memory>
#include <string>
#include <map>
#include <algorithm>

#include "Effekseer.h"
#include "EffekseerRendererGL.h"

#include "../common/IUnityGraphics.h"
#include "../common/EffekseerPluginTexture.h"
#include "../common/EffekseerPluginModel.h"
#include "../../EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.ModelLoader.h"

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
		return new TextureLoaderOSX( load, unload );
	}
	
	ModelLoader* ModelLoader::Create(
		ModelLoaderLoad load,
		ModelLoaderUnload unload)
	{
		auto loader = new ModelLoader( load, unload );
		auto internalLoader = new EffekseerRendererGL::ModelLoader(loader->GetFileInterface());
		loader->SetInternalLoader( internalLoader );
		return loader;
	}
};