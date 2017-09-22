#include "Engine.h"
#include "SkyBox.h"

Engine* Engine::mEngine = nullptr;

Engine::Engine(HINSTANCE _hInstance) : 
	SceneRender(_hInstance) 
{
	mClientWidth = 1280;
	mClientHeight = 960;
	mMainWndCaption = L"DX11RenderEngine";	
	bEnable4xMsaa = false;
	mShaders = new Shader();
	mAssetsPool = new Objects();
	mEngine = this;
}

Engine::~Engine()
{
	delete mShaders;
	delete mAssetsPool;
	
	for (size_t i = 0; i < mAllScenes->size(); i++)
	{
		delete (*mAllScenes)[i];
	}
	delete mAllScenes;
}

bool Engine::InitApp()
{
	if (SceneRender::InitApp())
	{

		LoadAndLinkResource();

		InitRenderSettings();

		// //Load Scenes

		size_t numOfScene = 2;
		mCarDemo = new CarDemo();
		mCountrySideScene = new CountrySide();

		mAllScenes = new std::vector<BaseScene*>(numOfScene);

		(*mAllScenes)[0] = mCountrySideScene;
		(*mAllScenes)[1] = mCarDemo;

		for (size_t i = 0; i < numOfScene; i++)
		{
			(*mAllScenes)[i]->EditScene();
		}
		(*mAllScenes)[mCurrentScene]->InitScene();
		return true;
	}
	else
	{
		return false;
	}
}

void Engine::AddBlur()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	// horizontal Blur
	md3dImmediateContext->CSSetShaderResources(9, 1, mOffscreenSRV.GetAddressOf());
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 1, mHorBluredUAV.GetAddressOf(), nullptr);

	md3dImmediateContext->CSSetShader(mShaders->GetBlurHorCS(), nullptr, 0);

	UINT threadsGroupCountX = (UINT)ceil(mClientWidth * divBy256);
	md3dImmediateContext->Dispatch(threadsGroupCountX, mClientHeight, 1);

	md3dImmediateContext->CSSetShaderResources(9, 1, &nullSRV);
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	//// vert blur
	md3dImmediateContext->CSSetShaderResources(9, 1, mHorBluredSRV.GetAddressOf());
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 1, mOffscreenUAV.GetAddressOf(), nullptr);

	md3dImmediateContext->CSSetShader(mShaders->GetBlurVerCS(), nullptr, 0);

	UINT threadsGroupCountY = (UINT)ceil(mClientHeight * divBy256);
	md3dImmediateContext->Dispatch(mClientWidth, threadsGroupCountY, 1);

	md3dImmediateContext->CSSetShaderResources(9, 1, &nullSRV);
	md3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}

Engine * Engine::GetEngine()
{
	return mEngine;
}
BaseScene * Engine::GetCurrentScene() const
{
	return (*mAllScenes)[mCurrentScene];
}

ID3D11Buffer * Engine::GetScreenVertexBuffer() const
{
	return mScreenVertexBuffer.Get();
}

bool Engine::GetIsBlurOn() const
{
	return bBlur;
}

Shader * Engine::GetShaders() const
{
	return mShaders;
}


void Engine::CreateScreenBuffer()
{

	Mesh* screenQuad = new Mesh();
	GeometryGenerator::CreateSreenQuad(screenQuad);

	D3D11_SUBRESOURCE_DATA initData;
	D3D11_BUFFER_DESC vbd;

	ZeroMemory(&vbd, sizeof(vbd));
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = 4 * sizeof(Vertex);
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;

	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = screenQuad->vertices.data();
	HR(md3dDevice->CreateBuffer(&vbd, &initData, mScreenVertexBuffer.GetAddressOf()));

	delete screenQuad;

}

void Engine::InitRenderSettings()
{
	ID3D11SamplerState* arr[2] = { mSSANISOTROPICWRAP.Get(), mSSANISOTROPICCLAMP.Get() };
	md3dImmediateContext->PSSetSamplers(0, _countof(arr), arr);

	// Build Screen Vertex
	CreateScreenBuffer();

}

void Engine::LoadAndLinkResource()
{
	// Load Models and texture
	Objects::LoadAssets(md3dDevice);

	// Load shaders and create input layout
	mShaders->LoadShaders(md3dDevice);

	//Link the resources to the scene code base
	BaseScene::LinkResource(this);

}


void Engine::UpdateScene(float _deltaTime)
{

	GetCurrentScene()->Update(_deltaTime);

	SceneRender::UpdateScene(_deltaTime);

	OverallInput();
}

void Engine::DrawScene()
{

	GetCurrentScene()->Render();

	HR(mSwapChain->Present(0, 0));
}

void Engine::OverallInput()
{
	if (GetAsyncKeyState(KEY_B) & 0x8000)
	{
		if (!bBlur)
		{
			bBlur = true;
		}
	}
	else if (GetAsyncKeyState(VK_HOME) & 0x8000)
	{
		bBlur = false;
	}
	else if (GetAsyncKeyState(KEY_1) & 0x8000)
	{
		if (mCurrentScene != 0)
		{
			GetCurrentScene()->CleanScene();
			mCurrentScene = 0;
			GetCurrentScene()->InitScene();
		}
	}
	else if (GetAsyncKeyState(KEY_2) & 0x8000)
	{
		if (mCurrentScene != 1)
		{
			GetCurrentScene()->CleanScene();
			mCurrentScene = 1;
			GetCurrentScene()->InitScene();
		}
	}

}


