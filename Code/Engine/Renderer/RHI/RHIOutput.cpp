#include "Engine/Renderer/RHI/RHIOutput.hpp"

#include "Engine/Renderer/RHI/RHIInstance.hpp"
#include "Engine/Renderer/RHI/RHIDevice.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"

RHIOutput::RHIOutput(RHIDevice *owner, IDXGISwapChain *sc, Window *wnd) :
	swapChain(sc),
	window(wnd)
{
	device = owner;
	CreateRenderTarget();
}

RHIOutput::~RHIOutput()
{
	Close();
}

void RHIOutput::Present()
{
	swapChain->Present(1, 0);
}

void RHIOutput::Close()
{
	DestroyRenderTarget();
	DestroySwapChain();

	if (window != nullptr)
	{
		window->Close();
		delete window;
		window = nullptr;
	}
}

RHITexture2D* RHIOutput::GetRenderTarget()
{
	return renderTarget;
}

float RHIOutput::GetAspectRatio() const
{
	return outputWidth / outputHeight;
}

bool RHIOutput::CreateRenderTarget()
{
	renderTarget = new RHITexture2D(device, this);
	return renderTarget->IsValid();
}

void RHIOutput::DestroyRenderTarget()
{
	if (nullptr != renderTarget) {
		delete renderTarget;
		renderTarget = nullptr;
	}
}

void RHIOutput::DestroySwapChain()
{
	DX_SAFE_RELEASE(swapChain);
}
