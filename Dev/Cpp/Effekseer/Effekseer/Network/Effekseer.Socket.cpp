
#if (defined(__EFFEKSEER_NETWORK_ENABLED__))

#include "Effekseer.Socket.h"

#if defined(_WIN32) && !defined(_PS4)
#else
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
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
	: handle_(handle)
	, sockAddr_(sockAddr)
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

	// Set socket to non-blocking mode
#if defined(EfkWinSock)
	u_long val = 1;
	ioctlsocket(handle_, FIONBIO, &val);
#elif defined(EfkBSDSock)
	int flags = fcntl(handle_, F_GETFL, 0);
	fcntl(handle_, F_SETFL, flags | O_NONBLOCK);
#endif

	// connect to the remort host
	SockAddrIn sockAddr = {};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr = addr;

	int32_t ret = ::connect(handle_, (SockAddr*)(&sockAddr), sizeof(SockAddrIn));

	if (ret == SocketError)
	{
#if defined(EfkWinSock)
		int error = ::WSAGetLastError();
		if (error != WSAEWOULDBLOCK)
#elif defined(EfkBSDSock)
		if (errno != EINPROGRESS)
#endif
		{
			// If connection is not in progress, connection failed
			Close();
			return false;
		}
		else
		{
			// Wait for connection to complete or timeout
			fd_set wfds;
			FD_ZERO(&wfds);
			FD_SET(handle_, &wfds);

			struct timeval tv;
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			int selectRet = 0;
#if defined(EfkWinSock)
			selectRet = ::select(0, nullptr, &wfds, nullptr, &tv);
			if (selectRet == 0 || selectRet == SocketError)
#elif defined(EfkBSDSock)
			selectRet = ::select(handle_ + 1, nullptr, &wfds, nullptr, &tv);
			if (selectRet == 0 || selectRet < 0)
#endif
			{
				// Timeout or Socket error
				Close();
				return false;
			}

			// Check connection
			int so_error = 0;
#if defined(EfkWinSock)
			int len = sizeof(so_error);
			if (::getsockopt(handle_, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len) == SocketError || so_error != 0)
#elif defined(EfkBSDSock)
			socklen_t len = sizeof(so_error);
			if (::getsockopt(handle_, SOL_SOCKET, SO_ERROR, &so_error, &len) < 0 || so_error != 0)
#endif
			{
				Close();
				return false;
			}
		}
	}

	// Set socket back to blocking mode for Send/Recv operations
#if defined(EfkWinSock)
	val = 0;
	::ioctlsocket(handle_, FIONBIO, &val);
#elif defined(EfkBSDSock)
	flags = fcntl(handle_, F_GETFL, 0);
	::fcntl(handle_, F_SETFL, flags & ~O_NONBLOCK);
#endif

	sockAddr_ = sockAddr;

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

	// #if defined(EfkWinSock)
	//	u_long val = 1;
	//	ioctlsocket(retHandle, FIONBIO, &val);
	// #elif defined(EfkBSDSock)
	//	int val = 1;
	//	ioctl(retHandle, FIONBIO, &val);
	// #endif

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