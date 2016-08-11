

#include "Effekseer.h"
#include "EffekseerRendererGL.h"

#include "../../EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.ModelLoader.h"

#include "EffekseerPluginLoaderGL.h"

using namespace Effekseer;

namespace EffekseerPlugin
{
	TextureLoaderGL::TextureLoaderGL(
		TextureLoaderLoad load,
		TextureLoaderUnload unload)
		: TextureLoader(load, unload)
	{}

	TextureLoaderGL::~TextureLoaderGL()
	{}

	void* TextureLoaderGL::Load(const EFK_CHAR* path, Effekseer::TextureType textureType)
	{
		// リソーステーブルを検索して存在したらそれを使う
		auto it = resources.find((const char16_t*) path);
		if (it != resources.end()) {
			it->second.referenceCount++;
			return it->second.texture;
		}

		// Unityでテクスチャをロード
		TextureResource res;
		res.texture = load((const char16_t*) path);
		if (res.texture == nullptr)
		{
			return nullptr;
		}
		// リソーステーブルに追加
		resources.insert(std::make_pair((const char16_t*) path, res));

#ifdef __EMSCRIPTEN__
		if (res.texture) {
			glBindTexture(GL_TEXTURE_2D, (GLuint)res.texture);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
#endif

		return res.texture;
	}

	void TextureLoaderGL::Unload(void* source)
	{
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

#ifdef _WIN32

#else
	TextureLoader* TextureLoader::Create(
		TextureLoaderLoad load,
		TextureLoaderUnload unload)
	{
		return new TextureLoaderGL( load, unload );
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
#endif
};