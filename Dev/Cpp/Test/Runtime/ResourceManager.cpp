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

	auto texture1 = resourceManager->LoadTexture(u"Test", Effekseer::TextureType::Color);
	if (texture1 == nullptr)
	{
		throw std::string("Failed.");
	}
	auto texture2 = resourceManager->LoadTexture(u"Test", Effekseer::TextureType::Color);
	if (texture1 != texture2)
	{
        throw std::string("Failed.");
	}

	resourceManager->UnloadTexture(texture1);
	resourceManager->UnloadTexture(texture2);

	if (texture1->GetRef() != 2)
	{
        throw std::string("Failed.");
	}
	texture2.Reset();
	if (texture1->GetRef() != 1)
	{
        throw std::string("Failed.");
	}
	texture1.Reset();
}

TestRegister ResourceManager_Basic_Test("ResourceManager.Basic", []() -> void { ResourceManager_Basic(); });
