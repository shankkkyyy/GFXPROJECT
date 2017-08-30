#include "SkyBox.h"
#include "Object.h"



SkyBox::SkyBox()
{
}


SkyBox::~SkyBox()
{
}

void SkyBox::Edit(ID3D11VertexShader * const _VS, ID3D11PixelShader * const _PS, 
	ID3D11ShaderResourceView * const _tex, ID3D11InputLayout * const _IL,
	ID3D11Device* const _device)
{
	mMap = _tex;
	mVS = _VS;
	mPS = _PS;
	mIL = _IL;

	Mesh* skyGeometery = Objects::GetDefaultCubeMesh();

	Vertex12* vertices = new Vertex12[skyGeometery->vertices.size()];
	for (size_t i = 0; i < skyGeometery->vertices.size(); i++)
	{
		vertices[i].pos = skyGeometery->vertices[i].pos;
	}


#pragma region  Create VB and IB and push data to VRAM

	D3D11_BUFFER_DESC      vbd, ibd;
	D3D11_SUBRESOURCE_DATA initData;

	// VB
	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = (UINT)skyGeometery->vertices.size() * sizeof(Vertex12);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = vertices;

	HR(_device->CreateBuffer(&vbd, &initData, mVB.GetAddressOf()));


	// IB:
	ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = (UINT)skyGeometery->indices.size() * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = skyGeometery->indices.data();
	HR(_device->CreateBuffer(&ibd, &initData, mIB.GetAddressOf()));

#pragma endregion

	delete vertices;
}

void SkyBox::Draw(ID3D11DeviceContext * _ImmediateContext)
{

	// IA stage
	UINT stride, offset;
	offset = 0;
	stride = sizeof(Vertex12);
	_ImmediateContext->IASetVertexBuffers(0, 1, mVB.GetAddressOf(), &stride, &offset);
	_ImmediateContext->IASetIndexBuffer(mIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	_ImmediateContext->VSSetShader(mVS, 0, 0);
	_ImmediateContext->PSSetShader(mPS, 0, 0);
	_ImmediateContext->GSSetShader(nullptr, 0, 0);
	_ImmediateContext->IASetInputLayout(mIL);
	
	// texture
	_ImmediateContext->PSSetShaderResources(1, 1, &mMap);

	// draw
	_ImmediateContext->DrawIndexed(36, 0, 0);

}
