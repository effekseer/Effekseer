
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.ClientImplemented.h"

#include "Effekseer.Manager.h"
#include "Effekseer.EffectLoader.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer {
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#ifdef _WIN32
static void Sleep_( int32_t ms )
{
	Sleep( ms );
}
#else
static void Sleep_( int32_t ms )
{
	usleep( 1000 * ms );
}
#endif
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static void PathCombine(EFK_CHAR* dst, const EFK_CHAR* src1, const EFK_CHAR* src2)
{
	int len1 = 0, len2 = 0;
	if( src1 != NULL )
	{
		for( len1 = 0; src1[len1] != L'\0'; len1++ ) {}
		memcpy( dst, src1, len1 * sizeof(EFK_CHAR) );
		if( len1 > 0 && src1[len1 - 1] != L'/' && src1[len1 - 1] != L'\\' )
		{
			dst[len1++] = L'/';
		}
	}
	if( src2 != NULL)
	{
		for( len2 = 0; src2[len2] != L'\0'; len2++ ) {}
		memcpy( &dst[len1], src2, len2 * sizeof(EFK_CHAR) );
	}
	dst[len1 + len2] = L'\0';
}

static void GetParentDir(EFK_CHAR* dst, const EFK_CHAR* src)
{
	int i, last = -1;
	for( i = 0; src[i] != L'\0'; i++ )
	{
		if( src[i] == L'/' || src[i] == L'\\' )
			last = i;
	}
	if( last >= 0 )
	{
		memcpy( dst, src, last * sizeof(EFK_CHAR) );
		dst[last] = L'\0';
	}
	else
	{
		dst[0] = L'\0';
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ClientImplemented::RecvAsync( void* data )
{
	ClientImplemented* client = (ClientImplemented*)data;

	while(true)
	{
		int32_t size = 0;
		int32_t restSize = 0;

		restSize = 4;
		while(restSize > 0)
		{
			int32_t recvSize = ::recv( client->m_socket, (char*)(&size), restSize, 0 );
			restSize -= recvSize;

			if( recvSize == 0 || recvSize == -1 )
			{
				/* 失敗 */
				client->Stop();
				return;
			}
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ClientImplemented::ClientImplemented()
	: m_running		( false )
{
	Socket::Initialize();
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ClientImplemented::~ClientImplemented()
{
	Stop();

	Socket::Finalize();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Client* Client::Create()
{
	return new ClientImplemented();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
HOSTENT* ClientImplemented::GetHostEntry( const char* host )
{
	HOSTENT* hostEntry = NULL;
	IN_ADDR InAddrHost;

	/* IPアドレスかDNSか調べる */
	InAddrHost.s_addr = ::inet_addr( host );
	if ( InAddrHost.s_addr == InaddrNone )
	{
		/* DNS */
		hostEntry = ::gethostbyname( host );
		if ( hostEntry == NULL )
		{
			return NULL;
		}
	}
	else
	{
		/* IPアドレス */
		hostEntry = ::gethostbyaddr( (const char*)(&InAddrHost), sizeof(IN_ADDR), AF_INET );
		if ( hostEntry == NULL )
		{
			return NULL;
		}
	}

	return hostEntry;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool ClientImplemented::Start( char* host, uint16_t port )
{
	if( m_running ) return false;

	SOCKADDR_IN sockAddr;
	HOSTENT* hostEntry= NULL;
	
	/* ソケット生成 */
	EfkSocket socket_ = Socket::GenSocket();
	if ( socket_ == InvalidSocket )
	{
		if ( socket_ != InvalidSocket ) Socket::Close( socket_ );
		return false;
	}

	/* ホスト情報取得 */
	hostEntry = GetHostEntry( host );
	if ( hostEntry == NULL )
	{
		if ( socket_ != InvalidSocket ) Socket::Close( socket_ );
		return false;
	}

	/* 接続用データ生成 */
	memset( &sockAddr, 0, sizeof(SOCKADDR_IN) );
	sockAddr.sin_family	= AF_INET;
	sockAddr.sin_port	= htons( port );
	sockAddr.sin_addr	= *(IN_ADDR*)(hostEntry->h_addr_list[0]);

	/* 接続 */
	int32_t ret = ::connect( socket_, (SOCKADDR*)(&sockAddr), sizeof(SOCKADDR_IN) );
	if ( ret == SocketError )
	{
		if ( socket_ != InvalidSocket ) Socket::Close( socket_ );
		return false;
	}

	m_socket = socket_;
	m_port = port;

	m_running = true;

	m_threadRecv.Create( RecvAsync, this );

	EffekseerPrintDebug("Client : Start\n");

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ClientImplemented::Stop()
{
	if( !m_running ) return;

	Socket::Shutsown( m_socket );
	Socket::Close( m_socket );
	m_running = false;

	EffekseerPrintDebug("Client : Stop\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool ClientImplemented::Send( void* data, int32_t datasize )
{
	if( !m_running ) return false;

	m_sendBuffer.clear();
	for( int32_t i = 0; i < sizeof(int32_t); i++ )
	{
		m_sendBuffer.push_back( ((uint8_t*)(&datasize))[i] );
	}

	for( int32_t i = 0; i < datasize; i++ )
	{
		m_sendBuffer.push_back( ((uint8_t*)(data))[i] );
	}

	int32_t size = m_sendBuffer.size();
	while( size > 0 )
	{
		int32_t ret = ::send( m_socket, (const char*)(&(m_sendBuffer[m_sendBuffer.size()-size])), size, 0 );
		if( ret == 0 || ret < 0 )
		{
			Stop();
			return false;
		}
		size -= ret;
	}

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ClientImplemented::Reload( const EFK_CHAR* key, void* data, int32_t size )
{
	int32_t keylen = 0;
	for( ; ; keylen++ )
	{
		if(key[keylen] == 0 ) break;
	}

	std::vector<uint8_t> buf;

	for( int32_t i = 0; i < sizeof(int32_t); i++ )
	{
		buf.push_back( ((uint8_t*)(&keylen))[i] );
	}

	for( int32_t i = 0; i < keylen * 2; i++ )
	{
		buf.push_back( ((uint8_t*)(key))[i] );
	}

	for( int32_t i = 0; i < size; i++ )
	{
		buf.push_back( ((uint8_t*)(data))[i] );
	}

	Send( &(buf[0]), buf.size() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ClientImplemented::Reload( Manager* manager, const EFK_CHAR* path, const EFK_CHAR* key )
{
	EffectLoader* loader = manager->GetEffectLoader();
	
	void* data = NULL;
	int32_t size = 0;

	if( !loader->Load( path, data, size ) ) return;

	Reload( key, data, size );

	loader->Unload( data, size );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool ClientImplemented::IsConnected()
{
	return m_running;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
