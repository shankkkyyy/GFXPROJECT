#pragma once


#include "d3dUtil.h"
#include "GameTimer.h"


struct WndInput
{
	WORD mButton;
	WORD key;
	SHORT wheelDistance;
	POINT deltaXY;
	POINT cursorPos_pre;
	POINT cursorPos_curr;
};







class D3DApp
{

protected:
	D3DApp(HINSTANCE _hInstance);
	virtual ~D3DApp();
	D3DApp(const D3DApp& _other) = delete;
	D3DApp& operator= (const D3DApp& _other) = delete;

public:
	
	static D3DApp*        GetApp();
	HINSTANCE             GetAppInstance() const; 
	HWND                  GetMainWnd() const;
	float                 GetAspectRatio() const;
	float                 GetDeltaTime() const;
	POINT                 GetLastMousePosition() const;
	const WndInput* const GetWndInput() const;


	int Run();

	virtual bool InitApp();
	virtual LRESULT MsgProc(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);


protected:

	bool  InitMainWindow();
	bool  InitDirect3D();
	void  CalculateFrameRate();



	
	// Resize swapChain, recreate RTV and SDV
	virtual void OnResize();
	virtual void UpdateScene(float _deltaTime) = 0;
	virtual void DrawScene() = 0;


	virtual void OnMouseDown();

	virtual void OnMouseUp();

	virtual void OnMouseMove();

	virtual void OnMouseWheel();

	virtual void OnKeyDown();




protected:

	static D3DApp* mApp;
	HINSTANCE      mhAppInst;
	HWND           mhMainWnd;
	bool           bAppPaused;
	bool           bMinimized;
	bool           bMaximized;
	bool           bResizing;	
	UINT           m4xMsaaQuality;
	float          mAspectRatio;
	WndInput       mWndInput;


	GameTimer mTimer;

	POINT     mMouseLastPos;

	
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

private:

	void CalculateAspectRatio();

	void FetchWndInputOnMouse(WPARAM _wParam, LPARAM _lParam);

};

