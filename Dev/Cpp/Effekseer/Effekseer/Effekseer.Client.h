
#ifndef __EFFEKSEER_CLIENT_H__
#define __EFFEKSEER_CLIENT_H__

#include "Effekseer.Base.h"

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

	virtual bool Start(char* host, uint16_t port) = 0;
	virtual void Stop() = 0;

	virtual void Reload(const char16_t* key, void* data, int32_t size) = 0;
	virtual void Reload(ManagerRef manager, const char16_t* path, const char16_t* key) = 0;
	virtual bool IsConnected() = 0;
};

} // namespace Effekseer

#endif // __EFFEKSEER_CLIENT_H__
