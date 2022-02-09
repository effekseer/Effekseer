#pragma once

#include <stdint.h>

#include "../IPC/IPC.h"

namespace Effekseer::Tool
{

class ProcessConnector
{
	std::shared_ptr<IPC::CommandQueue> commandQueueToMaterialEditor_;
	std::shared_ptr<IPC::CommandQueue> commandQueueFromMaterialEditor_;

	bool isUpdateMaterialRequired_ = false;

public:
	ProcessConnector();

	~ProcessConnector();

	void Update();

	void OpenOrCreateMaterial(const char16_t* path);

	void TerminateMaterialEditor();

	bool GetIsUpdateMaterialRequiredAndReset();
};

} // namespace Effekseer::Tool