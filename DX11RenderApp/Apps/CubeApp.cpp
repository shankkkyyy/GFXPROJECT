#include "CubeApp.h"
#include "Object.h"


CubeApp::CubeApp(HINSTANCE _hInstance) :
	SceneRender(_hInstance),
	mCubeIB(nullptr),
	mCubeVB(nullptr),
	mInputLayout(nullptr),
	mFX(nullptr),
	mfxWorldViewProj(nullptr),
	mTech(nullptr)
{
	mClientWidth = 1280;
	mClientHeight = 960;
	mMainWndCaption = L"Cube Demo";

	DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();

	DirectX::XMStoreFloat4x4(&mCubeWorld, I);

}

CubeApp::~CubeApp()
{
	ReleaseCOM(mCubeIB);
	ReleaseCOM(mCubeVB);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mFX);
}

bool CubeApp::InitApp()
{

	if (D3DApp::InitApp())
	{
		BuildGeometryBuffer();
		BuildFX();
		BuildVertexLayout();

		return true;
	}
	else
		return false;
}

void CubeApp::BuildGeometryBuffer()
{
#pragma region Create Vertex Buffer


	Mesh Mp7;
	Object::LoadObjFile("../Models/M16/m16Maya.obj", Mp7);


	UINT verticesSize = Mp7.vertices.size();
	Vertex* vertices = new Vertex[verticesSize];

	for (UINT iv = 0; iv < Mp7.vertices.size(); iv++)
	{
		vertices[iv].pos = Mp7.vertices[iv].pos;
		vertices[iv].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
	

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbd.ByteWidth = 8 * sizeof(Vertex);
	vbd.ByteWidth = verticesSize * sizeof(Vertex);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	ZeroMemory(&vInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	vInitData.pSysMem = vertices;

	HR(md3dDevice->CreateBuffer(&vbd, &vInitData, &mCubeVB));

#pragma endregion

#pragma region Create Indices Buffer

	indicesAmount = Mp7.indices.size();
	UINT* indices = new UINT[indicesAmount];

	for (UINT ii = 0; ii < indicesAmount; ii++)
	{
		indices[ii] = Mp7.indices[ii];
	}

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = indicesAmount * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;


	D3D11_SUBRESOURCE_DATA iInitData;
	ZeroMemory(&iInitData, sizeof(iInitData));
	iInitData.pSysMem = indices;

	HR(md3dDevice->CreateBuffer(&ibd, &iInitData, &mCubeIB));

	delete[] vertices;
	delete[] indices;

#pragma endregion
}

void CubeApp::BuildFX()
{
#pragma region Read The Complied Byte File

	std::ifstream fin;
	fin.open("FXs/Color.fxo", std::ios::binary);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios_base::end);
		int size = (int)fin.tellg();
		fin.seekg(0, std::ios_base::beg);
		std::vector<char> compiledShader(size);
		fin.read(compiledShader.data(), size);
		fin.close();

		HR(D3DX11CreateEffectFromMemory(compiledShader.data(), size, 0, md3dDevice, &mFX));
	}
	else
	{
		MessageBox(0, L"Can not open shader file", 0, 0);
	}


#pragma endregion


#pragma region grab tech pointer and the to - be - updated variables within cbuffer from .fx 

	mTech = mFX->GetTechniqueByName("colorTech");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
#pragma endregion

}

void CubeApp::BuildVertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC vd[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};


	D3DX11_PASS_DESC pd;
	mTech->GetPassByIndex(0)->GetDesc(&pd);
	md3dDevice->CreateInputLayout(vd, 2, pd.pIAInputSignature, pd.IAInputSignatureSize, &mInputLayout);
}

void CubeApp::OnResize()
{
	SceneRender::OnResize();

}

void CubeApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(md3dRTV, DirectX::Colors::Black);
	md3dImmediateContext->ClearDepthStencilView(md3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetIndexBuffer(mCubeIB, DXGI_FORMAT_R32_UINT, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mCubeVB, &stride, &offset);

	// update the wvp on cbuffer

	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mCubeWorld);
	DirectX::XMMATRIX wvp = world * GetMainCamera()->GetViewProj();

	mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&wvp));

	// draw through fx pass
	D3DX11_TECHNIQUE_DESC td;
	mTech->GetDesc(&td);

	for (UINT i = 0; i < td.Passes; i++)
	{
		mTech->GetPassByIndex(i)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(indicesAmount, 0, 0);
		//md3dImmediateContext->DrawIndexed(36, 0, 0);

	}
	HR(mSwapChain->Present(0, 0));
}

void CubeApp::UpdateScene(float _deltaTime)
{

	SceneRender::UpdateScene(_deltaTime);

}



