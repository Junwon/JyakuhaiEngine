#include "Engine/Renderer/RHI/RHIInstance.hpp"

#include "Engine/Core/Window.hpp"

#include "Engine/Renderer/RHI/RHI.hpp"

#include "Engine/Core/EngineConfig.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <dxgidebug.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

#define UNUSED(x) (void)(x);

static RHIInstance* gRenderInstance = nullptr;

RHIInstance::RHIInstance()
{
	debug = nullptr; // IDXGIDebug
	debug_module = nullptr; // HMODULE
	#if defined(_DEBUG)
		#if defined(_X64)
			debug_module = ::LoadLibrary("dxgidebug.dll");
		#else 
			debug_module = LoadLibraryExA("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		#endif

		typedef HRESULT(WINAPI *GetDebugModuleCB)(REFIID, void**);
		GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress(debug_module, "DXGIGetDebugInterface");
		cb(__uuidof(IDXGIDebug), (void**)&debug);

		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	#endif

	gRenderInstance = this;
}

RHIInstance::~RHIInstance()
{
	gRenderInstance = nullptr;

	// Debug Shutdown
	if (debug != nullptr) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);

		DX_SAFE_RELEASE(debug);
		if (nullptr != debug_module) {
			FreeLibrary(debug_module);
			debug_module = nullptr;
		}
	}
}

RHIInstance* RHIInstance::GetInstance()
{
	if (gRenderInstance == nullptr) {
		new RHIInstance();
	}

	return gRenderInstance;
}

bool RHIInstance::CreateOutput(RHIDevice** outDevice, RHIDeviceContext** outContext, RHIOutput** outOutput,
	unsigned int const px_width, unsigned int const px_height)
{
	Window* window = new Window();
	window->SetClientDimension(px_width, px_height);

	if (!window->Open())
	{
		delete window;
		return false;
	}
	window->Show();


	// Creation Flags
	// For options, see: https://msdn.microsoft.com/en-us/library/windows/desktop/ff476107(v=vs.85).aspx

	unsigned int device_flags = 0U;

	#if defined(RENDER_DEBUG)
		device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	// This flag fails unless we' do 11.1 (which we're not), and we query that
	// the adapter support its (which we're not).  Just here to let you know it exists.
	// device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
	#endif

	// Setup our Swap Chain
	// For options, see: https://msdn.microsoft.com/en-us/library/windows/desktop/bb173075(v=vs.85).aspx

	DXGI_SWAP_CHAIN_DESC scd;
	memset(&scd, 0, sizeof(scd));

	// fill the swap chain description struct
	scd.BufferCount = 2;                                    // two buffers (one front, one back?)

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;
	scd.OutputWindow = window->m_windowHandle;						// the window to be copied to on present
	scd.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)

															// Default options.
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferDesc.Width = px_width;
	scd.BufferDesc.Height = px_height;					

	ID3D11Device *dx_device;
	ID3D11DeviceContext *dx_context;
	IDXGISwapChain *dx_swapchain;
															// Actually Create
	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(nullptr,	// Adapter, if nullptr, will use adapter window is primarily on.
		D3D_DRIVER_TYPE_HARDWARE,							// Driver Type - We want to use the GPU (HARDWARE)
		nullptr,											// Software Module - DLL that implements software mode (we do not use)
		device_flags,										// device creation options
		nullptr,											// feature level (use default)
		0U,													// number of feature levels to attempt
		D3D11_SDK_VERSION,									// SDK Version to use
		&scd,												// Description of our swap chain
		&dx_swapchain,										// Swap Chain we're creating
		&dx_device,											// [out] The device created
		nullptr,											// [out] Feature Level Acquired
		&dx_context);										// Context that can issue commands on this pipe.

	if (SUCCEEDED(hr)) 
	{
		RHIDevice* device = new RHIDevice(this, dx_device);
		RHIDeviceContext* context = new RHIDeviceContext(device, dx_context);
		RHIOutput* output = new RHIOutput(device, dx_swapchain, window);

		device->immediateContext = context;

		*outDevice = device;
		*outContext = context;
		*outOutput = output;

		return true;
	}
	else {
		delete window;
		return false;
	}
}

void RHIInstance::ReportLiveObjects()
{
	if ((gRenderInstance != nullptr) && (gRenderInstance->debug != nullptr)) {
		gRenderInstance->debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	}
}

void RHIInstance::SetDebugName(const char* name, ID3D11DeviceChild* d3dObject)
{
	#if defined (RENDER_DEBUG)
		d3dObject->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name) + 1, name);
	#endif
}

/*
void RHIInstance::Render()
{
	// clear the back buffer to a deep blue
	pD3DContext->ClearRenderTargetView(pD3DBackBuffer, SCREEN_COLOR);
	
	// do 3D rendering on the back buffer here
	
	// switch the back buffer and the front buffer
	
	pD3DSwapChain->Present(0, 0);
}*/
