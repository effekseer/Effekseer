﻿
#include "Effekseer.DefaultFile.h"
#include <assert.h>
#include <stdio.h>

namespace Effekseer
{

DefaultFileReader::DefaultFileReader(FILE* filePtr)
	: m_filePtr(filePtr)
{
	assert(filePtr != nullptr);
}

DefaultFileReader::~DefaultFileReader()
{
	fclose(m_filePtr);
}

size_t DefaultFileReader::Read(void* buffer, size_t size)
{
	return fread(buffer, 1, size, m_filePtr);
}

void DefaultFileReader::Seek(int position)
{
	fseek(m_filePtr, (size_t)position, SEEK_SET);
}

int DefaultFileReader::GetPosition() const
{
	return (int)ftell(m_filePtr);
}

size_t DefaultFileReader::GetLength() const
{
	long position = ftell(m_filePtr);
	fseek(m_filePtr, 0, SEEK_END);
	long length = ftell(m_filePtr);
	fseek(m_filePtr, position, SEEK_SET);
	return (size_t)length;
}

DefaultFileWriter::DefaultFileWriter(FILE* filePtr)
	: m_filePtr(filePtr)
{
	assert(filePtr != nullptr);
}

DefaultFileWriter::~DefaultFileWriter()
{
	fclose(m_filePtr);
}

size_t DefaultFileWriter::Write(const void* buffer, size_t size)
{
	return fwrite(buffer, 1, size, m_filePtr);
}

void DefaultFileWriter::Flush()
{
	fflush(m_filePtr);
}

void DefaultFileWriter::Seek(int position)
{
	fseek(m_filePtr, (size_t)position, SEEK_SET);
}

int DefaultFileWriter::GetPosition() const
{
	return (int)ftell(m_filePtr);
}

size_t DefaultFileWriter::GetLength() const
{
	long position = ftell(m_filePtr);
	fseek(m_filePtr, 0, SEEK_END);
	long length = ftell(m_filePtr);
	fseek(m_filePtr, position, SEEK_SET);
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
