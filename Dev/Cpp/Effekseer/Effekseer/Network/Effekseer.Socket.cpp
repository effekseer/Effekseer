
#if (defined(__EFFEKSEER_NETWORK_ENABLED__))

#include "Effekseer.Socket.h"

#if defined(_WIN32) && !defined(_PS4)
#else
#include <sys/socket.h>
#include <sys/types.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Socket::Initialize()
{
#if defined(EfkWinSock)
	// Initialize  Winsock
	WSADATA m_WsaData;
	::WSAStartup(MAKEWORD(2, 0), &m_WsaData);
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Socket::Finalize()
{
#if defined(EfkWinSock)
	// Dispose winsock or decrease a counter
	WSACleanup();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Socket::Socket()
{
	Initialize();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Socket::Socket(SockHandle handle, const SockAddrIn& sockAddr)
	: handle_(handle), sockAddr_(sockAddr)
{
	Initialize();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Socket::~Socket()
{
	Close();
	Finalize();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Socket::Socket(Socket&& rhs)
{
	handle_ = rhs.handle_;
	rhs.handle_ = InvalidHandle;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Socket& Socket::operator=(Socket&& rhs)
{
	handle_ = rhs.handle_;
	rhs.handle_ = InvalidHandle;
	return *this;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Socket::Close()
{
	if (handle_ == InvalidHandle)
	{
		return;
	}

#if defined(EfkWinSock)
	::shutdown(handle_, SD_BOTH);
#elif defined(EfkBSDSock)
	::shutdown(handle_, SHUT_RDWR);
#endif

#if defined(EfkWinSock)
	::closesocket(handle_);
#elif defined(EfkBSDSock)
	::close(handle_);
#endif

	handle_ = InvalidHandle;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Socket::Connect(const char* host, int32_t port)
{
	InAddr addr = {};

	// check ip adress or DNS
	addr.s_addr = ::inet_addr(host);
	if (addr.s_addr == InaddrNone)
	{
		// DNS
		Hostent* hostEntry = ::gethostbyname(host);
		if (hostEntry == nullptr)
		{
			return false;
		}

		addr.s_addr = *(unsigned int*)hostEntry->h_addr_list[0];
	}

	// create a socket
	handle_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// connect to the remort host
	SockAddrIn sockAddr = {};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr = addr;

	int32_t ret = ::connect(handle_, (SockAddr*)(&sockAddr), sizeof(SockAddrIn));
	if (ret == SocketError)
	{
		Close();
		return false;
	}

	sockAddr_ = sockAddr;

//#if defined(EfkWinSock)
//	u_long val = 1;
//	ioctlsocket(handle_, FIONBIO, &val);
//#elif defined(EfkBSDSock)
//	int val = 1;
//	ioctl(handle_, FIONBIO, &val);
//#endif

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Socket::Listen(int32_t port, int32_t backlog)
{
	// create a socket
	handle_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// bind the port
	SockAddrIn sockAddr = {};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);

	int32_t ret = ::bind(handle_, (SockAddr*)&sockAddr, sizeof(SockAddrIn));
	if (ret == SocketError)
	{
		Close();
		return false;
	}

	// listen the socket
	ret = ::listen(handle_, backlog);
#if defined(EfkWinSock)
	if (ret == SocketError)
#elif defined(EfkBSDSock)
	if (ret < 0)
#endif
	{
		Close();
		return false;
	}

	sockAddr_ = sockAddr;

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Socket Socket::Accept()
{
	SockAddrIn sockAddr = {};
	SockLen size = sizeof(SockAddrIn);

	SockHandle retHandle = ::accept(handle_, (SockAddr*)(&sockAddr), (SockLen*)(&size));
	if (retHandle == InvalidHandle)
	{
		return Socket();
	}

//#if defined(EfkWinSock)
//	u_long val = 1;
//	ioctlsocket(retHandle, FIONBIO, &val);
//#elif defined(EfkBSDSock)
//	int val = 1;
//	ioctl(retHandle, FIONBIO, &val);
//#endif

	return Socket(retHandle, sockAddr);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t Socket::Send(const void* data, int32_t size)
{
	return ::send(handle_, (const char*)data, size, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t Socket::Recv(void* buffer, int32_t size)
{
	return ::recv(handle_, (char*)buffer, size, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif