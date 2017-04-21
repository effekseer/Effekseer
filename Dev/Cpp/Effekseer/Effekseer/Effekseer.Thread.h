
#ifndef	__EFFEKSEER_THREAD_H__
#define	__EFFEKSEER_THREAD_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.CriticalSection.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
	
class Thread
{
private:
#ifdef _WIN32
	/* DWORDを置きかえ */
	static unsigned long EFK_STDCALL ThreadProc(void* arguments);
#elif defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE)

#else
	static void* ThreadProc( void* arguments );
#endif

private:
#ifdef _WIN32
	HANDLE m_thread;
#elif defined(_PSVITA) || defined(_PS4) || defined(_SWITCH) || defined(_XBOXONE)

#else
	pthread_t m_thread;
	bool m_running;
#endif

	void* m_data;
	void (*m_mainProc)( void* );
	CriticalSection m_cs;

public:

	Thread();
	~Thread();


	/**
		@brief スレッドを生成する。
		@param threadFunc	[in] スレッド関数
		@param pData		[in] スレッドに引き渡すデータポインタ
		@return	成否
	*/
	bool Create( void (*threadFunc)( void* ), void* data );

	/**
		@brief スレッド終了を確認する。
	*/
	bool IsExitThread() const;

	/**
		@brief スレッド終了を待つ。
	*/
	bool Wait() const;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_VECTOR3D_H__
