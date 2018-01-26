#include "Engine/Renderer/RHI/DX11.hpp"

 ID3D11Device* g_D3DDevice = nullptr;
 ID3D11DeviceContext* g_D3DContext = nullptr;
 IDXGISwapChain* g_D3DSwapChain = nullptr;
 ID3D11RenderTargetView* g_D3DBackBuffer = nullptr;
