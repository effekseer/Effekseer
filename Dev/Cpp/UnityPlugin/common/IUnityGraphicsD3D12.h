#pragma once
#include "IUnityInterface.h"
#ifndef __cplusplus
	#include <stdbool.h>
#endif

// Should only be used on the rendering thread unless noted otherwise.
UNITY_DECLARE_INTERFACE(IUnityGraphicsD3D12)
{
	ID3D12Device* (UNITY_INTERFACE_API * GetDevice)();
	ID3D12CommandQueue* (UNITY_INTERFACE_API * GetCommandQueue)();

	ID3D12Fence* (UNITY_INTERFACE_API * GetFrameFence)();
	// Returns the value set on the frame fence once the current frame completes
	UINT64 (UNITY_INTERFACE_API * GetNextFrameFenceValue)();

	// Returns the state a resource will be in after the last command list is executed
	bool (UNITY_INTERFACE_API * GetResourceState)(ID3D12Resource* resource, D3D12_RESOURCE_STATES* outState);
	// Specifies the state a resource will be in after a plugin command list with resource barriers is executed
	void (UNITY_INTERFACE_API * SetResourceState)(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
};
UNITY_REGISTER_INTERFACE_GUID(0xEF4CEC88A45F4C4CULL,0xBD295B6F2A38D9DEULL,IUnityGraphicsD3D12)
