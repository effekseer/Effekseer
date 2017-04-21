
#ifndef	__EFFEKSEER_SOCKET_H__
#define	__EFFEKSEER_SOCKET_H__

#if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#ifdef _WIN32

typedef SOCKET	EfkSocket;
typedef int		SOCKLEN;
const EfkSocket InvalidSocket = INVALID_SOCKET;
const int32_t SocketError = SOCKET_ERROR;
const int32_t InaddrNone = INADDR_NONE;

#else

typedef int32_t	EfkSocket;
typedef socklen_t SOCKLEN;
const EfkSocket InvalidSocket = -1;
const int32_t SocketError = -1;
const int32_t InaddrNone = -1;

typedef struct hostent HOSTENT;
typedef struct in_addr IN_ADDR;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#endif

#ifdef _WIN32
static void Sleep_(int32_t ms)
{
	Sleep(ms);
}
#else
static void Sleep_(int32_t ms)
{
	usleep(1000 * ms);
}
#endif


class Socket
{
private:

public:
	static void Initialize();
	static void Finalize();

	static EfkSocket GenSocket();

	static void Close( EfkSocket s );
	static void Shutsown( EfkSocket s );

	static bool Listen( EfkSocket s, int32_t backlog );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif	// #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

#endif	// __EFFEKSEER_SOCKET_H__
