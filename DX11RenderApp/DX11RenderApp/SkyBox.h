#pragma once
#include "d3dUtil.h"


class SkyBox
{
public:
	SkyBox();
	~SkyBox();



private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVB = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIB = nullptr;

	ID3D11VertexShader* mVS = nullptr;
	ID3D11PixelShader*  mPS = nullptr;
	ID3D11ShaderResourceView* mMap = nullptr;
	ID3D11InputLayout* mIL = nullptr;

public:

	void Edit(ID3D11VertexShader* const _VS, ID3D11PixelShader* const _PS,
		ID3D11ShaderResourceView* const _tex, ID3D11InputLayout* const _IL,
		ID3D11Device* const _device);

	void Draw(ID3D11DeviceContext* _ImmediateContext);

	
};

