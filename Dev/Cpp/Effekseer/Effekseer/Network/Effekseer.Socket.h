
#ifndef __EFFEKSEER_SOCKET_H__
#define __EFFEKSEER_SOCKET_H__

#if !(defined(_PSVITA) || defined(_XBOXONE))

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <stdint.h>
#include <atomic>

#if defined(_WIN32) && !defined(_PS4)
#define EfkWinSock
#else
#define EfkBSDSock
#endif

#if defined(EfkWinSock)

#ifdef __EFFEKSEER_FOR_UE4__
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCKAPI_
#define NOMINMAX
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#ifdef __EFFEKSEER_FOR_UE4__
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#else

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#if !defined(_PS4)
#include <netdb.h>
#endif

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class Socket
{
public:
#if defined(EfkWinSock)
	using SockHandle = SOCKET;
	using SockLen = int;
	using InAddr = IN_ADDR;
	using Hostent = HOSTENT;
	using SockAddrIn = SOCKADDR_IN;
	using SockAddr = SOCKADDR;
	
	static const SockHandle InvalidHandle = INVALID_SOCKET;
	static const int32_t SocketError = SOCKET_ERROR;
	static const int32_t InaddrNone = INADDR_NONE;
#elif defined(EfkBSDSock)
	using SockHandle = int32_t;
	using SockLen = socklen_t;
	using Hostent = struct hostent;
	using InAddr = struct in_addr;
	using SockAddrIn = struct sockaddr_in;
	using SockAddr = struct sockaddr;

	static const SockHandle InvalidHandle = -1;
	static const int32_t SocketError = -1;
	static const int32_t InaddrNone = -1;
#endif

private:
	static void Initialize();

	static void Finalize();

public:
	Socket();

	Socket(SockHandle handle, const SockAddrIn& sockAddr);

	~Socket();

	Socket(const Socket&) = delete;

	Socket& operator=(const Socket&) = delete;

	Socket(Socket&& rhs);

	Socket& operator=(Socket&& rhs);

	bool IsValid() const { return handle_ != InvalidHandle; }

	void Close();

	bool Connect(const char* host, int32_t port);

	bool Listen(int32_t port, int32_t backlog);
	
	Socket Accept();

	int32_t Send(const void* data, int32_t size);

	int32_t Recv(void* buffer, int32_t size);

private:
	SockHandle handle_ = InvalidHandle;
	SockAddrIn sockAddr_ = {};
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif // #if !( defined(_PSVITA) || defined(_XBOXONE) )

#endif // __EFFEKSEER_SOCKET_H__
