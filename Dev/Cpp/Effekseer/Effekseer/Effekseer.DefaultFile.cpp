
#include "Effekseer.DefaultFile.h"
#include <assert.h>
#include <stdio.h>

namespace Effekseer
{

DefaultFileReader::DefaultFileReader(FILE* filePtr)
	: filePtr_(filePtr)
{
	assert(filePtr != nullptr);
}

DefaultFileReader::~DefaultFileReader()
{
	fclose(filePtr_);
}

size_t DefaultFileReader::Read(void* buffer, size_t size)
{
	return fread(buffer, 1, size, filePtr_);
}

void DefaultFileReader::Seek(int position)
{
	fseek(filePtr_, (size_t)position, SEEK_SET);
}

int DefaultFileReader::GetPosition() const
{
	return (int)ftell(filePtr_);
}

size_t DefaultFileReader::GetLength() const
{
	long position = ftell(filePtr_);
	fseek(filePtr_, 0, SEEK_END);
	long length = ftell(filePtr_);
	fseek(filePtr_, position, SEEK_SET);
	return (size_t)length;
}

DefaultFileWriter::DefaultFileWriter(FILE* filePtr)
	: filePtr_(filePtr)
{
	assert(filePtr != nullptr);
}

DefaultFileWriter::~DefaultFileWriter()
{
	fclose(filePtr_);
}

size_t DefaultFileWriter::Write(const void* buffer, size_t size)
{
	return fwrite(buffer, 1, size, filePtr_);
}

void DefaultFileWriter::Flush()
{
	fflush(filePtr_);
}

void DefaultFileWriter::Seek(int position)
{
	fseek(filePtr_, (size_t)position, SEEK_SET);
}

int DefaultFileWriter::GetPosition() const
{
	return (int)ftell(filePtr_);
}

size_t DefaultFileWriter::GetLength() const
{
	long position = ftell(filePtr_);
	fseek(filePtr_, 0, SEEK_END);
	long length = ftell(filePtr_);
	fseek(filePtr_, position, SEEK_SET);
	return (size_t)length;
}

FileReaderRef DefaultFileInterface::OpenRead(const char16_t* path)
{
	FILE* filePtr = nullptr;
#ifdef _WIN32
	_wfopen_s(&filePtr, (const wchar_t*)path, L"rb");
#else
	char path8[256];
	ConvertUtf16ToUtf8(path8, 256, path);
	filePtr = fopen(path8, "rb");
#endif

	if (filePtr == nullptr)
	{
		return nullptr;
	}

	return MakeRefPtr<DefaultFileReader>(filePtr);
}

FileWriterRef DefaultFileInterface::OpenWrite(const char16_t* path)
{
	FILE* filePtr = nullptr;
#ifdef _WIN32
	_wfopen_s(&filePtr, (const wchar_t*)path, L"wb");
#else
	char path8[256];
	ConvertUtf16ToUtf8(path8, 256, path);
	filePtr = fopen(path8, "wb");
#endif

	if (filePtr == nullptr)
	{
		return nullptr;
	}

	return MakeRefPtr<DefaultFileWriter>(filePtr);
}

} // namespace Effekseer
