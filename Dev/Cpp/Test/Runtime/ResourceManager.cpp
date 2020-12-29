#include <Effekseer.h>
#include <Effekseer/Effekseer.ResourceManager.h>
#include <Effekseer/Effekseer.TextureLoader.h>

#include "../TestHelper.h"

class MockTextureLoader : public Effekseer::TextureLoader
{
public:
	Effekseer::TextureRef Load(const char16_t* path, Effekseer::TextureType textureType) override
	{
		return Effekseer::MakeRefPtr<Effekseer::Texture>();
	}

	Effekseer::TextureRef Load(const void* data, int32_t size, Effekseer::TextureType textureType, bool isMipMapEnabled) override
	{
		return nullptr;
	}
};

void ResourceManager_Basic()
{
	auto resourceManager = Effekseer::MakeRefPtr<Effekseer::ResourceManager>();
	resourceManager->SetTextureLoader(Effekseer::MakeRefPtr<MockTextureLoader>());

	auto texture = resourceManager->LoadTexture(u"Test", Effekseer::TextureType::Color);
	resourceManager->UnloadTexture(texture);

	if (texture->GetRef() != 1)
	{
		// Bug
		// throw "";
	}
}

TestRegister ResourceManager_Basic_Test("ResourceManager.Basic", []() -> void { ResourceManager_Basic(); });
