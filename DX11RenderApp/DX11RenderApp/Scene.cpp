#include "Scene.h"
#include "SkyBox.h"


Scene::Scene(HINSTANCE _hInstance) : 
	SceneRender(_hInstance) 
{
	mClientWidth = 1280;
	mClientHeight = 960;
	mMainWndCaption = L"Scene_0";	

	mShaders = new Shader();
	mObjects = new std::vector<Object>();
	mSkyBox = new SkyBox();
}

Scene::~Scene()
{
	delete mShaders;
	delete mObjects;
	delete mSkyBox;
	delete mOpaqueIndices;
	delete mTransparentIndices;
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
		return true;
	}
	else
	{
		return false;
	}
}

void Scene::ObjInitAndEdit()
{

#pragma region Scene Setup

	GetMainCamera()->SetPosition(XMFLOAT3(0, 5, -50));

	mObjects->resize(4);

	// Surface
	(*mObjects)[0].Edit(Objects::GetDefaultPlaneMesh(), nullptr, Objects::GetFloorTexture());
	(*mObjects)[0].SetPosition(XMFLOAT3(0, -0.01f, 10));
	(*mObjects)[0].SetScale(XMFLOAT3(200, 0.25f, 200));
	(*mObjects)[0].SetTexTransform(5, 5);

	// Car
	(*mObjects)[1].Edit(Objects::GetCarMesh(), nullptr, Objects::GetCarTexture());
	(*mObjects)[1].SetPosition(XMFLOAT3(0, 0, 15));

	// transparent cube
	(*mObjects)[2].Edit(Objects::GetDefaultCubeMesh(), nullptr, nullptr);
	(*mObjects)[2].SetPosition(XMFLOAT3(0, 5, -15));
	(*mObjects)[2].SetScale(XMFLOAT3(10, 10, 1));
	(*mObjects)[2].SetDiffuseColor(DirectX::Colors::Cyan, 0.3f);
	(*mObjects)[2].SetTransparent(true);

	(*mObjects)[3].Edit(Objects::GetDefaultCubeMesh(), nullptr, Objects::GetIceTexture());
	(*mObjects)[3].SetPosition(XMFLOAT3(-15 , 7.5f, 15));
	(*mObjects)[3].SetScale(XMFLOAT3(1, 15, 25));
	(*mObjects)[3].SetTexTransform(3, 2);
	(*mObjects)[3].SetDiffuseAlpha(0.5f);
	(*mObjects)[3].SetTransparent(true);



#pragma endregion

#pragma region  Grouping the obj by transparency
	bool IsTransparent = false;
	for (size_t iObj = 0; iObj < mObjects->size(); iObj++)
	{
		IsTransparent = (*mObjects)[iObj].IsTransparent();
		if (IsTransparent)
			mTransparentAmount++;
		else
			mOpagueAmount++;
	}

	mOpaqueIndices = new UINT[mOpagueAmount];
	mTransparentIndices = new UINT[mTransparentAmount];

	mOpagueAmount = 0;
	mTransparentAmount = 0;

	for (size_t iObj = 0; iObj < mObjects->size(); iObj++)
	{
		IsTransparent = (*mObjects)[iObj].IsTransparent();
		if (IsTransparent)
		{
			mTransparentIndices[mTransparentAmount] = (UINT)iObj;
			mTransparentAmount++;
		}
		else
		{
			mOpaqueIndices[mOpagueAmount] = (UINT)iObj;
			mOpagueAmount++;
		}
	}

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
	mToVRAMPerFrame_PS.light[1].position = { 0.0f, 10.0f, 40.0f };
	mToVRAMPerFrame_PS.light[1].fallOffStart = 0;
	mToVRAMPerFrame_PS.light[1].fallOffend = 40;

	// Spot light - 
	mToVRAMPerFrame_PS.light[2].lightColor = XMFLOAT3((const float*)&Colors::DeepPink);
	mToVRAMPerFrame_PS.light[2].direction = { -1, 0.3f, 1 };
	XMStoreFloat3(&mToVRAMPerFrame_PS.light[2].direction,
		XMVector3Normalize(MathHelper::XMFloat3ToXMVector(mToVRAMPerFrame_PS.light[2].direction)));
	mToVRAMPerFrame_PS.light[2].position = { 18.0f, 1.0f, -18.0f };
	mToVRAMPerFrame_PS.light[2].fallOffStart = 0;
	mToVRAMPerFrame_PS.light[2].fallOffend = 40;
	mToVRAMPerFrame_PS.light[2].spotAngle = 0.7f; //45

	XMFLOAT4 fogColor = XMFLOAT4((const float*)(&DirectX::Colors::WhiteSmoke));
	mToVRAMPerFrame_PS.fogColor = { fogColor.x, fogColor.y, fogColor.z };
	mToVRAMPerFrame_PS.fogStart = 15.0f;
	mToVRAMPerFrame_PS.fogRange = 50.0f;


#pragma endregion

}

void Scene::BuildGeometry()
{

	// for Obj
	UINT                   objVerticeSize = 0, objIndicesSize = 0;
	D3D11_BUFFER_DESC      vbd, ibd;
	D3D11_SUBRESOURCE_DATA initData;
	size_t i = 0;
	size_t vOffset = 0, iOffset = 0; 

#pragma region Gather all vertices and indices for scene objectes into to two separate array
	// Get total unique vertex size, and indices size 
	for (i = 0; i < mObjects->size(); i++)
	{
		objVerticeSize += (UINT) (*mObjects)[i].GetMesh()->vertices.size();
		objIndicesSize += (UINT)(*mObjects)[i].GetMesh()->indices.size();
	}

	// load all mesh into a vertex array
	// load all indices into an indices array

	UINT* objIndices = new UINT[objIndicesSize];
	Vertex* objVertices = new Vertex[objVerticeSize];

	for (i = 0; i < mObjects->size(); i++)
	{
		size_t vlength = (*mObjects)[i].GetMesh()->vertices.size();
		for (size_t iV = 0; iV < vlength; iV++)
		{
			objVertices[iV + vOffset] = (*mObjects)[i].GetMesh()->vertices[iV];
		}


		// reset length to indices size
		size_t ilength = (*mObjects)[i].GetMesh()->indices.size();
		for (size_t iI = 0; iI < ilength; iI++)
		{
			objIndices[iI + iOffset] = (*mObjects)[i].GetMesh()->indices[iI];
		}

		// get the Index && vertex offset for index draw within one buffer
		(*mObjects)[i].SortIndex((UINT)iOffset,(UINT)vOffset);

		// update offset
		vOffset += vlength;
		iOffset += ilength;
	}

#pragma endregion

#pragma region Create Buffer and push data into VB and IB
	// VB
	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = objVerticeSize * sizeof(Vertex);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	//_initData.pSysMem = mMeshToDraw->vertices.data();
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
#pragma endregion

	delete[] objVertices;
	delete[] objIndices;
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

}

void Scene::DrawScene()
{
	const Camera* const mainCamera = GetMainCamera();
	XMFLOAT3 camPos = mainCamera->GetPosition();

	// clear render view and depth view
	md3dImmediateContext->ClearRenderTargetView(md3dRTV, DirectX::Colors::Black);
	md3dImmediateContext->ClearDepthStencilView(md3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render setting
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->PSSetSamplers(0, 1, mSS4xAnisotropyWRAP.GetAddressOf());

	// update frame cbuffer for PS and VS
	D3D11_MAPPED_SUBRESOURCE mSource;

#pragma region   Upload PerFrame cBuffer VS
	// Update
	XMStoreFloat4x4(&mToVRAMPerFrame_VS.camPosTransform, XMMatrixTranspose(XMMatrixTranslation(camPos.x, camPos.y, camPos.z) * mainCamera->GetViewProj()));
	
	// mapping
	ZeroMemory(&mSource, sizeof(mSource));
	HR(md3dImmediateContext->Map(mCBPerFrame_VS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mSource));
	memcpy(mSource.pData, &mToVRAMPerFrame_VS, sizeof(mToVRAMPerFrame_VS));
	md3dImmediateContext->Unmap(mCBPerFrame_VS.Get(), 0);

	// Apply
	md3dImmediateContext->VSSetConstantBuffers(0, 1, mCBPerFrame_VS.GetAddressOf());

#pragma endregion

#pragma region Upload PerFrame cBuffer PS

	mToVRAMPerFrame_PS.eyePosition = GetMainCamera()->GetPosition();

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


	// 2. Draw Every Object
	UINT index = 0;
	for (size_t iOpa = 0; iOpa < mOpagueAmount; iOpa++)
	{
		index = mOpaqueIndices[iOpa];
		(*mObjects)[index].Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
	}

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


	// 1. draw mirror stencil buffer
	md3dImmediateContext->OMSetBlendState(mBSNoRenderTargetWrite.Get(), bf, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(mDSStencilMark.Get(), 0x01);

	(*mObjects)[3].Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);

	md3dImmediateContext->OMSetBlendState(nullptr, bf, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(mDSReflection.Get(), 0x01);
	md3dImmediateContext->RSSetState(mRSFrontCull.Get());

	// 2. draw mirrored car
	XMVECTOR mirrorPlane = XMVectorSet(1, 0, 0, 15);
	XMMATRIX prevWorldMatrix = (*mObjects)[1].GetWorldMatrixXM();
	(*mObjects)[1].SetWorldMatrix(prevWorldMatrix * XMMatrixReflect(mirrorPlane));
	(*mObjects)[1].Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
	(*mObjects)[1].SetWorldMatrix(prevWorldMatrix);

	md3dImmediateContext->OMSetDepthStencilState(nullptr, 0);

	// 3. draw mirror


	// 4. restore


#pragma endregion

	md3dImmediateContext->OMSetBlendState(mBSTransparent.Get(), bf, 0xffffffff);

	// Draw Front face first
	md3dImmediateContext->RSSetState(mRSFrontCull.Get());
	size_t iTrans = 0;
	UINT index = 0;
	for (; iTrans < mTransparentAmount; iTrans++)
	{
		index = mTransparentIndices[iTrans];
		(*mObjects)[index].Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
	}

	// Draw back face 
	md3dImmediateContext->RSSetState(nullptr);
	for (iTrans = 0; iTrans < mTransparentAmount; iTrans++)
	{
		index = mTransparentIndices[iTrans];
		(*mObjects)[index].Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);
	}



	// draw a car shadow
	md3dImmediateContext->PSSetShader(mShaders->GetObjShaderPS(), 0, 0);

	XMVECTOR shadowPlane = XMVectorSet(0, 1, 0, 0);
	XMVECTOR lightDirection = -XMLoadFloat3(&mToVRAMPerFrame_PS.light[0].direction);
	XMMATRIX shadowMatrix = XMMatrixShadow(shadowPlane, lightDirection);

	(*mObjects)[1].SetWorldMatrix(prevWorldMatrix * shadowMatrix);

	md3dImmediateContext->OMSetDepthStencilState(mDSNoDoubleBlend.Get(), 0);
	(*mObjects)[1].Draw(md3dImmediateContext, mCBPerObj_VS.Get(), mCBPerObj_PS.Get(), mainCamera);

	(*mObjects)[1].SetWorldMatrix(prevWorldMatrix);

	//restore blend state

	md3dImmediateContext->OMSetDepthStencilState(nullptr, 0);
	md3dImmediateContext->OMSetBlendState(nullptr, bf, 0xffffffff);
}
