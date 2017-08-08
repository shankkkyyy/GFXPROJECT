#pragma once


#include "d3dUtil.h"


struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 nor;
	DirectX::XMFLOAT2 uv;
};


class InputLayoutDesc
{
public:

	static const D3D11_INPUT_ELEMENT_DESC IDBasic32[3];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* _d3dDevice);
	static void DestroyAll();

	static ID3D11InputLayout* ILBasic32;
};