#include "Vertex.h"


const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::IDPos[1] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::IDPosNor[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::IDBasic32[3] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

Vertex & Vertex::operator=(const Vertex & _other)
{
	// TODO: insert return statement here
	pos = _other.pos;
	nor = _other.nor;
	uv = _other.uv;
	return *this;
}
