#include "DX11InitApp.h"




DX11InitApp::DX11InitApp(HINSTANCE _hInstance) : D3DApp(_hInstance)
{
	
}

DX11InitApp::~DX11InitApp()
{
}

bool DX11InitApp::InitApp()
{
	if (D3DApp::InitApp())
	{
		return true;
	}
	else
	{
		return false;
	}
}


void DX11InitApp::UpdateScene()
{
}

void DX11InitApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	md3dImmediateContext->ClearRenderTargetView(md3dRTV, DirectX::Colors::DarkCyan);
	md3dImmediateContext->ClearDepthStencilView(md3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	HR(mSwapChain->Present(0, 0));
}
