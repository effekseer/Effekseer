#include <algorithm>
#include "EffekseerPluginModel.h"

namespace EffekseerPlugin
{
	ModelLoader::MemoryFileReader::MemoryFileReader(uint8_t* data, size_t length)
		: data(data), length(length), position(0)
	{
	}

	size_t ModelLoader::MemoryFileReader::Read( void* buffer, size_t size )
	{
		if (size >= length - position) {
			size = length - position;
		}
		memcpy(buffer, &data[position], size);
		position += (int)size;
		return size;
	}

	void ModelLoader::MemoryFileReader::Seek( int position )
	{
		this->position = position;
	}
	int ModelLoader::MemoryFileReader::GetPosition()
	{
		return position;
	}
	size_t ModelLoader::MemoryFileReader::GetLength()
	{
		return length;
	}
	ModelLoader::MemoryFile::MemoryFile( size_t bufferSize ) {
		loadbuffer.resize(bufferSize);
	}
	Effekseer::FileReader* ModelLoader::MemoryFile::OpenRead( const EFK_CHAR* path ) {
		return new MemoryFileReader(&loadbuffer[0], loadsize);
	}
	Effekseer::FileWriter* ModelLoader::MemoryFile::OpenWrite( const EFK_CHAR* path ) {
		return nullptr;
	}

	ModelLoader::ModelLoader(
		ModelLoaderLoad load,
		ModelLoaderUnload unload ) 
		: load( load )
		, unload( unload )
		, memoryFile( 1 * 1024 * 1024 )
	{
	}
	void* ModelLoader::Load( const EFK_CHAR* path ){
		// リソーステーブルを検索して存在したらそれを使う
		auto it = resources.find((const char16_t*)path);
		if (it != resources.end()) {
			it->second.referenceCount++;
			return it->second.internalData;
		}

		// Unityでロード
		ModelResource res;
		int size = load( (const char16_t*)path, 
			&memoryFile.loadbuffer[0], (int)memoryFile.loadbuffer.size() );
		if (size <= 0) {
			return nullptr;
		}

		// 内部ローダに渡してロード処理する
		memoryFile.loadsize = (size_t)size;
		res.internalData = internalLoader->Load( path );
			
		resources.insert( std::make_pair(
			(const char16_t*)path, res ) );
		return res.internalData;
	}
	void ModelLoader::Unload( void* source ){
		if (source == nullptr) {
			return;
		}

		// アンロードするモデルを検索
		auto it = std::find_if(resources.begin(), resources.end(), 
			[source](const std::pair<std::u16string, ModelResource>& pair){
				return pair.second.internalData == source;
			});

		// 参照カウンタが0になったら実際にアンロード
		it->second.referenceCount--;
		if (it->second.referenceCount <= 0) {
			unload( it->first.c_str() );
			resources.erase(it);
		}
	}
}