
#ifndef	__EFFEKSEER_CLIENT_IMPLEMENTED_H__
#define	__EFFEKSEER_CLIENT_IMPLEMENTED_H__

#if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Client.h"

#include "Effekseer.Socket.h"
#include "Effekseer.Thread.h"
#include "Effekseer.CriticalSection.h"

#include <vector>
#include <set>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ClientImplemented : public Client
{
private:
	Thread		m_threadRecv;

	EfkSocket	m_socket;
	uint16_t	m_port;
	std::vector<uint8_t>	m_sendBuffer;

	bool		m_running;

	HOSTENT* GetHostEntry( const char* host );

	static void RecvAsync( void* data );
public:
	ClientImplemented();
	~ClientImplemented();

	bool Start( char* host, uint16_t port );
	void Stop();

	bool Send( void* data, int32_t datasize );

	void Reload( const EFK_CHAR* key, void* data, int32_t size );
	void Reload( Manager* manager, const EFK_CHAR* path, const EFK_CHAR* key );

	bool IsConnected();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif	// #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

#endif	// __EFFEKSEER_CLIENT_IMPLEMENTED_H__
