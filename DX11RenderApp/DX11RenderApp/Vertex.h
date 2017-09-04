#pragma once
#include "d3dUtil.h"




struct Vertex12
{
	DirectX::XMFLOAT3 pos;
};


struct Vertex24
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 nor;
};

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 nor;
	DirectX::XMFLOAT2 uv;

	Vertex& operator= (const Vertex& _other);
};

struct VertexBB
{
	DirectX::XMFLOAT3 root;
	DirectX::XMFLOAT2 size;
};


class InputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC IDPosSize[2];
	static const D3D11_INPUT_ELEMENT_DESC IDPos[1];
	static const D3D11_INPUT_ELEMENT_DESC IDPosNor[2];
	static const D3D11_INPUT_ELEMENT_DESC IDBasic32[3];
	static const D3D11_INPUT_ELEMENT_DESC IDBasic32Inst[7];
};


