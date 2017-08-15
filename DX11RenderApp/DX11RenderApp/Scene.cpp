#include "Scene.h"


Scene::Scene(HINSTANCE _hInstance) : 
	SceneRender(_hInstance), 
	mVB(nullptr),
	mIB(nullptr),
	mCBVSPerFrame(nullptr),
	mCBVSPerObj(nullptr),
	mCBPSPerFrame(nullptr),
	mCBPSPerObj(nullptr),
	mVS(nullptr),
	mPS(nullptr),
	mIL(nullptr),
	mSS(nullptr),
	mIndexToDraw(0)
{
	mClientWidth = 1280;
	mClientHeight = 960;
	mMainWndCaption = L"Scene_0";		
}

Scene::~Scene()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mCBVSPerFrame);
	ReleaseCOM(mCBVSPerObj);
	ReleaseCOM(mCBPSPerFrame);
	ReleaseCOM(mCBPSPerObj);
	ReleaseCOM(mPS);
	ReleaseCOM(mVS);
	ReleaseCOM(mIL);
	ReleaseCOM(mSS);
}

bool Scene::InitApp()
{
	if (SceneRender::InitApp())
	{
		// Load Models
		Objects::LoadAssets(md3dDevice);


		BuildGeometryBuffer();

		CreateConstantBuffer();

		// Load Shaders and Create Input Layout
		CreateShaders();

		CreateSampleState();

		BuildSkyBoxGeometry();

		LoadSkyBoxShaderAndCreateInputLayout();




#pragma region Objs Init
		// init texture transform
		ZeroMemory(&mVSPerObj, sizeof(mVSPerObj));

		XMStoreFloat4x4(&mVSPerObj.texTransform, XMMatrixIdentity()); //(1, 1)
		mVSPerObj.texTransform(0, 0) = 5;
		mVSPerObj.texTransform(1, 1) = 5;



		// init lighting
		ZeroMemory(&mPSPerFrame, sizeof(mPSPerFrame));
		mPSPerFrame.eyePosition = GetMainCamera()->GetPosition();
		mPSPerFrame.ambientLight = { 0.3f, 0.3f, 0.3f, 1.0f };

		mPSPerFrame.light.lightColor = { .8f, .8f, .8f };
		mPSPerFrame.light.direction = { 1, -1, 0 };
		XMStoreFloat3(&mPSPerFrame.light.direction,
			XMVector3Normalize(MathHelper::XMFloat3ToXMVector(mPSPerFrame.light.direction)));

		mPSPerFrame.light.position = { -5.0f, 5.0f, 0 };
		mPSPerFrame.light.fallOffStart = 0;
		mPSPerFrame.light.fallOffend = 20;
		mPSPerFrame.light.spotAngle = 0.7f;

		// init Material
		ZeroMemory(&mPSPerObj, sizeof(mPSPerObj));
		mPSPerObj.material.diffuseAlbedo = { 0.5f,  0.5f,  0.5f, 1.0f };
		mPSPerObj.material.fresnelR0 = { 0.95f, 0.93f,0.88f };//sliver
		mPSPerObj.material.shininess = 10.0f;

		GetMainCamera()->SetPosition(XMFLOAT3(0, 10, -20));
		//GetMainCamera()->LookRight(-1.57f);
#pragma endregion



		return true;
	}
	else
	{
		return false;
	}
}

void Scene::BuildSkyBoxGeometry()
{
	Mesh* mesh = Objects::GetDefaultCubeMesh();
	
	UINT vertexAmount = (UINT)mesh->vertices.size();

	Vertex12* cube = new Vertex12[vertexAmount];

	for (UINT i = 0; i < vertexAmount; i++)
	{
		cube[i].pos = mesh->vertices[i].pos;
	}

	D3D11_BUFFER_DESC sky_vbd;
	sky_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	sky_vbd.ByteWidth = vertexAmount * sizeof(Vertex12);
	sky_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	sky_vbd.CPUAccessFlags = 0;
	sky_vbd.MiscFlags = 0;
	sky_vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA sky_initData;
	ZeroMemory(&sky_initData, sizeof(sky_initData));
	sky_initData.pSysMem = cube;

	HR(md3dDevice->CreateBuffer(&sky_vbd, &sky_initData, skyVB.GetAddressOf()));


	//
	// Indices
	skyIndexSize = (UINT)mesh->indices.size();
	D3D11_BUFFER_DESC sky_ibd;
	sky_ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	sky_ibd.ByteWidth = skyIndexSize * sizeof(UINT);
	sky_ibd.Usage = D3D11_USAGE_IMMUTABLE;
	sky_ibd.CPUAccessFlags = 0;
	sky_ibd.MiscFlags = 0;
	sky_ibd.StructureByteStride = 0;

	ZeroMemory(&sky_initData, sizeof(sky_initData));
	sky_initData.pSysMem = mesh->indices.data();

	HR(md3dDevice->CreateBuffer(&sky_ibd, &sky_initData, skyIB.GetAddressOf()));



	delete cube;
}

void Scene::LoadSkyBoxShaderAndCreateInputLayout()
{
	std::vector<char> shaderByteCode;
	d3dHelper::LoadShaderByteCode(L"Shaders/SkyBoxVS.cso", shaderByteCode);
	D3D11_INPUT_ELEMENT_DESC id[1] =
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	HR(md3dDevice->CreateInputLayout(id, _countof(id), shaderByteCode.data(), shaderByteCode.size(), &skyIL));
	HR(md3dDevice->CreateVertexShader(shaderByteCode.data(), shaderByteCode.size(), nullptr, &skyVS));


	d3dHelper::LoadShaderByteCode(L"Shaders/SkyBoxPS.cso", shaderByteCode);
	HR(md3dDevice->CreatePixelShader(shaderByteCode.data(), shaderByteCode.size(), nullptr, &skyPS));
}

void Scene::BuildGeometryBuffer()
{
	mMeshToDraw = Objects::GetDefaultCubeMesh();

	Vertex* vertices = new Vertex[mMeshToDraw->vertices.size()];


	for (unsigned i = 0; i < mMeshToDraw->vertices.size(); i++)
	{
		vertices[i].pos = mMeshToDraw->vertices[i].pos;
		vertices[i].nor = mMeshToDraw->vertices[i].nor;
		vertices[i].uv  = mMeshToDraw->vertices[i].uv;
	}

	D3D11_BUFFER_DESC _vbd, _ibd;
	D3D11_SUBRESOURCE_DATA _initData;

	ZeroMemory(&_vbd, sizeof(D3D11_BUFFER_DESC));
	_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_vbd.ByteWidth = (UINT)mMeshToDraw->vertices.size() * sizeof(Vertex);
	_vbd.Usage = D3D11_USAGE_IMMUTABLE;
	_vbd.CPUAccessFlags = 0;
	_vbd.MiscFlags = 0;
	_vbd.StructureByteStride = 0;

	ZeroMemory(&_initData, sizeof(D3D11_SUBRESOURCE_DATA));
	//_initData.pSysMem = mMeshToDraw->vertices.data();
	_initData.pSysMem = vertices;

	HR(md3dDevice->CreateBuffer(&_vbd, &_initData, &mVB));

	delete[] vertices;

	// IB:
	ZeroMemory(&_ibd, sizeof(D3D11_BUFFER_DESC));
	_ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	_ibd.ByteWidth = (UINT)mMeshToDraw->indices.size() * sizeof(UINT);
	_ibd.Usage = D3D11_USAGE_IMMUTABLE;
	_ibd.CPUAccessFlags = 0;
	_ibd.MiscFlags = 0;
	_ibd.StructureByteStride = 0;

	ZeroMemory(&_initData, sizeof(_initData));
	_initData.pSysMem = mMeshToDraw->indices.data();
	HR(md3dDevice->CreateBuffer(&_ibd, &_initData, &mIB));
	mIndexToDraw =(UINT) mMeshToDraw->indices.size();

}

void Scene::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


#pragma region VS

	cbd.ByteWidth = sizeof(VSCBPerFrame);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, &mCBVSPerFrame));

	cbd.ByteWidth = sizeof(VSCBPerObj);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, &mCBVSPerObj));

#pragma endregion

#pragma region PS
	cbd.ByteWidth = sizeof(PSCBPerFrame);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, &mCBPSPerFrame));

	cbd.ByteWidth = sizeof(PSCBPerObj);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, &mCBPSPerObj));

#pragma endregion



}

void Scene::CreateShaders()
{	
	std::ifstream fin;
	fin.open("Shaders/VertexShader.cso", std::ios::binary);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios_base::end);
		int byteSize = (int)fin.tellg();
		std::vector<char> byteCode(byteSize);
		fin.seekg(0, std::ios_base::beg);
		fin.read(byteCode.data(), byteSize);

		HR(md3dDevice->CreateVertexShader(byteCode.data(), byteSize, nullptr, &mVS));
		fin.close();

		D3D11_INPUT_ELEMENT_DESC id[3] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		HR(md3dDevice->CreateInputLayout(id, _countof(id), byteCode.data(), byteSize, &mIL));
	}

	fin.open("Shaders/PixelShader.cso", std::ios::binary);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios_base::end);
		int byteSize = (int)fin.tellg();
		std::vector<char> byteCode(byteSize);
		fin.seekg(0, std::ios_base::beg);
		fin.read(byteCode.data(), byteSize);

		HR(md3dDevice->CreatePixelShader(byteCode.data(), byteSize, nullptr, &mPS));
		fin.close();
	}

}

void Scene::CreateSampleState()
{

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MipLODBias = 0.0f;
	sd.MaxAnisotropy = 4;

	HR(md3dDevice->CreateSamplerState(&sd, &mSS));

}



void Scene::DrawScene()
{


	Camera* const mainCamera = GetMainCamera();
	md3dImmediateContext->ClearRenderTargetView(md3dRTV, DirectX::Colors::Black);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



#pragma region Update VS CBuffer
	D3D11_MAPPED_SUBRESOURCE mSource;
	// VS perFrame

	
	XMFLOAT3 camPos = mainCamera->GetPosition();
	XMStoreFloat4x4(&mVSPerFrame.camWVP, XMMatrixTranspose(XMMatrixTranslation(camPos.x, camPos.y, camPos.z) * mainCamera->GetViewProj()));
	// mapping

	ZeroMemory(&mSource, sizeof(mSource));
	HR(md3dImmediateContext->Map(mCBVSPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mSource));
	memcpy(mSource.pData, &mVSPerFrame, sizeof(mVSPerFrame));
	md3dImmediateContext->Unmap(mCBVSPerFrame, 0);

	md3dImmediateContext->VSSetConstantBuffers(0, 1, &mCBVSPerFrame);

	// VS obj

	XMMATRIX world = XMMatrixScaling(20, 0.5f, 20);

	XMStoreFloat4x4(&mVSPerObj.world, XMMatrixTranspose(world));
	XMStoreFloat4x4(&mVSPerObj.wvp, XMMatrixTranspose(XMMatrixMultiply(world, GetMainCamera()->GetViewProj())));

	XMVECTOR det = XMMatrixDeterminant(world);
	XMStoreFloat4x4(&mVSPerObj.worldInvTranspose, XMMatrixInverse(&det, world));

	//mapping
	ZeroMemory(&mSource, sizeof(mSource));
	HR(md3dImmediateContext->Map(mCBVSPerObj, 0, D3D11_MAP_WRITE_DISCARD, 0, &mSource));
	memcpy(mSource.pData, &mVSPerObj, sizeof(mVSPerObj));
	md3dImmediateContext->Unmap(mCBVSPerObj, 0);

	md3dImmediateContext->VSSetConstantBuffers(1, 1, &mCBVSPerObj);
#pragma endregion

#pragma region Update PS cbuffer
	// PS perframe
	ZeroMemory(&mSource, sizeof(mSource));

	// update light direction along Y axis	 
	//XMVECTOR lightDir = XMVectorSet(mPSPerFrame.light.direction.x, mPSPerFrame.light.direction.y, mPSPerFrame.light.direction.z, 0);
	//XMVECTOR yAxis = XMVectorSet(0, 1, 0, 0);
	//XMStoreFloat3(&mPSPerFrame.light.direction, XMVector3Rotate(lightDir, XMQuaternionRotationNormal(yAxis, GetDeltaTime())));

	//if (mPSPerFrame.light.position.y > 8)
	//{
	//	speedDir = -1;
	//}
	//else if (mPSPerFrame.light.position.y < 3)
	//{
	//	speedDir = 1;
	//}

	//mPSPerFrame.light.position.y += speedDir * GetDeltaTime();


	// map
	HR(md3dImmediateContext->Map(mCBPSPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mSource));
	memcpy(mSource.pData, &mPSPerFrame, sizeof(mPSPerFrame));
	md3dImmediateContext->Unmap(mCBPSPerFrame, 0);
	// set buffer
	md3dImmediateContext->PSSetConstantBuffers(0, 1, &mCBPSPerFrame);




	// PS OBJ
	ZeroMemory(&mSource, sizeof(mSource));


	HR(md3dImmediateContext->Map(mCBPSPerObj, 0, D3D11_MAP_WRITE_DISCARD, 0, &mSource));
	memcpy(mSource.pData, &mPSPerObj, sizeof(mPSPerObj));
	md3dImmediateContext->Unmap(mCBPSPerObj, 0);

	md3dImmediateContext->PSSetConstantBuffers(1, 1, &mCBPSPerObj);
#pragma endregion

	UINT stride, offset;
	ID3D11ShaderResourceView* tex;

#pragma region Draw Sky Box

	md3dImmediateContext->RSSetState(mRSBackFCC.Get());
	tex = Objects::GetSkyTexuture();

	stride = sizeof(Vertex12);
	offset = 0;

	md3dImmediateContext->IASetInputLayout(skyIL.Get());
	md3dImmediateContext->IASetVertexBuffers(0, 1, skyVB.GetAddressOf(), &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(skyIB.Get(), DXGI_FORMAT_R32_UINT, 0);

	md3dImmediateContext->VSSetShader(skyVS.Get(), 0, 0);
	md3dImmediateContext->PSSetShader(skyPS.Get(), 0, 0);

	md3dImmediateContext->PSSetShaderResources(1, 1, &tex);
	md3dImmediateContext->DrawIndexed(skyIndexSize, 0, 0);

	md3dImmediateContext->RSSetState(mRSDef.Get());

#pragma endregion

	md3dImmediateContext->ClearDepthStencilView(md3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



	md3dImmediateContext->IASetInputLayout(mIL);

	stride = sizeof(Vertex);
	offset = 0;



	md3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);



	md3dImmediateContext->VSSetShader(mVS, 0, 0);
	md3dImmediateContext->PSSetShader(mPS, 0, 0);

	tex = Objects::GetGrassTexture();
	md3dImmediateContext->PSSetShaderResources(0, 1, &tex);
	md3dImmediateContext->PSSetSamplers(0, 1, &mSS);

	md3dImmediateContext->DrawIndexed(mIndexToDraw, 0, 0);






	HR(mSwapChain->Present(0, 0));
}
