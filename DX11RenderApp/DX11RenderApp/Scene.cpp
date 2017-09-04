#include "Scene.h"
#include "SkyBox.h"

Scene* Scene::mBaseScene = nullptr;

Scene::Scene(HINSTANCE _hInstance) : 
	SceneRender(_hInstance) 
{
	mClientWidth = 1280;
	mClientHeight = 960;
	mMainWndCaption = L"Scene_0";	

	mShaders = new Shader();
	mSkyBox = new SkyBox();
	bEnable4xMsaa = true;
	mBaseScene = this;
}

Scene::~Scene()
{
	delete mShaders;
	delete mSkyBox;

	size_t i = 0;
	for (; i < mOpagueObjs->size(); i++)
	{
		delete (*mOpagueObjs)[i];
	}
	delete mOpagueObjs;

	for (i = 0; i < mTransparentObjs->size(); i++)
	{
		delete (*mTransparentObjs)[i];
	}
	delete mTransparentObjs;

	for (i = 0; i < mInstances->size(); i++)
	{
		delete (*mInstances)[i];
	}
	delete mInstances;

	delete mInstanceData;

}

bool Scene::InitApp()
{
	if (SceneRender::InitApp())
	{

		// Load Models and texture
		Objects::LoadAssets(md3dDevice);

		// Load shaders and create input layout
		mShaders->LoadShaders(md3dDevice);

		// create constant buffer
		CreateConstantBuffer();

		// ObjEdit&Init
		ObjInitAndEdit();

		// build Geometry and push data into VRAM throught VB && IB buffer
		BuildGeometry();

		InitRenderSettings();


		return true;
	}
	else
	{
		return false;
	}
}

Scene * Scene::GetBaseScene()
{
	return mBaseScene;
}

ID3D11Buffer * Scene::GetPSCB()
{
	return mCBPerObj_PS.Get();
}

void Scene::ObjInitAndEdit()
{

#pragma region Scene Setup
	// set up the camera
	GetMainCamera()->SetPosition(XMFLOAT3(0, 5, -50));

	mOpagueObjs = new std::vector<Object*>();
	mTransparentObjs = new std::vector<Object*>();
	mInstances = new std::vector<Object*>();

	// Surface
	Object* surface = new Object();
	surface->SetName("surface");
	surface->Edit(Objects::GetDefaultPlaneMesh(), nullptr, Objects::GetFloorTexture());
	surface->SetPosition(XMFLOAT3(0, -0.01f, 10));
	surface->SetScale(XMFLOAT3(200, 0.25f, 200));
	surface->SetTexTransform(5, 5);
	mOpagueObjs->push_back(surface);

	// sphere
	Object* sphere = new Object();
	sphere->SetName("sphere");
	sphere->Edit(Objects::GetSphereMesh(), Objects::GetSilverMaterial(), nullptr);
	sphere->SetPosition(XMFLOAT3(-100, 10, 10));
	sphere->SetScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
	sphere->SetInstanceCount(10);
	mInstances->push_back(sphere);

	// Car
	Object* car = new Object();
	car->SetName("car");
	car->Edit(Objects::GetCarMesh(), nullptr, Objects::GetCarTexture());
	car->SetPosition(XMFLOAT3(0, 0, 5));
	car->Rotate(0, 90, 0);
	mOpagueObjs->push_back(car);


	// transparent cube
	Object* transparentCube = new Object();
	transparentCube->SetName("transparentCube");
	transparentCube->Edit(Objects::GetDefaultCubeMesh(), nullptr, nullptr);
	transparentCube->SetPosition(XMFLOAT3(0, 5, -15));
	transparentCube->SetScale(XMFLOAT3(10, 10, 1));
	transparentCube->SetDiffuseColor(DirectX::Colors::Cyan, 0.3f);
	transparentCube->SetTransparent(true);
	mTransparentObjs->push_back(transparentCube);

	Object* transparentSphere = new Object();
	transparentSphere->SetName("transparentSphere");
	transparentSphere->Edit(Objects::GetSphereMesh(), nullptr, nullptr);
	transparentSphere->SetPosition(XMFLOAT3(30, 10, 0));
	transparentSphere->SetDiffuseColor(DirectX::Colors::RoyalBlue, 0.3f);
	transparentSphere->SetTransparent(true);
	mTransparentObjs->push_back(transparentSphere);

	// mirror
	Object* mirror = new Object();
	mirror->SetName("mirror");
	mirror->Edit(Objects::GetDefaultCubeMesh(), nullptr, Objects::GetIceTexture());
	mirror->SetPosition(XMFLOAT3(-30 , 15, 0));
	mirror->SetScale(XMFLOAT3(1, 30, 50));
	mirror->SetTexTransform(3, 5);
	mirror->SetDiffuseAlpha(0.3f);
	mirror->SetTransparent(true);
	mTransparentObjs->push_back(mirror);


	// build instance data
	mInstanceData = new std::vector<InstanceData>(sphere->GetInstanceAmount());

	for (size_t i = 0; i < mInstanceData->size(); i++)
	{
		(*mInstanceData)[i].world = sphere->GetWorldMatrix();
		(*mInstanceData)[i].world(3, 0) += 20 * i;
		XMStoreFloat4x4(&(*mInstanceData)[i].world, 
			XMMatrixTranspose(XMLoadFloat4x4(&(*mInstanceData)[i].world)) );
	}


	mTreeAmount = 15;
	mTrees = new VertexBB[mTreeAmount];
	float treeSize = 20;

	for (size_t i = 0; i < mTreeAmount; i++)
	{
		mTrees[i].root.x = (MathHelper::RandF() - 0.5f) * 180;
		mTrees[i].root.y = 0;


		mTrees[i].root.z = (float)(rand() % 50 )+ 50;
		mTrees[i].size.x = MathHelper::Clamp(MathHelper::RandF(), 0.5f, 1.0f) * treeSize;
		mTrees[i].size.y = MathHelper::Clamp(MathHelper::RandF(), 0.7f, 1.3f) * treeSize;
	}

	mTreeToVRAM.material = *Objects::GetDefMaterial();
	mTreeToVRAM.renderSetting.x = 1;
	mTreeToVRAM.renderSetting.y = 0;

#pragma endregion

	// sky Box
	mSkyBox->
		Edit(mShaders->GetSkyVS(), mShaders->GetSkyPS(), Objects::GetSkyTexuture(), mShaders->GetPosIL(), md3dDevice);


#pragma region Objs Init
	
	// init lighting
	ZeroMemory(&mToVRAMPerFrame_VS, sizeof(VSCBPerFrame));
	mToVRAMPerFrame_PS.ambientLight = { 0.3f, 0.3f, 0.3f, 1.0f };

	// Dir light
	mToVRAMPerFrame_PS.light[0].lightColor = { .8f, .8f, .8f };
	mToVRAMPerFrame_PS.light[0].direction = { 1, -1, 1};
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

void Scene::GetIndexAndVertexSize(UINT & objVerticeSize, UINT & objIndicesSize, std::vector<Object*>* _objList)
{
	for (size_t i = 0; i < _objList->size(); i++)
	{
		objVerticeSize += (UINT)(*_objList)[i]->GetMesh()->vertices.size();
		objIndicesSize += (UINT)(*_objList)[i]->GetMesh()->indices.size();
	}
}

void Scene::GetIndexAndVertexStartIndex(size_t& iOffset, size_t& vOffset, UINT * _indices, Vertex * _vertices, std::vector<Object*>* _objList)
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

void Scene::BuildGeometry()
{

	// for Obj
	UINT                   objVerticeSize = 0, objIndicesSize = 0;
	D3D11_BUFFER_DESC      vbd, ibd, instbd;
	D3D11_SUBRESOURCE_DATA initData;
	size_t i = 0;
	size_t vOffset = 0, iOffset = 0; 

#pragma region Gather all vertices and indices for scene objectes into to two separate array

	GetIndexAndVertexSize(objVerticeSize, objIndicesSize, mOpagueObjs);
	GetIndexAndVertexSize(objVerticeSize, objIndicesSize, mInstances);
	GetIndexAndVertexSize(objVerticeSize, objIndicesSize, mTransparentObjs);

	// load all mesh into a vertex array
	// load all indices into an indices array

	UINT* objIndices = new UINT[objIndicesSize];
	Vertex* objVertices = new Vertex[objVerticeSize];

	GetIndexAndVertexStartIndex(iOffset, vOffset, objIndices, objVertices, mOpagueObjs);
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

	HR(md3dDevice->CreateBuffer(&vbd, &initData, mVB.GetAddressOf()));


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
	HR(md3dDevice->CreateBuffer(&ibd, &initData, mIB.GetAddressOf()));

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
	HR(md3dDevice->CreateBuffer(&instbd, &initData, mInstB.GetAddressOf()));
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
	delete[] mTrees;

}

void Scene::InitRenderSettings()
{
	ID3D11SamplerState* arr[2] = { mSSs[0].Get(), mSSs[1].Get() };
	md3dImmediateContext->PSSetSamplers(0, _countof(arr), arr);
}

void Scene::CreateConstantBuffer()
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

void Scene::UpdateScene(float _deltaTime)
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


	SceneRender::UpdateScene(_deltaTime);
}

void Scene::DrawScene()
{


	const Camera* const mainCamera = GetMainCamera();
	XMFLOAT3 camPos = mainCamera->GetPosition();

	// clear render view and depth view
	md3dImmediateContext->ClearRenderTargetView(md3dRTV, DirectX::Colors::Black);
	md3dImmediateContext->ClearDepthStencilView(md3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// update frame cbuffer for PS and VS
	D3D11_MAPPED_SUBRESOURCE mSource;

#pragma region   Upload PerFrame cBuffer VS
	// Update

	XMStoreFloat4x4(&mToVRAMPerFrame_VS.camPosTransform, XMMatrixTranspose(XMMatrixTranslation(camPos.x, camPos.y, camPos.z) * mainCamera->GetViewProjXM()));
	XMStoreFloat4x4(&mToVRAMPerFrame_VS.viewProj, XMMatrixTranspose(mainCamera->GetViewProjXM()));
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
	XMStoreFloat4x4(&mToVRAMPerFrame_GS.viewProj, XMMatrixTranspose(mainCamera->GetViewProjXM()));

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




	DrawOpaques();
	DrawSkyBox();
	DrawTransparents();


	HR(mSwapChain->Present(0, 0));
}

void Scene::DrawOpaques()
{

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const Camera* const mainCamera = GetMainCamera();

#pragma region Draws

	// 1. Set Vertex, Index Buffer , shaders for all objects to draw

	UINT stride = sizeof(Vertex); 
	UINT offset = 0;

	md3dImmediateContext->IASetVertexBuffers(0, 1, mVB.GetAddressOf(), &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIB.Get(), DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->VSSetShader(mShaders->GetObjVS(), 0, 0);
	md3dImmediateContext->PSSetShader(mShaders->GetObjPS(), 0, 0);
	md3dImmediateContext->IASetInputLayout(mShaders->GetBasic32IL());

	// 2. Draw Every Object
	for (size_t iOpa = 0; iOpa < mOpagueObjs->size(); iOpa++)
	{
		(*mOpagueObjs)[iOpa]->Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
	}

#pragma endregion

#pragma region Draw Instances

	UINT strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
	UINT offsets[2] = { 0, 0 };

	ID3D11Buffer* vbs[2] = { mVB.Get(), mInstB.Get() };

	md3dImmediateContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	md3dImmediateContext->IASetIndexBuffer(mIB.Get(), DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->VSSetShader(mShaders->GetInstVS(), 0, 0);
	md3dImmediateContext->PSSetShader(mShaders->GetObjPS(), 0, 0);
	md3dImmediateContext->IASetInputLayout(mShaders->GetBasic32ILInst());

	for (size_t iInst = 0; iInst < mInstances->size(); iInst++)
	{
		(*mInstances)[iInst]->DrawInstance();
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
	md3dImmediateContext->PSSetShaderResources(2, 1, &treeArrayTex);

	// Draw


	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	md3dImmediateContext->OMSetBlendState(mBSAlphaToCoverage.Get(), blendFactor, 0xffffffff);
	md3dImmediateContext->Draw(mTreeAmount, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);


	// restore topology
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->GSSetShader(nullptr, 0, 0);


#pragma endregion



}

void Scene::DrawSkyBox()
{
	// Change Rasterization State, draw Triangle inside out
	md3dImmediateContext->RSSetState(mRSFrontCull.Get());
	md3dImmediateContext->OMSetDepthStencilState(mDSLessEqual.Get(), 0);

	mSkyBox->Draw(md3dImmediateContext);

	// Restore Rasterization State
	md3dImmediateContext->RSSetState(nullptr);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
}

void Scene::DrawTransparents()
{
	const Camera* const mainCamera = GetMainCamera();

	UINT stride, offset;
	stride = sizeof(Vertex);
	offset = 0;

	// 1. Set Vertex, Index Buffer , shaders for all objects to draw
	md3dImmediateContext->IASetVertexBuffers(0, 1, mVB.GetAddressOf(), &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mIB.Get(), DXGI_FORMAT_R32_UINT, 0);
	md3dImmediateContext->VSSetShader(mShaders->GetObjVS(), 0, 0);
	md3dImmediateContext->PSSetShader(mShaders->GetObjPS(), 0, 0);
	md3dImmediateContext->IASetInputLayout(mShaders->GetBasic32IL());

	// 2. Draw Every transparent Object
	float bf[] = { 0.0f, 0.0f, 0.0f, 0.0f };

#pragma region Using Stencil
	//// 1. draw mirror stencil buffer
	//md3dImmediateContext->OMSetBlendState(mBSNoRenderTargetWrite.Get(), bf, 0xffffffff);
	//md3dImmediateContext->OMSetDepthStencilState(mDSStencilMark.Get(), 0x01);

	//(*mObjects)[3]. (md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);

	//md3dImmediateContext->OMSetBlendState(nullptr, bf, 0xffffffff);
	//md3dImmediateContext->OMSetDepthStencilState(mDSReflection.Get(), 0x01);

	//md3dImmediateContext->RSSetState(mRSFrontCull.Get());

	//// 2. draw mirrored car
	//XMVECTOR mirrorPlane = XMVectorSet(1, 0, 0, 30);
	//XMMATRIX prevWorldMatrix = (*mObjects)[1].GetWorldMatrixXM();
	//(*mOpagueObjs)[1]->SetWorldMatrix(prevWorldMatrix * XMMatrixReflect(mirrorPlane));
	//(*mOpagueObjs)[1]->Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
	//(*mOpagueObjs)[1]->SetWorldMatrix(prevWorldMatrix);

	//md3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
	////3. draw mirror


	////4. restore


#pragma endregion

	md3dImmediateContext->OMSetBlendState(mBSTransparent.Get(), bf, 0xffffffff);

	int size = (int)mTransparentObjs->size();
	int iTrans = 0;

	/* sort the transparent objects by depth */
	// update the depth
	for ( ; iTrans < size; iTrans++)
	{
		(*mTransparentObjs)[iTrans]->UpdateDepth(mainCamera);
	}

	// sort the depth from close to far
	std::sort(mTransparentObjs->begin(), mTransparentObjs->end(), Object::CompareDepth);

	// draw from far to close
	for (iTrans = size - 1; iTrans >= 0 ; iTrans--)
	{
		// Draw Front face first
		md3dImmediateContext->RSSetState(mRSFrontCull.Get());
		(*mTransparentObjs)[iTrans]->Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
		// Draw Back face
		md3dImmediateContext->RSSetState(nullptr);
		(*mTransparentObjs)[iTrans]->Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
	}

	// draw a car shadow
	//md3dImmediateContext->PSSetShader(mShaders->GetObjShaderPS(), 0, 0);
	//XMVECTOR shadowPlane = XMVectorSet(0, 1, 0, 0);
	//XMVECTOR lightDirection = -XMLoadFloat3(&mToVRAMPerFrame_PS.light[0].direction);
	//XMMATRIX shadowMatrix = XMMatrixShadow(shadowPlane, lightDirection);
	//(*mObjects)[1].SetWorldMatrix(prevWorldMatrix * shadowMatrix);
	//md3dImmediateContext->OMSetDepthStencilState(mDSNoDoubleBlend.Get(), 0);
	//(*mObjects)[1].Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
	//(*mObjects)[1].SetWorldMatrix(prevWorldMatrix);
	//restore blend state
	md3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
	md3dImmediateContext->OMSetBlendState(nullptr, bf, 0xffffffff);
}

