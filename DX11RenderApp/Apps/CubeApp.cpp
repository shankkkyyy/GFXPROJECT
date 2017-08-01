#include "CubeApp.h"

CubeApp::CubeApp(HINSTANCE _hInstance) :
	D3DApp(_hInstance),
	mPitch(MathHelper::Pi * 0.25f),
	mYaw(1.5f*MathHelper::Pi),
	mRadius(5.0f),
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
	DirectX::XMStoreFloat4x4(&mView, I);
	DirectX::XMStoreFloat4x4(&mProj, I);
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

	Vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4((const float*)(&DirectX::Colors::White)) },
		{ DirectX::XMFLOAT3(-1.0f, +1.0f, -1.0f), DirectX::XMFLOAT4((const float*)(&DirectX::Colors::Black)) },
		{ DirectX::XMFLOAT3(+1.0f, +1.0f, -1.0f), DirectX::XMFLOAT4((const float*)(&DirectX::Colors::Red)) },
		{ DirectX::XMFLOAT3(+1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4((const float*)(&DirectX::Colors::Green)) },

		{ DirectX::XMFLOAT3(-1.0f, -1.0f, +1.0f), DirectX::XMFLOAT4((const float*)(&DirectX::Colors::Blue)) },
		{ DirectX::XMFLOAT3(-1.0f, +1.0f, +1.0f), DirectX::XMFLOAT4((const float*)(&DirectX::Colors::Yellow)) },
		{ DirectX::XMFLOAT3(+1.0f, +1.0f, +1.0f), DirectX::XMFLOAT4((const float*)(&DirectX::Colors::Cyan)) },
		{ DirectX::XMFLOAT3(+1.0f, -1.0f, +1.0f), DirectX::XMFLOAT4((const float*)(&DirectX::Colors::Magenta)) }

	};

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = 8 * sizeof(Vertex);
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

	UINT indices[] =
	{
		// front, back, left ,right, up, down
		0, 1, 2,
		0, 2, 3,

		4, 6, 5,
		4, 7, 6,

		4, 5, 1,//0, 5, 1,
		4, 1, 0,//0, 4, 5,

		3, 2, 6,
		3, 6, 7,

		1, 5, 6,
		1, 6, 2,

		4, 0 ,3, //0, 7, 4,
		4, 3, 7//0, 3, 7
	};
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = 36 * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;


	D3D11_SUBRESOURCE_DATA iInitData;
	ZeroMemory(&iInitData, sizeof(iInitData));
	iInitData.pSysMem = indices;

	HR(md3dDevice->CreateBuffer(&ibd, &iInitData, &mCubeIB));

#pragma endregion
}

void CubeApp::BuildFX()
{
#pragma region Read The Complied Byte File


	if (!d3dHelper::LoadShaderByteCode("FXs/Color.fxo", md3dDevice, &mFX))
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
	D3DApp::OnResize();

	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(MathHelper::Pi * 0.25f, GetAspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&mProj, P);
}

void CubeApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(md3dRTV, DirectX::Colors::AntiqueWhite);
	md3dImmediateContext->ClearDepthStencilView(md3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	md3dImmediateContext->IASetInputLayout(mInputLayout);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetIndexBuffer(mCubeIB, DXGI_FORMAT_R32_UINT, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mCubeVB, &stride, &offset);

	// update the wvp on cbuffer

	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mCubeWorld);
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mView);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
	DirectX::XMMATRIX wvp = world * view * proj;

	mfxWorldViewProj->SetMatrix(reinterpret_cast<const float*>(&wvp));

	// draw through fx pass
	D3DX11_TECHNIQUE_DESC td;
	mTech->GetDesc(&td);

	for (UINT i = 0; i < td.Passes; i++)
	{
		mTech->GetPassByIndex(i)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(36, 0, 0);
	}
	HR(mSwapChain->Present(0, 0));
}

void CubeApp::UpdateScene()
{

	// update view matrix

	float sin_pitch = sinf(mPitch);


	float x = mRadius * sin_pitch * cosf(mYaw);
	float z = mRadius * sin_pitch * sinf(mYaw);
	float y = mRadius * cosf(mPitch);

	DirectX::XMVECTOR eye = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR tar = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX v = DirectX::XMMatrixLookAtLH(eye, tar, up);
	DirectX::XMStoreFloat4x4(&mView, v);

}

void CubeApp::OnMouseDown(WPARAM _btnState, int x, int y)
{
	mMouseLastPos.x = x;
	mMouseLastPos.y = y;
	SetCapture(mhMainWnd);
}

void CubeApp::OnMouseUp(WPARAM _btnState, int x, int y)
{
	ReleaseCapture();
}

void CubeApp::OnMouseMove(WPARAM _btnState, int x, int y)
{
	if (_btnState & MK_LBUTTON)
	{
		float dx = 0.25f * DirectX::XMConvertToRadians(static_cast<float>(x - mMouseLastPos.x));
		float dy = 0.25f * DirectX::XMConvertToRadians(static_cast<float>(y - mMouseLastPos.y));

		mYaw -= dx;
		mPitch += dy;
		mPitch = MathHelper::Clamp(mPitch, 0.1f, MathHelper::Pi - 0.1f);

	}
	else if (_btnState & MK_RBUTTON)
	{
		float dx = 0.25f * DirectX::XMConvertToRadians(static_cast<float>(x - mMouseLastPos.x));
		float dy = 0.25f * DirectX::XMConvertToRadians(static_cast<float>(y - mMouseLastPos.y));

		mRadius += dx - dy;
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mMouseLastPos.x = x;
	mMouseLastPos.y = y;
}
