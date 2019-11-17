#include "StaticFile.h"

namespace Effekseer
{

StaticFile::StaticFile(std::shared_ptr<FileReader>& reader) : reader_(reader) { reader_->GetData(buffer_); }

StaticFile::~StaticFile() {}

void* StaticFile::GetData() { return buffer_.data(); }

int StaticFile::GetSize() { return buffer_.size(); }

} // namespace Effekseer