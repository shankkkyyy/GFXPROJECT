#include "Vertex.h"
#include "Effect.h"


const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::IDBasic32[3] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOOD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

ID3D11InputLayout* InputLayouts::ILBasic32 = nullptr;

void InputLayouts::InitAll(ID3D11Device * _d3dDevice)
{
	
	D3DX11_PASS_DESC pd;
	Effects::BaseFx->mLightTech->GetPassByIndex(0)->GetDesc(&pd);
	HR(_d3dDevice->CreateInputLayout(InputLayoutDesc::IDBasic32, _countof(InputLayoutDesc::IDBasic32), pd.pIAInputSignature, pd.IAInputSignatureSize, &ILBasic32));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(ILBasic32);
}
