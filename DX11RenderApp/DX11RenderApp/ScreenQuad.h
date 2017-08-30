#pragma once

#include "d3dUtil.h"

class ScreenQuad
{
public:
	ScreenQuad();
	~ScreenQuad();

private:


	Microsoft::WRL::ComPtr<ID3D11Buffer> mVB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIB;

public:
	void Draw();

};

