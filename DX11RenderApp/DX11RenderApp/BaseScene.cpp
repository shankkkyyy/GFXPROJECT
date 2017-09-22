#include "BaseScene.h"
#include "SkyBox.h"
#include "Engine.h"
#include "terrian.h"

#pragma region BaseScene


Shader*              BaseScene::mShaders = nullptr;
Engine*              BaseScene::mEngine = nullptr;
ID3D11Device*        BaseScene::md3dDevice = nullptr;
ID3D11DeviceContext* BaseScene::md3dImmediateContext = nullptr;


BaseScene::BaseScene()
{
	mCurrentCam = mEngine->GetMainCamera();
}

BaseScene::~BaseScene()
{

}

void BaseScene::LinkResource(class Engine* _engine)
{
	mEngine              = _engine;
	mShaders             = mEngine->GetShaders();
	md3dDevice           = mEngine->GetDevice();
	md3dImmediateContext = mEngine->GetDeviceContext();
}

void BaseScene::InitScene()
{
	CreateConstantBuffer();
	CreateGeometryBuffer();
	InitialRenderSettings();
}

void BaseScene::Render()
{
	RenderOffScreen();
	RenderToScreen();
}

void BaseScene::CleanScene()
{
	// reset Shaders
	md3dImmediateContext->VSSetShader(nullptr, 0, 0);
	md3dImmediateContext->HSSetShader(nullptr, 0, 0);
	md3dImmediateContext->DSSetShader(nullptr, 0, 0);
	md3dImmediateContext->GSSetShader(nullptr, 0, 0);
	md3dImmediateContext->PSSetShader(nullptr, 0, 0);
	// Release Constant Buffer

	mCBPerObj_VS.Reset();
	mCBPerObj_PS.Reset();
	mCBPerFrame_VS.Reset();
	mCBPerFrame_HS.Reset();
	mCBPerFrame_DS.Reset();
	mCBPerFrame_GS.Reset();
	mCBPerFrame_PS.Reset();

	// Release Geometry Buffer
	mVertexBuffer.Reset();
	mIndexBuffer.Reset();
	mInstanceBuffer.Reset();
}

void BaseScene::InitialRenderSettings()
{
	// Set Camera position
	mCurrentCam->SetPosition(XMFLOAT3(0, 5, -30.0f));
	mCurrentCam->SetRotation(0, 0, 0);

}

ID3D11Buffer * BaseScene::GetPSCBPerObj() const
{
	return mCBPerObj_PS.Get();
}

ID3D11Buffer * BaseScene::GetVSCBPerObj() const
{
	return mCBPerObj_VS.Get();
}

ID3D11Buffer * BaseScene::GetInstanceBuffer() const
{
	return mInstanceBuffer.Get();
}

Camera * BaseScene::GetCurrentCamera() const
{
	return mCurrentCam;
}


void BaseScene::RenderOffScreen()
{
	ID3D11RenderTargetView* offscreenRTV = mEngine->GetOffScreenRTV();
	ID3D11DepthStencilView* dsv = mEngine->GetDepthStencilView();
	md3dImmediateContext->OMSetRenderTargets(1, &offscreenRTV, dsv);
	md3dImmediateContext->ClearRenderTargetView(offscreenRTV, DirectX::Colors::Yellow);
	md3dImmediateContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void BaseScene::RenderToScreen()
{
	ID3D11RenderTargetView* screenRTV = mEngine->GetRenderTarget();
	ID3D11DepthStencilView* dsv = mEngine->GetDepthStencilView();
	md3dImmediateContext->OMSetRenderTargets(1, &screenRTV, dsv);
	md3dImmediateContext->ClearRenderTargetView(screenRTV, DirectX::Colors::SkyBlue);
	md3dImmediateContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void BaseScene::GetIndexAndVertexSize(UINT & totalVerticeSize, UINT & totalIndicesSize, std::vector<Object*>* _objList)
{
	for (size_t i = 0; i < _objList->size(); i++)
	{
		totalVerticeSize += (UINT)(*_objList)[i]->GetMesh()->vertices.size();
		totalIndicesSize += (UINT)(*_objList)[i]->GetMesh()->indices.size();
	}
}

void BaseScene::GetIndexAndVertexStartIndex(size_t & iOffset, size_t & vOffset, UINT * _indices, Vertex * _vertices, std::vector<Object*>* _objList)
{
	for (size_t i = 0; i < _objList->size(); i++)
	{
		size_t vlength = (*_objList)[i]->GetMesh()->vertices.size();
		for (size_t iV = 0; iV < vlength; iV++)
		{
			_vertices[iV + vOffset] = (*_objList)[i]->GetMesh()->vertices[iV];
		}
		// reset length to indices size
		size_t ilength = (*_objList)[i]->GetMesh()->indices.size();
		for (size_t iI = 0; iI < ilength; iI++)
		{
			_indices[iI + iOffset] = (*_objList)[i]->GetMesh()->indices[iI];
		}

		// get the Index && vertex offset for index draw within one buffer
		(*_objList)[i]->SetIndexOffset((UINT)iOffset, (UINT)vOffset);

		// update offset
		vOffset += vlength;
		iOffset += ilength;
	}
}


#pragma endregion


#pragma region CountrySide

CountrySide::CountrySide() : BaseScene::BaseScene()
{
}

CountrySide::~CountrySide()
{
	delete mTerrian;
}

void CountrySide::EditScene()
{
	
	mTerrian = new Terrian();
	Terrian::TerrianInfo tInfo;
	tInfo.cellSpacing = 0.5f;
	tInfo.heightMapDepth = 2049;
	tInfo.heightMapWidth = 2049;
	tInfo.heightScale = 50.0f;
	tInfo.heightMap = L"../Models/terrain.raw";
	mTerrian->Initialize(tInfo);

}

void CountrySide::CreateGeometryBuffer()
{
	mTerrian->CreateGeometryBuffer();
}

void CountrySide::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	cbd.ByteWidth = sizeof(mHSCBPerFrame);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mCBPerFrame_HS.GetAddressOf()));

	cbd.ByteWidth = sizeof(mDSCBPerFrame);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mCBPerFrame_DS.GetAddressOf()));
}

void CountrySide::InitialRenderSettings()
{


	ID3D11SamplerState* ss = mEngine->GetSSANISOTROPICWRAP();
	md3dImmediateContext->DSSetSamplers(0, 1, &ss);


	md3dImmediateContext->VSSetShader(mShaders->GetTerrianVS(), 0, 0);
	md3dImmediateContext->HSSetShader(mShaders->GetTerrianHS(), 0, 0);
	md3dImmediateContext->DSSetShader(mShaders->GetTerrianDS(), 0, 0);
	md3dImmediateContext->PSSetShader(mShaders->GetTerrianPS(), 0, 0);


	// set height map
	ID3D11ShaderResourceView* resourceSRV = mTerrian->GetHeightSRV();
	md3dImmediateContext->VSSetShaderResources(12, 1, &resourceSRV);
	md3dImmediateContext->DSSetShaderResources(12, 1, &resourceSRV);

	// set layer map and blend map
	resourceSRV = mTerrian->GetBlendSRV();
	md3dImmediateContext->PSSetShaderResources(13, 1, &resourceSRV);
	resourceSRV = mTerrian->GetLayerSRV();
	md3dImmediateContext->PSSetShaderResources(11, 1, &resourceSRV);

	BaseScene::InitialRenderSettings();

}

void CountrySide::Update(float _deltaTime)
{
	if (GetAsyncKeyState(KEY_R) & 0x8000)
	{
		SwitchToWireFrame();
	}
	else if (GetAsyncKeyState(VK_HOME) & 0x8000)
	{
		SwitchToNormal();
	}
}

void CountrySide::RenderOffScreen()
{
}

void CountrySide::RenderToScreen()
{
	BaseScene::RenderToScreen();
	// update Buffer
	const Camera* const mMainCamera = mEngine->GetMainCamera();

	mHSCBPerFrame.eyePos = mMainCamera->GetPosition();
	XMStoreFloat4x4(&mDSCBPerFrame.worldViewProj, XMMatrixTranspose(mMainCamera->GetViewProjXM()));

	D3D11_MAPPED_SUBRESOURCE mapResource;

	ZeroMemory(&mapResource, sizeof(mapResource));
	HR(md3dImmediateContext->Map(mCBPerFrame_HS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));
	memcpy(mapResource.pData, &mHSCBPerFrame, sizeof(mHSCBPerFrame));
	md3dImmediateContext->Unmap(mCBPerFrame_HS.Get(), 0);

	md3dImmediateContext->HSSetConstantBuffers(0, 1, mCBPerFrame_HS.GetAddressOf());

	ZeroMemory(&mapResource, sizeof(mapResource));
	HR(md3dImmediateContext->Map(mCBPerFrame_DS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));
	memcpy(mapResource.pData, &mDSCBPerFrame, sizeof(mDSCBPerFrame));
	md3dImmediateContext->Unmap(mCBPerFrame_DS.Get(), 0);

	md3dImmediateContext->DSSetConstantBuffers(0, 1, mCBPerFrame_DS.GetAddressOf());

	mTerrian->Draw();
}

void CountrySide::SwitchToWireFrame()
{
	md3dImmediateContext->RSSetState(mEngine->GetRSWireFrame());
	md3dImmediateContext->PSSetShader(mShaders->GetTessPS(), 0, 0);
}

void CountrySide::SwitchToNormal()
{
	md3dImmediateContext->RSSetState(nullptr);
	md3dImmediateContext->PSSetShader(mShaders->GetTerrianPS(), 0, 0);

}

void CountrySide::CleanScene()
{
	BaseScene::CleanScene();
	mTerrian->ReleaseResource();
}

#pragma endregion


#pragma region CarDemo
CarDemo::CarDemo() : BaseScene::BaseScene()
{
}

CarDemo::~CarDemo()
{
	delete mSkyBox;
	delete[] mTrees;

	size_t i = 0;
	// delete reflection objs data
	for (; i < mReflectionObjs->size(); i++)
	{
		delete (*mReflectionObjs)[i];
	}
	delete mReflectionObjs;

	// delete opague objs data
	for (i = 0; i < mOpagueObjs->size(); i++)
	{
		delete (*mOpagueObjs)[i];
	}
	delete mOpagueObjs;
	// delete transparent obj data
	for (i = 0; i < mTransparentObjs->size(); i++)
	{
		delete (*mTransparentObjs)[i];
	}
	delete mTransparentObjs;
	// delete instance data
	for (i = 0; i < mInstances->size(); i++)
	{
		delete (*mInstances)[i];
	}
	delete mInstances;
	delete mInstanceData;
}

void CarDemo::EditScene()
{
#pragma region Scene Setup
	// set up the camera
	mEngine->GetMainCamera()->SetPosition(XMFLOAT3(0, 5, -50));
	std::vector<ID3D11ShaderResourceView*> textures;

	mOpagueObjs = new std::vector<Object*>();
	mReflectionObjs = new std::vector<Object*>();
	mTransparentObjs = new std::vector<Object*>();
	mInstances = new std::vector<Object*>();

	// Surface
	Object* surface = new Object();
	surface->SetName("surface");
	textures.clear();
	textures.resize(1);
	textures[0] = Objects::GetFloorTexture();
	surface->Edit(Objects::GetDefaultPlaneMesh(), nullptr, textures.data(), (UINT)textures.size(), SingleDiffuse);
	surface->SetPosition(XMFLOAT3(0, -0.01f, 10));
	surface->SetScale(XMFLOAT3(200, 0.25f, 200));
	surface->SetTexTransform(5, 5);
	mOpagueObjs->push_back(surface);

	// sphere
	Object* sphere = new Object();
	sphere->SetName("sphere");
	textures.clear();
	textures.resize(1);
	textures[0] = Objects::GetSkyTexuture();
	sphere->Edit(Objects::GetSphereMesh(), Objects::GetSilverMaterial(), textures.data(), UINT(textures.size()), StaticCube);
	sphere->SetPosition(XMFLOAT3(-100, 10, -30));
	sphere->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	sphere->SetInstanceCount(400);
	mInstances->push_back(sphere);

	// Car
	Object* car = new Object();
	car->SetName("car");
	textures.clear();
	textures.resize(1);
	textures[0] = Objects::GetCarTexture();
	car->Edit(Objects::GetCarMesh(), nullptr, textures.data(), (UINT)textures.size(), SingleDiffuse);
	car->SetPosition(XMFLOAT3(0, 0, 5));
	car->Rotate(0, 90, 0);
	mOpagueObjs->push_back(car);


	// transparent cube
	Object* transparentCube = new Object();
	transparentCube->SetName("transparentCube");
	transparentCube->Edit(Objects::GetDefaultCubeMesh(), nullptr, nullptr, 0);
	transparentCube->SetPosition(XMFLOAT3(0, 5, -15));
	transparentCube->SetScale(XMFLOAT3(10, 10, 1));
	transparentCube->SetDiffuseColor(DirectX::Colors::Cyan, 0.3f);
	transparentCube->SetTransparent(true);
	mTransparentObjs->push_back(transparentCube);

	Object* transparentSphere = new Object();
	transparentSphere->SetName("transparentSphere");
	transparentSphere->Edit(Objects::GetSphereMesh(), nullptr, nullptr, 0);
	transparentSphere->SetPosition(XMFLOAT3(30, 10, 0));
	transparentSphere->SetDiffuseColor(DirectX::Colors::RoyalBlue, 0.3f);
	transparentSphere->SetTransparent(true);
	mTransparentObjs->push_back(transparentSphere);

	// mirror
	Object* mirror = new Object();
	mirror->SetName("mirror");
	textures.clear();
	textures.resize(1);
	textures[0] = Objects::GetIceTexture();
	mirror->Edit(Objects::GetDefaultCubeMesh(), nullptr, textures.data(), (UINT)textures.size(), SingleDiffuse);
	mirror->SetPosition(XMFLOAT3(-30, 7.5f, 0));
	mirror->SetScale(XMFLOAT3(1, 15, 25));
	mirror->SetTexTransform(3, 5);
	mirror->SetDiffuseAlpha(0.3f);
	mirror->SetTransparent(true);
	mTransparentObjs->push_back(mirror);


	// Wall
	Object* wall = new Object();
	wall->SetName("Wall");
	textures.clear();
	textures.resize(1);
	textures[0] = Objects::GetWallTexture();
	wall->Edit(Objects::GetDefaultCubeMesh(), nullptr, textures.data(), (UINT)textures.size(), SingleDiffuse);
	wall->SetPosition(XMFLOAT3(-40, 7.5f, 0));
	wall->SetScale(XMFLOAT3(1, 15, 25));
	wall->SetTexTransform(3, 5);
	mOpagueObjs->push_back(wall);


	// IceWall
	Object* IceWall = new Object();
	IceWall->SetName("IceWall");
	textures.clear();
	textures.resize(2);
	textures[0] = Objects::GetWallTexture();
	textures[1] = Objects::GetIceTexture();
	IceWall->Edit(Objects::GetDefaultCubeMesh(), nullptr, textures.data(), (UINT)textures.size(), DoubleDiffuse);
	IceWall->SetPosition(XMFLOAT3(-50, 7.5f, 0));
	IceWall->SetScale(XMFLOAT3(1, 15, 25));
	IceWall->SetTexTransform(3, 5);
	mOpagueObjs->push_back(IceWall);

	// reflection sphere
	Object* reflectiveSphere = new Object();
	reflectiveSphere->SetName("reflectiveSphere");
	reflectiveSphere->Edit(Objects::GetSphereMesh(), Objects::GetSilverMaterial(), nullptr, 0);
	reflectiveSphere->SetPosition(XMFLOAT3(0, 15, 40));
	reflectiveSphere->SetScale(XMFLOAT3(1.5f, 1.5f, 1.5f));

	mReflectionObjs->push_back(reflectiveSphere);


	// setup the dynamic cube map Camera
	XMFLOAT4 rSpherePosition = reflectiveSphere->GetPosition();
	mEngine->SetDynamicCubeMapCamera(XMFLOAT3(rSpherePosition.x, rSpherePosition.y, rSpherePosition.z));



	// build instance data
	mInstanceData = new std::vector<InstanceData>(sphere->GetInstanceAmount());

	UINT column = 20;
	UINT raw = (UINT)mInstanceData->size() / column;

	size_t index;

	for (size_t iR = 0; iR < raw; iR++)
	{
		for (size_t iC = 0; iC < column; iC++)
		{
			index = iR * column + iC;
			(*mInstanceData)[index].world = sphere->GetWorldMatrix();
			(*mInstanceData)[index].world(3, 0) += 20 * iC;//x
			(*mInstanceData)[index].world(3, 1) += 10 * iR;
		}
	}




	mTreeAmount = 15;
	mTrees = new VertexBB[mTreeAmount];
	float treeSize = 20;

	for (size_t i = 0; i < mTreeAmount; i++)
	{
		mTrees[i].root.x = (MathHelper::RandF() - 0.5f) * 180;
		mTrees[i].root.y = 0;


		mTrees[i].root.z = (float)(rand() % 50) + 50;
		mTrees[i].size.x = MathHelper::Clamp(MathHelper::RandF(), 0.5f, 1.0f) * treeSize;
		mTrees[i].size.y = MathHelper::Clamp(MathHelper::RandF(), 0.7f, 1.3f) * treeSize;
	}

	mTreeToVRAM.material = *Objects::GetDefMaterial();
	mTreeToVRAM.renderSetting.x = 1;
	mTreeToVRAM.renderSetting.y = 0;

#pragma endregion

	// sky Box
	mSkyBox = new SkyBox();
	mSkyBox->
		Edit(mShaders->GetSkyVS(), mShaders->GetSkyPS(), Objects::GetSkyTexuture(), mShaders->GetPosIL(), md3dDevice);


#pragma region Objs Init

	// init lighting
	ZeroMemory(&mToVRAMPerFrame_VS, sizeof(VSCBPerFrame));
	mToVRAMPerFrame_PS.ambientLight = { 0.3f, 0.3f, 0.3f, 1.0f };

	// Dir light
	mToVRAMPerFrame_PS.light[0].lightColor = { .8f, .8f, .8f };
	mToVRAMPerFrame_PS.light[0].direction = { 1, -1, 1 };
	XMStoreFloat3(&mToVRAMPerFrame_PS.light[0].direction,
		XMVector3Normalize(MathHelper::XMFloat3ToXMVector(mToVRAMPerFrame_PS.light[0].direction)));

	// Point light 
	mToVRAMPerFrame_PS.light[1].lightColor = XMFLOAT3((const float*)&Colors::Gold);;
	mToVRAMPerFrame_PS.light[1].position = { 0.0f, 15.0f, 40.0f };
	mToVRAMPerFrame_PS.light[1].fallOffStart = 0;
	mToVRAMPerFrame_PS.light[1].fallOffend = 40;

	// Spot light - 
	mToVRAMPerFrame_PS.light[2].lightColor = XMFLOAT3((const float*)&Colors::DeepPink);
	mToVRAMPerFrame_PS.light[2].direction = { -1, -.3f, 1 };
	XMStoreFloat3(&mToVRAMPerFrame_PS.light[2].direction,
		XMVector3Normalize(MathHelper::XMFloat3ToXMVector(mToVRAMPerFrame_PS.light[2].direction)));
	mToVRAMPerFrame_PS.light[2].position = { 25.0f, 7.5f, -30.0f };
	mToVRAMPerFrame_PS.light[2].fallOffStart = 0;
	mToVRAMPerFrame_PS.light[2].fallOffend = 50;
	mToVRAMPerFrame_PS.light[2].spotAngle = 0.7f; //45

	pointSpeedY = 1.0f;

	XMFLOAT4 fogColor = XMFLOAT4((const float*)(&DirectX::Colors::WhiteSmoke));
	mToVRAMPerFrame_PS.fogColor = { fogColor.x, fogColor.y, fogColor.z };
	mToVRAMPerFrame_PS.fogStart = 15.0f;
	mToVRAMPerFrame_PS.fogRange = 50.0f;


#pragma endregion
}

void CarDemo::CreateGeometryBuffer()
{
	// for screen quad
	// for Obj
	UINT                   objVerticeSize = 0, objIndicesSize = 0;
	D3D11_BUFFER_DESC      vbd, ibd, instbd;
	D3D11_SUBRESOURCE_DATA initData;
	size_t i = 0;
	size_t vOffset = 0, iOffset = 0;

#pragma region Gather all vertices and indices for scene objectes into to two separate array

	GetIndexAndVertexSize(objVerticeSize, objIndicesSize, mOpagueObjs);
	GetIndexAndVertexSize(objVerticeSize, objIndicesSize, mReflectionObjs);
	GetIndexAndVertexSize(objVerticeSize, objIndicesSize, mInstances);
	GetIndexAndVertexSize(objVerticeSize, objIndicesSize, mTransparentObjs);

	// load all mesh into a vertex array
	// load all indices into an indices array

	UINT* objIndices = new UINT[objIndicesSize];
	Vertex* objVertices = new Vertex[objVerticeSize];

	GetIndexAndVertexStartIndex(iOffset, vOffset, objIndices, objVertices, mOpagueObjs);
	GetIndexAndVertexStartIndex(iOffset, vOffset, objIndices, objVertices, mReflectionObjs);
	GetIndexAndVertexStartIndex(iOffset, vOffset, objIndices, objVertices, mInstances);
	GetIndexAndVertexStartIndex(iOffset, vOffset, objIndices, objVertices, mTransparentObjs);

#pragma endregion

#pragma region Create Buffer and push data into VB and IB for non - geometry shader generated obj
	// VB
	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = objVerticeSize * sizeof(Vertex);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = objVertices;

	HR(md3dDevice->CreateBuffer(&vbd, &initData, mVertexBuffer.GetAddressOf()));


	// IB:
	ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = objIndicesSize * sizeof(UINT);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = objIndices;
	HR(md3dDevice->CreateBuffer(&ibd, &initData, mIndexBuffer.GetAddressOf()));

	// Inst Buffer:
	ZeroMemory(&instbd, sizeof(D3D11_BUFFER_DESC));
	instbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instbd.ByteWidth = (UINT)mInstanceData->size() * sizeof(InstanceData);
	instbd.Usage = D3D11_USAGE_DYNAMIC;
	instbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instbd.MiscFlags = 0;
	instbd.StructureByteStride = 0;

	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = mInstanceData->data();
	HR(md3dDevice->CreateBuffer(&instbd, &initData, mInstanceBuffer.GetAddressOf()));
#pragma endregion

#pragma region Create Buffer and push data into VB and IB for geometry shader generated obj

	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = mTreeAmount * sizeof(VertexBB);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = mTrees;

	HR(md3dDevice->CreateBuffer(&vbd, &initData, mGeoVB.GetAddressOf()));

#pragma endregion


	delete[] objVertices;
	delete[] objIndices;
}

void CarDemo::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


	cbd.ByteWidth = sizeof(VSCBPerFrame);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mCBPerFrame_VS.GetAddressOf()));

	cbd.ByteWidth = sizeof(VSCBPerObj);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mCBPerObj_VS.GetAddressOf()));

	cbd.ByteWidth = sizeof(PSCBPerFrame);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mCBPerFrame_PS.GetAddressOf()));

	cbd.ByteWidth = sizeof(PSCBPerObj);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mCBPerObj_PS.GetAddressOf()));

	cbd.ByteWidth = sizeof(GSCBPerFrame);
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, mCBPerFrame_GS.GetAddressOf()));

}

void CarDemo::InitialRenderSettings()
{
	BaseScene::InitialRenderSettings();
}

void CarDemo::Update(float _deltaTime)
{
	CXMVECTOR zAxis = XMVectorSet(0, 0, 1, 0);
	CXMVECTOR yAxis = XMVectorSet(0, 1, 0, 0);

	//rotate direction light on Z;
	CXMMATRIX rotationZ = XMMatrixRotationNormal(zAxis, 0.5f * _deltaTime);

	XMStoreFloat3(&mToVRAMPerFrame_PS.light[0].direction,
		XMVector4Transform(XMLoadFloat3(&mToVRAMPerFrame_PS.light[0].direction), rotationZ));

	//move pointlight 
	CXMMATRIX rotationY = XMMatrixRotationNormal(yAxis, 0.5f * _deltaTime);

	XMStoreFloat3(&mToVRAMPerFrame_PS.light[1].position,
		XMVector4Transform(XMLoadFloat3(&mToVRAMPerFrame_PS.light[1].position), rotationY));

	// move spot light

	XMStoreFloat3(&mToVRAMPerFrame_PS.light[2].position,
		XMVector4Transform(XMLoadFloat3(&mToVRAMPerFrame_PS.light[2].position), rotationY));
	XMMATRIX rotationNegY = XMMatrixTranspose(rotationY);
	XMStoreFloat3(&mToVRAMPerFrame_PS.light[2].direction,
		XMVector4Transform(XMLoadFloat3(&mToVRAMPerFrame_PS.light[2].direction), rotationY));
}

void CarDemo::RenderOffScreen()
{
	// Draw Cube Map
	md3dImmediateContext->RSSetViewports(1, mEngine->GetCubeMapViewPort());
	ID3D11DepthStencilView* cubeMapDsv = mEngine->GetDynamicCubeMapDSV();
	ID3D11RenderTargetView* currentRTV = nullptr;
	for (size_t i = 0; i < 6; i++)
	{
		mCurrentCam = mEngine->GetDynamicCubeMapCamera()[i];
		MapConstantBuffer();
		currentRTV = mEngine->GetDynamicCubeMapRTV((UINT)i);
		md3dImmediateContext->ClearDepthStencilView(cubeMapDsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		md3dImmediateContext->ClearRenderTargetView(currentRTV, DirectX::Colors::Aqua);
		md3dImmediateContext->OMSetRenderTargets(1, &currentRTV, cubeMapDsv);

		DrawOpaques(false);
		DrawSkyBox();
		DrawTransparents();
	}

	// Generate Mip Map for cube map
	md3dImmediateContext->GenerateMips(mEngine->GetDynamicCubeMapSRV());

	// Draw Scene
	BaseScene::RenderOffScreen();
	md3dImmediateContext->RSSetViewports(1, mEngine->GetScreenViewPort());

	mCurrentCam = mEngine->GetMainCamera();
	MapConstantBuffer();

	DrawOpaques(true);
	DrawSkyBox();
	DrawTransparents();
}

void CarDemo::RenderToScreen()
{
	BaseScene::RenderToScreen();

	ID3D11Buffer* screenQuad = mEngine->GetScreenVertexBuffer();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	md3dImmediateContext->IASetVertexBuffers(0, 1, &screenQuad, &stride, &offset);
	md3dImmediateContext->PSSetShader(mShaders->GetRTTPSRed(), nullptr, 0);
	md3dImmediateContext->VSSetShader(mShaders->GetRTTVS(), nullptr, 0);

	// For house keeping
	ID3D11ShaderResourceView* nullSRV = nullptr;	
	ID3D11ShaderResourceView* offsreenSRV = mEngine->GetOffScreenSRV();

	if (mEngine->GetIsBlurOn())
	{
		mEngine->AddBlur();
	}


	md3dImmediateContext->PSSetShaderResources(9, 1, &offsreenSRV);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// draw
	md3dImmediateContext->Draw(4, 0);
	md3dImmediateContext->PSSetShaderResources(9, 1, &nullSRV);	
}

void CarDemo::CleanScene()
{
	BaseScene::CleanScene();
	mGeoVB.Reset();
}

void CarDemo::DrawOpaques(bool _drawReflection)
{
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

#pragma region Draws

	// 1. Set Vertex, Index Buffer , shaders for all objects to draw

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->VSSetShader(mShaders->GetObjVS(), 0, 0);
	md3dImmediateContext->IASetInputLayout(mShaders->GetBasic32IL());

	// 2. Draw Every Object
	for (size_t iOpa = 0; iOpa < mOpagueObjs->size(); iOpa++)
	{
		(*mOpagueObjs)[iOpa]->Draw();
	}

	if (_drawReflection)
	{

		ID3D11ShaderResourceView* textures[1] = { mEngine->GetDynamicCubeMapSRV() };
		for (size_t iReflect = 0; iReflect < mReflectionObjs->size(); iReflect++)
		{

			(*mReflectionObjs)[iReflect]->SetTexture(textures, 1, DynamicCube);
			(*mReflectionObjs)[iReflect]->Draw();
		}
	}

#pragma endregion

#pragma region Draw Instances

	UINT strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
	UINT offsets[2] = { 0, 0 };

	ID3D11Buffer* vbs[2] = { mVertexBuffer.Get(), mInstanceBuffer.Get() };

	md3dImmediateContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->VSSetShader(mShaders->GetInstVS(), 0, 0);

	md3dImmediateContext->IASetInputLayout(mShaders->GetBasic32ILInst());

	for (size_t iInst = 0; iInst < mInstances->size(); iInst++)
	{
		(*mInstances)[iInst]->DrawInstance(mInstanceData->data());
	}

#pragma endregion

#pragma region  Draw Geometry shader generated objs


	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	stride = sizeof(VertexBB);
	offset = 0;
	md3dImmediateContext->IASetInputLayout(mShaders->GetPosSize());

	md3dImmediateContext->IASetVertexBuffers(0, 1, mGeoVB.GetAddressOf(), &stride, &offset);
	// set shaders
	md3dImmediateContext->VSSetShader(mShaders->GetBBVS(), 0, 0);
	md3dImmediateContext->GSSetShader(mShaders->GetBBGS(), 0, 0);
	md3dImmediateContext->PSSetShader(mShaders->GetBBPS(), 0, 0);
	// set InputLayout

	// update PSCB
	D3D11_MAPPED_SUBRESOURCE mapResource;
	ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HR(md3dImmediateContext->Map(mCBPerObj_PS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));
	memcpy(mapResource.pData, &mTreeToVRAM, sizeof(mTreeToVRAM));
	md3dImmediateContext->Unmap(mCBPerObj_PS.Get(), 0);
	md3dImmediateContext->PSSetConstantBuffers(1, 1, mCBPerObj_PS.GetAddressOf());

	// Get Texture
	ID3D11ShaderResourceView* treeArrayTex = Objects::GetTreeArrayTexture();
	md3dImmediateContext->PSSetShaderResources(11, 1, &treeArrayTex);

	// Draw
	

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	md3dImmediateContext->Draw(mTreeAmount, 0);

	// restore topology
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->GSSetShader(nullptr, 0, 0);


#pragma endregion
}

void CarDemo::DrawSkyBox()
{
	// Change Rasterization State, draw Triangle inside out
	md3dImmediateContext->RSSetState(mEngine->GetRSFrontCull());
	md3dImmediateContext->OMSetDepthStencilState(mEngine->GetDSLessEqual(), 0);

	mSkyBox->Draw(md3dImmediateContext);

	// Restore Rasterization State
	md3dImmediateContext->RSSetState(nullptr);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
}

void CarDemo::DrawTransparents()
{
	const Camera* const currCamera = mEngine ->GetCurrentScene()-> GetCurrentCamera();

	UINT stride, offset;
	stride = sizeof(Vertex);
	offset = 0;

	// 1. Set Vertex, Index Buffer , shaders for all objects to draw
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->VSSetShader(mShaders->GetObjVS(), 0, 0);
	md3dImmediateContext->PSSetShader(mShaders->GetPSt0(), 0, 0);
	md3dImmediateContext->IASetInputLayout(mShaders->GetBasic32IL());

	// 2. Draw Every transparent Object
	float bf[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	md3dImmediateContext->OMSetBlendState(mEngine->GetBSTransparent(), bf, 0xffffffff);

	int size = (int)mTransparentObjs->size();
	int iTrans = 0;

	/* sort the transparent objects by depth */
	// update the depth
	for (; iTrans < size; iTrans++)
	{
		(*mTransparentObjs)[iTrans]->UpdateDepth(currCamera);
	}

	// sort the depth from close to far
	std::sort(mTransparentObjs->begin(), mTransparentObjs->end(), Object::CompareDepth);

	// draw from far to close
	for (iTrans = size - 1; iTrans >= 0; iTrans--)
	{

		(*mTransparentObjs)[iTrans]->DrawTransparent();
	}

	md3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
	md3dImmediateContext->OMSetBlendState(nullptr, bf, 0xffffffff);

}

void CarDemo::MapConstantBuffer()
{

	const Camera* const currCam = this->GetCurrentCamera();
	XMFLOAT3 camPos = currCam->GetPosition();
	D3D11_MAPPED_SUBRESOURCE mSource;

#pragma region   Upload PerFrame cBuffer VS
	// Update

	XMStoreFloat4x4(&mToVRAMPerFrame_VS.camPosTransform, XMMatrixTranspose(XMMatrixTranslation(camPos.x, camPos.y, camPos.z) * currCam->GetViewProjXM()));
	XMStoreFloat4x4(&mToVRAMPerFrame_VS.viewProj, XMMatrixTranspose(currCam->GetViewProjXM()));
	// mapping
	ZeroMemory(&mSource, sizeof(mSource));
	HR(md3dImmediateContext->Map(mCBPerFrame_VS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mSource));
	memcpy(mSource.pData, &mToVRAMPerFrame_VS, sizeof(mToVRAMPerFrame_VS));
	md3dImmediateContext->Unmap(mCBPerFrame_VS.Get(), 0);

	// Apply
	md3dImmediateContext->VSSetConstantBuffers(0, 1, mCBPerFrame_VS.GetAddressOf());

#pragma endregion

#pragma region  Upload PerFrame cBuffer GS

	mToVRAMPerFrame_GS.eyePosition = { camPos.x, camPos.y, camPos.z, 1.0f };
	XMStoreFloat4x4(&mToVRAMPerFrame_GS.viewProj, XMMatrixTranspose(currCam->GetViewProjXM()));

	ZeroMemory(&mSource, sizeof(mSource));
	HR(md3dImmediateContext->Map(mCBPerFrame_GS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mSource));
	memcpy(mSource.pData, &mToVRAMPerFrame_GS, sizeof(mToVRAMPerFrame_GS));
	md3dImmediateContext->Unmap(mCBPerFrame_GS.Get(), 0);

	// Apply
	md3dImmediateContext->GSSetConstantBuffers(0, 1, mCBPerFrame_GS.GetAddressOf());

#pragma endregion

#pragma region Upload PerFrame cBuffer PS

	mToVRAMPerFrame_PS.eyePosition = camPos;

	ZeroMemory(&mSource, sizeof(mSource));
	HR(md3dImmediateContext->Map(mCBPerFrame_PS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mSource));
	memcpy(mSource.pData, &mToVRAMPerFrame_PS, sizeof(mToVRAMPerFrame_PS));
	md3dImmediateContext->Unmap(mCBPerFrame_PS.Get(), 0);

	// Apply
	md3dImmediateContext->PSSetConstantBuffers(0, 1, mCBPerFrame_PS.GetAddressOf());

#pragma endregion
}

#pragma endregion