#include "d3dApp.h"

// for GET_X_LPARAM and GET_Y_LPARAM
#include <windowsx.h>
#include <sstream>



LRESULT CALLBACK MainWndProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	return D3DApp::GetApp()->MsgProc(_hwnd, _msg, _wParam, _lParam);
}

D3DApp* D3DApp::mApp = nullptr;

D3DApp * D3DApp::GetApp()
{
	return mApp;
}

HINSTANCE D3DApp::GetAppInstance() const
{
	return mhAppInst;
}

HWND D3DApp::GetMainWnd() const
{
	return mhMainWnd;
}

float D3DApp::GetAspectRatio() const
{
	return mAspectRatio;
}

float D3DApp::GetDeltaTime() const
{
	return mTimer.DeltaTime();
}

POINT D3DApp::GetLastMousePosition() const
{
	return mMouseLastPos;
}



D3DApp::D3DApp(HINSTANCE _hInstance) : 
	mhAppInst(_hInstance),
	mClientHeight(600),
	mClientWidth(800),
	mMainWndCaption(L"D3D11 App"),
	md3dDriveType(D3D_DRIVER_TYPE_HARDWARE),
	bMaximized(false),
	bMinimized(false),
	bResizing(false),
	bEnable4xMsaa(false),
	bAppPaused(false),
	md3dDevice(nullptr),
	md3dImmediateContext(nullptr),
	md3dDSV(nullptr),
	md3dRTV(nullptr),
	mDepthStencilBuffer(nullptr),
	mSwapChain(nullptr),
	m4xMsaaQuality(0)
{		
	ZeroMemory(&mScreenViewPort, sizeof(D3D11_VIEWPORT));
}

D3DApp::~D3DApp()
{
	if (md3dImmediateContext)
	{
		md3dImmediateContext->ClearState();
	}
	ReleaseCOM(md3dDevice);
	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(md3dRTV);
	ReleaseCOM(md3dDSV);
	ReleaseCOM(mDepthStencilBuffer);
}

int D3DApp::Run()
{
	MSG msg = { 0 };
	mTimer.Reset();
	while (msg.message != WM_QUIT)
	{
		// if there are windows message then process it
		if (PeekMessage(&msg, 0,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mTimer.Tick();
			if (!bAppPaused)
			{
				float deltaTime = mTimer.DeltaTime();
				CalculateFrameRate();
				UpdateScene(deltaTime);
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool D3DApp::InitApp()
{
	if (!InitMainWindow())
	{
		return false;
	}

	if (!InitDirect3D())
	{
		return false;
	}

	OnResize();

	return true;
}

LRESULT D3DApp::MsgProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_msg)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(_wParam) == WA_INACTIVE)
		{
			bAppPaused = true;
			mTimer.Pause();
		}
		else
		{
			bAppPaused = false;
			mTimer.Unpause();
		}
		return 0;
	}
	case WM_SIZE:
	{
		mClientWidth = LOWORD(_lParam);
		mClientHeight = HIWORD(_lParam);
		if (md3dDevice)
		{
			if (_wParam == SIZE_MAXIMIZED)
			{
				bMaximized = true;
				bMinimized = false;
				bAppPaused = false;
				OnResize();
			}
			else if (_wParam == SIZE_MINIMIZED)
			{
				bMaximized = false;
				bMinimized = true;
				bAppPaused = true;
			}
			else if (_wParam == SIZE_RESTORED)
			{
				if (bMaximized)
				{
					bMaximized = false;
					bAppPaused = false;
					OnResize();
				}
				else if (bMinimized)
				{
					bMinimized = false;
					bAppPaused = false;
					OnResize();
				}
				else if (bResizing)
				{

				}
				else
				{
					OnResize();
				}
			}
		}
		return 0;

	}
	case WM_ENTERSIZEMOVE:
	{
		bAppPaused = true;
		bResizing = true;
		mTimer.Pause();
		return 0;

	}
	case WM_EXITSIZEMOVE:
	{
		bAppPaused = false;
		bResizing = false;
		mTimer.Unpause();
		OnResize();
		return 0;

	}
	case WM_DESTROY:
	{		
		PostQuitMessage(0);
		return 0;

	}
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
	{
		((MINMAXINFO*)_lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)_lParam)->ptMinTrackSize.y = 200;
		return 0;

	}

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		FetchWndInputOnMouse(_wParam, _lParam);
		OnMouseDown(); 
		return 0;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{

		FetchWndInputOnMouse(_wParam, _lParam);
		OnMouseUp();
		return 0;

	}
	case WM_MOUSEMOVE:
	{
		FetchWndInputOnMouse(_wParam, _lParam);
		OnMouseMove();
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		mWndInput.wheelDistance = HIWORD(_wParam);
		OnMouseWheel();
		return 0;
	}
	case WM_KEYDOWN:
	{
		mWndInput.key = (WORD)_wParam;
		OnKeyDown();
		return 0;
	}

	case WM_KEYUP:
	{
		if (_wParam == VK_ESCAPE)
		{
			mSwapChain->SetFullscreenState(false, nullptr);
			PostQuitMessage(0);
		}
		return 0;
	}


	default:
		return DefWindowProc(_hwnd, _msg, _wParam, _lParam);
	}
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"D3DWndClassName", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

bool D3DApp::InitDirect3D()
{

#pragma region Create Device
	UINT createDeviceFlag = 0;
#if defined(DEBUG) | defined(_DEBUG)
	createDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif // defined(DEBUG) | defined((_DEBUG))

	D3D_FEATURE_LEVEL featureLevels[2] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};
	D3D_FEATURE_LEVEL featureLevel;


	HRESULT hr  = D3D11CreateDevice(
		nullptr, // using primary default display adapter
		md3dDriveType,
		0,
		createDeviceFlag,
		featureLevels,
		2,
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext
	);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}
#pragma endregion


#pragma region Check 4xMsaa Quality Support

	HR(md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

#pragma endregion


#pragma region Create Swap Chain

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	if (bEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.Flags = 0;
	sd.OutputWindow = mhMainWnd;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed = true;

	IDXGIDevice* dxgiDevice = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	IDXGIFactory* dxgiFactory = nullptr;

	HR(md3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice)));
	HR(dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter)));
	HR(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));
	
	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
	

#pragma endregion



	CreateRasterizationStates();

	CreateSamplerStates();

	CreateDepthStencilStates();

	CreateBlendStates();

	return true;
}

void D3DApp::CalculateFrameRate()
{
	static unsigned FrameCnt = 0;
	static float timeElapsed = 0;
	FrameCnt++;
	if (mTimer.TotalTime() - timeElapsed > 1.0f)
	{
		float fps = float(FrameCnt);
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    FPS: " << fps << L"    mspf: " << mspf;
		SetWindowText(mhMainWnd, outs.str().c_str());

		FrameCnt = 0;
		timeElapsed = mTimer.TotalTime();
	}
}

void D3DApp::OnResize()
{
	assert(md3dDevice);
	assert(md3dImmediateContext);
	assert(mSwapChain);


	ReleaseCOM(md3dRTV);
	ReleaseCOM(md3dDSV);
	ReleaseCOM(mDepthStencilBuffer);

	//ID3D11RenderTargetView* offscreenRTV = mOffscreenRTV.Get();
	//ID3D11ShaderResourceView* offscreenSRV = mOffscreenSRV.Get();

	mOffscreenRTV.Reset();
	mOffscreenSRV.Reset();

	//ReleaseCOM(offscreenRTV);
	//ReleaseCOM(offscreenSRV);


#pragma region Recreate Render Target View

	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer, nullptr, &md3dRTV));
	ReleaseCOM(backBuffer);

#pragma endregion

#pragma region Recreate Depth Stencil View and buffer

	D3D11_TEXTURE2D_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_TEXTURE2D_DESC));
	dsvd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsvd.MipLevels = 1; // only one mip level
	dsvd.ArraySize = 1; // mip level array size = 1
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bit for depth map to [0,1], 8 bit for stencil map to [0, 255]
	dsvd.Usage = D3D11_USAGE_DEFAULT; // only GPU has access to read and write
	dsvd.CPUAccessFlags = 0;
	dsvd.Height = mClientHeight;
	dsvd.Width = mClientWidth;
	dsvd.MiscFlags = 0; // has nothing to do with dsv

	if (bEnable4xMsaa)
	{
		dsvd.SampleDesc.Count = 4;
		dsvd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		dsvd.SampleDesc.Count = 1;
		dsvd.SampleDesc.Quality = 0;
	}


	HR(md3dDevice->CreateTexture2D(&dsvd, nullptr, &mDepthStencilBuffer));

	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, nullptr, &md3dDSV));

#pragma endregion


#pragma region Recreate Offscreen Render Target view
	
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mClientWidth;
	texDesc.Height = mClientHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (bEnable4xMsaa)
	{
		texDesc.SampleDesc.Count = 4;
		texDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
	}
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;


	ID3D11Texture2D* offscreenTex = nullptr;

	HR(md3dDevice->CreateTexture2D(&texDesc, nullptr, &offscreenTex));

	//D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	//ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	//rtvDesc.Format = texDesc.Format;
	//rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//rtvDesc.Texture2D.MipSlice = 0;

	HR(md3dDevice->CreateRenderTargetView(offscreenTex, 0, mOffscreenRTV.GetAddressOf()));

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//srvDesc.Format = texDesc.Format;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MostDetailedMip = 0;
	//srvDesc.Texture2D.MipLevels = 1;

	HR(md3dDevice->CreateShaderResourceView(offscreenTex, 0, mOffscreenSRV.GetAddressOf()));


	ReleaseCOM(offscreenTex);
#pragma endregion

	// bind view to out merge stage
	//md3dImmediateContext->OMSetRenderTargets(1, &md3dRTV, md3dDSV);


#pragma region Setup ViewPort	
	mScreenViewPort.TopLeftX = 0;
	mScreenViewPort.TopLeftY = 0;
	mScreenViewPort.MinDepth = 0;
	mScreenViewPort.MaxDepth = 1;
	mScreenViewPort.Width = static_cast<float>(mClientWidth);
	mScreenViewPort.Height = static_cast<float>(mClientHeight);	
	md3dImmediateContext->RSSetViewports(1, &mScreenViewPort);
#pragma endregion

	CalculateAspectRatio();

}



void D3DApp::OnMouseDown()
{
	mWndInput.cursorPos_pre = mWndInput.cursorPos_curr;
	SetCapture(mhMainWnd);
}

void D3DApp::OnMouseUp()
{
	ReleaseCapture();
}

void D3DApp::OnMouseMove()
{
	mWndInput.cursorPos_pre = mWndInput.cursorPos_curr;
}

void D3DApp::OnMouseWheel()
{

}

void D3DApp::OnKeyDown()
{

}

const WndInput* const D3DApp::GetWndInput() const
{
	return &mWndInput;
}

ID3D11Device * D3DApp::GetDevice() const
{
	return md3dDevice;
}

ID3D11DeviceContext * D3DApp::GetDeviceContext() const
{
	return md3dImmediateContext;
}

ID3D11RasterizerState * D3DApp::GetRSFrontCull() const
{
	return mRSFrontCull.Get();
}

ID3D11RasterizerState * D3DApp::GetRSNoCull() const
{
	return mRSNoCull.Get();
}

void D3DApp::CreateRasterizationStates()
{
	// Default
	D3D11_RASTERIZER_DESC rd;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = false;
	rd.DepthBias = 0;
	rd.SlopeScaledDepthBias = 0.0f;
	rd.DepthBiasClamp = 0.0f;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = false;

	HR(md3dDevice->CreateRasterizerState(&rd, mRSNoCull.GetAddressOf()));

	rd.CullMode = D3D11_CULL_FRONT;
	HR(md3dDevice->CreateRasterizerState(&rd, mRSFrontCull.GetAddressOf()));
}

void D3DApp::CreateSamplerStates()
{


	mSSs.resize(2);

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MaxAnisotropy = 4;
	sd.MinLOD = -FLT_MAX;
	sd.MaxLOD = FLT_MAX;

	HR(md3dDevice->CreateSamplerState(&sd, mSSs[0].GetAddressOf()));


	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	HR(md3dDevice->CreateSamplerState(&sd, mSSs[1].GetAddressOf()));

}

void D3DApp::CreateDepthStencilStates()
{
	D3D11_DEPTH_STENCIL_DESC dsd;

	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsd.StencilEnable = false;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(md3dDevice->CreateDepthStencilState(&dsd, mDSLessEqual.GetAddressOf()));


#pragma region Reflection
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc      = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = true;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	dsd.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	HR(md3dDevice->CreateDepthStencilState(&dsd, mDSReflection.GetAddressOf()));
#pragma endregion

#pragma region 	Marker
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = true;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	HR(md3dDevice->CreateDepthStencilState(&dsd, mDSStencilMark.GetAddressOf()));


#pragma endregion

#pragma region no Double Blending
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	dsd.StencilEnable = true;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsd.FrontFace.StencilFunc        = D3D11_COMPARISON_EQUAL;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_INCR;

	dsd.BackFace.StencilFunc        = D3D11_COMPARISON_EQUAL;
	dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
	dsd.BackFace.StencilPassOp      = D3D11_STENCIL_OP_INCR;

	HR(md3dDevice->CreateDepthStencilState(&dsd, mDSNoDoubleBlend.GetAddressOf()));
#pragma endregion
}

void D3DApp::CreateBlendStates()
{
	D3D11_BLEND_DESC bsd;
	ZeroMemory(&bsd, sizeof(D3D11_BLEND_DESC));

	// mBSTransparent
	bsd.AlphaToCoverageEnable = false;
	bsd.IndependentBlendEnable = false;
	bsd.RenderTarget[0].BlendEnable = true;
	bsd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bsd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bsd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bsd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bsd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bsd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bsd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(md3dDevice->CreateBlendState(&bsd, mBSTransparent.GetAddressOf()));

	// mBSNoRenderTargetWrite
	bsd.RenderTarget[0].BlendEnable = false;
	bsd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bsd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	bsd.RenderTarget[0].RenderTargetWriteMask = 0;

	HR(md3dDevice->CreateBlendState(&bsd, mBSNoRenderTargetWrite.GetAddressOf()));

	ZeroMemory(&bsd, sizeof(D3D11_BLEND_DESC));
	bsd.AlphaToCoverageEnable = true;
	bsd.IndependentBlendEnable = false;
	bsd.RenderTarget[0].BlendEnable = false;
	bsd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(md3dDevice->CreateBlendState(&bsd, mBSAlphaToCoverage.GetAddressOf()));

}

void D3DApp::CalculateAspectRatio()
{
	mAspectRatio = static_cast<float>(mClientWidth) / mClientHeight;
}

void D3DApp::FetchWndInputOnMouse(WPARAM _wParam, LPARAM _lParam)
{
	mWndInput.mButton = (WORD)_wParam;
	mWndInput.cursorPos_curr.x = GET_X_LPARAM(_lParam);
	mWndInput.cursorPos_curr.y = GET_Y_LPARAM(_lParam);
	mWndInput.deltaXY.x = mWndInput.cursorPos_curr.x - mWndInput.cursorPos_pre.x;
	mWndInput.deltaXY.y = mWndInput.cursorPos_curr.y - mWndInput.cursorPos_pre.y;
}


