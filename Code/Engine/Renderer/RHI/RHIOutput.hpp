#pragma once

#include "Engine/Renderer/RHI/DX11.hpp"
#include "Engine/Renderer/RHI/RHITypes.hpp"
#include "Engine/Core/Window.hpp"

class RHIInstance;
class RHIDevice;
class RHIDeviceContext;
class RHITexture2D;

class RHIOutput
{
public:
	RHIOutput(RHIDevice *o, IDXGISwapChain *sc, Window *wnd);
	//RHIOutput(RHIDevice *owner, Window *wnd, /*[OPTIONAL]*/ eRHIOutputMode const mode = eRHIOutputMode::RENDEROUTPUT_WINDOWED);
	~RHIOutput();

	// Whatever is currently being renderered
	// that is what we're working with.
	void Present();
	void Close();

	RHITexture2D* GetRenderTarget();
	inline bool IsValid() const { return swapChain != nullptr; }
	inline float GetWidth() const { return outputWidth; }
	inline float GetHeight() const { return outputHeight; }
	float GetAspectRatio() const;

	// [OPTIONAL]
	//bool SetDisplayMode(eRHIOutputMode const mode, float const widthParam = 0.0f, float const heightParam = 0.0f);

public:
	float outputWidth;
	float outputHeight;

	RHIDevice* device;
	Window* window;

	IDXGISwapChain* swapChain;

	RHITexture2D* renderTarget;

private:
	eRHIOutputMode m_mode;

	bool CreateRenderTarget();
	void DestroyRenderTarget();
	void DestroySwapChain();
};