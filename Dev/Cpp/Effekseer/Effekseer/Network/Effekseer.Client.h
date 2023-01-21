
#ifndef __EFFEKSEER_CLIENT_H__
#define __EFFEKSEER_CLIENT_H__

#include "../Effekseer.Base.h"

namespace Effekseer
{

class Client;
using ClientRef = RefPtr<Client>;

class Client : public IReference
{
public:
	Client() = default;

	virtual ~Client() = default;

	static ClientRef Create();

	virtual bool Start(const char* host, uint16_t port) = 0;
	virtual void Stop() = 0;
	virtual void Update() = 0;

	virtual void Reload(const char16_t* key, void* data, int32_t size) = 0;
	virtual void Reload(ManagerRef manager, const char16_t* path, const char16_t* key) = 0;
	virtual bool IsConnected() const = 0;

	struct ProfileSample
	{
		bool IsValid = false;
		explicit operator bool() const { return IsValid; }

		struct Manager
		{
			uint32_t HandleCount = 0;
			float CPUTime = 0.0f;
			float GPUTime = 0.0f;
		};
		std::vector<Manager> Managers;

		struct Effect
		{
			std::u16string Key;
			uint32_t HandleCount = 0;
			float GPUTime = 0.0f;
		};
		std::vector<Effect> Effects;
	};
	virtual void StartProfiling() = 0;
	virtual void StopProfiling() = 0;
	virtual ProfileSample ReadProfileSample() = 0;
};

} // namespace Effekseer

#endif // __EFFEKSEER_CLIENT_H__
