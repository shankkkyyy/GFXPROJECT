#include "CubeApp.h"
#include "Vertex.h"



CubeApp::CubeApp(HINSTANCE _hInstance) :
	SceneRender(_hInstance),
	mCubeIB(nullptr),
	mCubeVB(nullptr),
	mTexMap(nullptr)
{
	mClientWidth = 1280;
	mClientHeight = 960;
	mMainWndCaption = L"Cube Demo";
}

CubeApp::~CubeApp()
{
	ReleaseCOM(mCubeIB);
	ReleaseCOM(mCubeVB);
	ReleaseCOM(mTexMap);
	Effects::DestroyAll();
	InputLayouts::DestroyAll();

	delete Car;
	delete Plane;
}

bool CubeApp::InitApp()
{

	if (D3DApp::InitApp())
	{
		Objects::LoadAssets(md3dDevice);


		//ID3D11Resource* texResource = nullptr;
		//HR(CreateWICTextureFromFile(md3dDevice, L"../Models/M16/m16.bmp", &texResource, &mTexMap));
		//ReleaseCOM(texResource);
		//HR(CreateDDSTextureFromFile(md3dDevice, L"../Models/uvmap.DDS", nullptr, &mTexMap));

		mDirLight.ambient = { 0.3f, 0.3f, 0.3f, 1.0f };
		mDirLight.diffuse = { 0.3f, 0.0f, 0.0f, 1.0f };
		mDirLight.specular = { 0.1f, 0.1f, 0.1f, 1.0f };
		mDirLight.direction = { -1.0f, -1.0f, -1.0f };


		mPointLight.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
		mPointLight.diffuse = { 0.0f, 10.0f, 0.0f, 1.0f };
		mPointLight.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
		mPointLight.range = 50.0f;
		mPointLight.position = { 0.0f ,5.0f, 0.0f };
		mPointLight.attenuation = { 1, 1, 1 };

		mSpotLight.ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
		mSpotLight.diffuse = { 1.0f, 1.0f, 0.0f, 1.0f };
		mSpotLight.specular = { 1, 1, 1, 1.0f };
		mSpotLight.range = 10.0f;
		mSpotLight.position = { 0.0f , 5.0f, -5.0f };
		mSpotLight.attenuation = { 1, 1, 1 };

		XMVECTOR dir = XMVectorSet(1, -0.5f, 1, 0);
		dir = XMVector3Normalize(dir);
		XMStoreFloat3(&mSpotLight.direction, dir);		
		mSpotLight.spot = 8;


		mMaterial.ambient  = { 1.0f, 1.0f, 1.0f, 1.0f };
		mMaterial.diffuse  = { 1.0f, 1.0f, 1.0f, 1.0f };
		mMaterial.specular = { 0.1f, 0.1f, 0.1f, 16.0f };



		Car = new Object();
		Car->Edit(Objects::GetCarMesh(), &mMaterial, Objects::GetCarTexture());


		Plane = new Object();
		Plane->Edit(Objects::GetDefaultCubeMesh(), Objects::GetDefMaterial(), nullptr);


		D3D11_BUFFER_DESC vbd;
		D3D11_BUFFER_DESC ibd;
		D3D11_SUBRESOURCE_DATA initData;

		Car->BuildVIBuffer(md3dDevice, vbd, ibd, initData);
		Plane->BuildVIBuffer(md3dDevice, vbd, ibd, initData);


		Car->SetPosition(XMFLOAT3(0, 0, 25));
		XMStoreFloat4x4(&Plane->mWorld, XMMatrixScaling(100, 1, 100)); 

		Effects::InitAll(md3dDevice);
		InputLayouts::InitAll(md3dDevice);

		return true;
	}
	else
		return false;
}

void CubeApp::BuildGeometryBuffer()
{
#pragma region Create Vertex Buffer


	Mesh* Mp7 = Objects:: GetCarMesh();
	//Mesh* Mp7 = Objects::GetTestCubeMesh();

	//Object::LoadObjFile("../Models/M16/m16Maya.obj", Mp7);	
	//Object::LoadObjFile("../Models/car.obj", Mp7);	
	//Object::LoadObjFile("../Models/cube.obj", Mp7);

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = Mp7->vertices.size() * sizeof(Vertex);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vInitData;
	ZeroMemory(&vInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	vInitData.pSysMem = Mp7->vertices.data();

	HR(md3dDevice->CreateBuffer(&vbd, &vInitData, &mCubeVB));

#pragma endregion

#pragma region Create Indices Buffer

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = Mp7->indices.size() * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iInitData;
	ZeroMemory(&iInitData, sizeof(iInitData));
	iInitData.pSysMem = Mp7->indices.data();
	indicesAmount = Mp7->indices.size();

	HR(md3dDevice->CreateBuffer(&ibd, &iInitData, &mCubeIB));
#pragma endregion
}

void CubeApp::OnResize()
{
	SceneRender::OnResize();

}

void CubeApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(md3dRTV, DirectX::Colors::Black);
	md3dImmediateContext->ClearDepthStencilView(md3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	md3dImmediateContext->IASetInputLayout(InputLayouts::ILBasic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// per frame
	Effects::BaseFx->SetDirectionLight(mDirLight);
	Effects::BaseFx->SetPointLight(mPointLight);
	Effects::BaseFx->SetSpotLight(mSpotLight);
	Effects::BaseFx->SetEyePosition(GetMainCamera()->GetPosition());
	// per object



	// draw through fx pass
	D3DX11_TECHNIQUE_DESC td;
	Effects::BaseFx->mLightTech->GetDesc(&td);

	for (UINT i = 0; i < td.Passes; i++)
	{

		Car->Draw(md3dImmediateContext,  stride, offset, GetMainCamera()->GetViewProj(), i);
		Plane->Draw(md3dImmediateContext,  stride, offset, GetMainCamera()->GetViewProj(), i);

	}
	HR(mSwapChain->Present(0, 0));
}

void CubeApp::UpdateScene(float _deltaTime)
{
	SceneRender::UpdateScene(_deltaTime);
}



