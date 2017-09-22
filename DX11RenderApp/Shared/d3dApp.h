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


	// offscreen buffer
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    mOffscreenRTV         = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  mOffscreenSRV         = nullptr;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mOffscreenUAV         = nullptr;

	// for blur
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  mHorBluredSRV = nullptr;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> mHorBluredUAV = nullptr;



	
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   mRSFrontCull           = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   mRSNoCull              = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   mRSWireFrame = nullptr;
																	       
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDSLessEqual           = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDSStencilMark         = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDSReflection          = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDSNoDoubleBlend       = nullptr;
																	       
	Microsoft::WRL::ComPtr<ID3D11BlendState>        mBSTransparent         = nullptr;
	Microsoft::WRL::ComPtr<ID3D11BlendState>        mBSNoRenderTargetWrite = nullptr;
	Microsoft::WRL::ComPtr<ID3D11BlendState>        mBSAlphaToCoverage     = nullptr;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>      mSSANISOTROPICWRAP = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>      mSSANISOTROPICCLAMP = nullptr;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDynamicCubeMapDSV  = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mDynamicCubeMapRTV[6];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDynamicCubeMapSRV  = nullptr;

	D3D11_VIEWPORT          mScreenViewPort;
	D3D11_VIEWPORT          mCubeMapViewPort;

	
	std::wstring mMainWndCaption;
	D3D_DRIVER_TYPE md3dDriveType;
	int mClientWidth;
	int mClientHeight;
	bool bEnable4xMsaa;

public:


	ID3D11Device*            GetDevice()         const;
	ID3D11DeviceContext*     GetDeviceContext()  const;
	ID3D11BlendState*        GetBSTransparent() const;
	ID3D11DepthStencilState* GetDSLessEqual()   const;
	ID3D11RasterizerState*   GetRSFrontCull() const;
	ID3D11RasterizerState*   GetRSNoCull()    const;
	ID3D11RasterizerState*   GetRSWireFrame() const;

	ID3D11RenderTargetView*  GetRenderTarget()     const;
	ID3D11RenderTargetView*  GetOffScreenRTV() const;
	ID3D11DepthStencilView*  GetDepthStencilView() const;

	ID3D11UnorderedAccessView* GetOffScreenUAV() const;
	ID3D11ShaderResourceView*  GetOffScreenSRV() const;
	ID3D11UnorderedAccessView* GetHorBluredUAV() const;
	ID3D11ShaderResourceView* GetHorBluredSRV() const;

	const D3D11_VIEWPORT*     GetScreenViewPort() const;
	const D3D11_VIEWPORT*     GetCubeMapViewPort() const;

	ID3D11DepthStencilView*   GetDynamicCubeMapDSV() const;
	ID3D11ShaderResourceView* GetDynamicCubeMapSRV()const;
	ID3D11RenderTargetView*   GetDynamicCubeMapRTV(UINT _index)const;

	ID3D11SamplerState*  GetSSANISOTROPICWRAP() const;
	ID3D11SamplerState*  GetSSANISOTROPICCLAMP() const;


	int GetScreenHeight()   const;
	int GetScreenWidth()    const;

private:

	void CreateRasterizationStates();

	void CreateSamplerStates();

	void CreateDepthStencilStates();

	void CreateBlendStates();

	void CalculateAspectRatio();

	void FetchWndInputOnMouse(WPARAM _wParam, LPARAM _lParam);

};

