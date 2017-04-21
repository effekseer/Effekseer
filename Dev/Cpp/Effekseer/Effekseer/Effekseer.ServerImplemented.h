
#ifndef	__EFFEKSEER_SERVER_IMPLEMENTED_H__
#define	__EFFEKSEER_SERVER_IMPLEMENTED_H__

#if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Server.h"

#include "Effekseer.Socket.h"
#include "Effekseer.Thread.h"
#include "Effekseer.CriticalSection.h"

#include <string>

#include <vector>
#include <set>
#include <map>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ServerImplemented : public Server
{
private:
	class InternalClient
	{
	public:
		Thread		m_threadRecv;
		EfkSocket	m_socket;
		ServerImplemented*		m_server;
		bool		m_active;

		std::vector<uint8_t>	m_recvBuffer;

		std::vector<std::vector<uint8_t> >	m_recvBuffers;
		CriticalSection						m_ctrlRecvBuffers;

		static void RecvAsync( void* data );

	public:
		InternalClient( EfkSocket socket_, ServerImplemented* server );
		~InternalClient();
		void ShutDown();
	};

private:
	EfkSocket	m_socket;
	uint16_t	m_port;

	Thread		m_thread;
	CriticalSection		m_ctrlClients;

	bool		m_running;

	std::set<InternalClient*>	m_clients;
	std::set<InternalClient*>	m_removedClients;

	std::map<std::wstring,Effect*>	m_effects;

	std::map<std::wstring,std::vector<uint8_t> >	m_data;

	std::vector<EFK_CHAR>	m_materialPath;

	void AddClient( InternalClient* client );
	void RemoveClient( InternalClient* client );
	static void AcceptAsync( void* data );

public:

	ServerImplemented();
	virtual ~ServerImplemented();

	/**
		@brief	サーバーを開始する。
	*/
	bool Start( uint16_t port );

	void Stop();

	void Regist( const EFK_CHAR* key, Effect* effect );

	void Unregist( Effect* effect );

	void Update();

	void SetMaterialPath( const EFK_CHAR* materialPath );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif	// #if !( defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE) )

#endif	// __EFFEKSEER_SERVER_IMPLEMENTED_H__
