
#ifndef __EFFEKSEER_FILE_H__
#define __EFFEKSEER_FILE_H__

#include "Effekseer.Base.h"

namespace Effekseer
{

class FileReader;
class FileWriter;
class FileInterface;

using FileReaderRef = RefPtr<FileReader>;
using FileWriterRef = RefPtr<FileWriter>;
using FileInterfaceRef = RefPtr<FileInterface>;

class FileReader : public ReferenceObject
{
private:
public:
	FileReader() = default;
	virtual ~FileReader() override = default;

	virtual size_t Read(void* buffer, size_t size) = 0;

	virtual void Seek(int position) = 0;

	virtual int GetPosition() const = 0;

	virtual size_t GetLength() const = 0;
};

class FileWriter : public ReferenceObject
{
private:
public:
	FileWriter() = default;
	virtual ~FileWriter() override = default;

	virtual size_t Write(const void* buffer, size_t size) = 0;

	virtual void Flush() = 0;

	virtual void Seek(int position) = 0;

	virtual int GetPosition() const = 0;

	virtual size_t GetLength() const = 0;
};

/**
	@brief
	\~English	factory class for io
	\~Japanese	IOのためのファクトリークラス
*/
class FileInterface : public ReferenceObject
{
private:
public:
	FileInterface() = default;
	virtual ~FileInterface() override = default;

	virtual FileReaderRef OpenRead(const char16_t* path) = 0;

	/**
		@brief
		\~English	try to open a reader. It need not to succeeds in opening it.
		\~Japanese	リーダーを開くことを試します。成功する必要はありません。
	*/
	virtual FileReaderRef TryOpenRead(const char16_t* path)
	{
		return OpenRead(path);
	}

	virtual FileWriterRef OpenWrite(const char16_t* path) = 0;
};

} // namespace Effekseer

#endif // __EFFEKSEER_FILE_H__
