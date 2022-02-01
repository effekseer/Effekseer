
#ifndef __EFFEKSEER_DEFAULT_FILE_H__
#define __EFFEKSEER_DEFAULT_FILE_H__

#include "Effekseer.Base.h"
#include "Effekseer.File.h"

namespace Effekseer
{

/**
	@brief	
	\~English	Default file loader
	\~Japanese	標準のファイル読み込みクラス
*/
class DefaultFileReader : public FileReader
{
private:
	FILE* m_filePtr;

public:
	DefaultFileReader(FILE* filePtr);

	~DefaultFileReader() override;

	size_t Read(void* buffer, size_t size);

	void Seek(int position);

	int GetPosition() const;

	size_t GetLength() const;
};

class DefaultFileWriter : public FileWriter
{
private:
	FILE* m_filePtr;

public:
	DefaultFileWriter(FILE* filePtr);

	~DefaultFileWriter() override;

	size_t Write(const void* buffer, size_t size);

	void Flush();

	void Seek(int position);

	int GetPosition() const;

	size_t GetLength() const;
};

class DefaultFileInterface : public FileInterface
{
public:
	FileReaderRef OpenRead(const char16_t* path);

	FileWriterRef OpenWrite(const char16_t* path);
};

} // namespace Effekseer

#endif // __EFFEKSEER_DEFAULT_FILE_H__
