#pragma once
#include "D3DApp.h"

class DX11InitApp : public D3DApp
{

public:
	DX11InitApp(HINSTANCE _hInstance);
	~DX11InitApp();
	bool InitApp() override;



private:
	
	void UpdateScene() override;
	void DrawScene() override;
};

