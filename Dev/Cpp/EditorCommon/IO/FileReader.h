#pragma once

#include <string>
#include <vector>

namespace Effekseer
{

enum class SeekOrigin
{
	Begin,
	Current,
	End
};

class FileReader
{
protected:
	std::u16string path_;
	int64_t position_;
	int64_t length_;

public:
	FileReader(const std::u16string& path);
	virtual ~FileReader();

	int64_t GetPosition() const { return position_; }
	const std::u16string& GetPath() { return path_; }

	virtual int64_t GetSize() = 0;

	virtual void ReadBytes(std::vector<uint8_t>& buffer, const int64_t size) = 0;
	void ReadAllBytes(std::vector<uint8_t>& buffer) { ReadBytes(buffer, GetSize()); }

	virtual void Seek(const int64_t offset, const SeekOrigin origin = SeekOrigin::Begin) = 0;
};

} // namespace Effekseer