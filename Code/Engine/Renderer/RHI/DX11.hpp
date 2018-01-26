#pragma once

#define INITGUID
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <d3d10.h>
#include <DXGI.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3d10.lib" )
#pragma comment( lib, "DXGI.lib" )

//For Shader Compilation
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )

// DEBUG STUFF
#include <dxgidebug.h>
#pragma comment( lib, "dxguid.lib" )

extern ID3D11Device* g_D3DDevice;
extern ID3D11DeviceContext* g_D3DContext;
extern IDXGISwapChain* g_D3DSwapChain;
extern ID3D11RenderTargetView* g_D3DBackBuffer;
