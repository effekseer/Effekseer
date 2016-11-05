
#ifndef	__EFFEKSEER_FILE_H__
#define	__EFFEKSEER_FILE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer { 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	ファイル読み込みクラス
*/
class FileReader
{
private:

public:
	FileReader() {}

	virtual ~FileReader() {}

	virtual size_t Read( void* buffer, size_t size ) = 0;

	virtual void Seek(int position) = 0;

	virtual int GetPosition() = 0;

	virtual size_t GetLength() = 0;
};

/**
	@brief	ファイル書き込みクラス
*/
class FileWriter
{
private:

public:
	FileWriter() {}

	virtual ~FileWriter() {}

	virtual size_t Write( const void* buffer, size_t size ) = 0;

	virtual void Flush() = 0;

	virtual void Seek(int position) = 0;

	virtual int GetPosition() = 0;

	virtual size_t GetLength() = 0;
};

/**
	@brief	ファイルアクセス用のファクトリクラス
*/
class FileInterface
{
private:

public:
	virtual FileReader* OpenRead( const EFK_CHAR* path ) = 0;

	virtual FileWriter* OpenWrite( const EFK_CHAR* path ) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
 } 
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_FILE_H__
