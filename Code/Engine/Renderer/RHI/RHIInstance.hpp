#pragma once

#include "Engine/Renderer/RHI/DX11.hpp"

class RHIInstance;  // System level singleton
class RHIDevice;     
class RHIDeviceContext;    
class RHIOutput;


#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

class RHIInstance
{
public:
	// Constructor is private:  Singleton Class
	~RHIInstance();
	HMODULE debug_module;
	IDXGIDebug *debug;

	bool RHIInstance::CreateOutput(RHIDevice** out_device, RHIDeviceContext** out_context, RHIOutput** out_output, unsigned int const px_width, unsigned int const px_height);

private:
	RHIInstance();

public:
	static RHIInstance* GetInstance();
	static void ReportLiveObjects();
	static void SetDebugName(const char* name, ID3D11DeviceChild* d3dObject);
};