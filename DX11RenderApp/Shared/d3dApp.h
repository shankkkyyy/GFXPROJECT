#pragma once


#include "d3dUtil.h"
#include "GameTimer.h"


class D3DApp
{

protected:
	D3DApp(HINSTANCE _hInstance);
	virtual ~D3DApp();
	D3DApp(const D3DApp& _other) = delete;
	D3DApp& operator= (const D3DApp& _other) = delete;

public:
	
	static D3DApp*    GetApp();
	HINSTANCE         GetAppInstance() const; 
	HWND              GetMainWnd() const;
	float             GetAspectRatio() const;


	int Run();

	virtual bool InitApp();
	virtual LRESULT MsgProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);


protected:

	bool InitMainWindow();
	bool InitDirect3D();
	void CalculateFrameRate();

	
	// Resize swapChain, recreate RTV and SDV
	virtual void OnResize();
	virtual void UpdateScene() = 0;
	virtual void DrawScene() = 0;
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}



protected:

	static D3DApp* mApp;
	HINSTANCE mhAppInst;
	HWND mhMainWnd;
	bool bAppPaused;
	bool bMinimized;
	bool bMaximized;
	bool bResizing;	
	UINT m4xMsaaQuality;

	GameTimer mTimer;
	
	ID3D11Device*           md3dDevice;
	ID3D11DeviceContext*    md3dImmediateContext;
	IDXGISwapChain*         mSwapChain;
	ID3D11Texture2D*        mDepthStencilBuffer; // only GPU has access to read and write
	ID3D11RenderTargetView* md3dRTV;
	ID3D11DepthStencilView* md3dDSV;
	D3D11_VIEWPORT          mScreenViewPort;

	
	std::wstring mMainWndCaption;
	D3D_DRIVER_TYPE md3dDriveType;
	int mClientWidth;
	int mClientHeight;
	bool bEnable4xMsaa;


};

