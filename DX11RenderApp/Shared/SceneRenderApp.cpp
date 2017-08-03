#include "SceneRenderApp.h"



SceneRender::SceneRender(HINSTANCE _hInstance) : D3DApp(_hInstance)
{

	assert(mApp == nullptr);
	mApp = this;
	mMainCamera = new Camera();
}

SceneRender::~SceneRender()
{
	delete mMainCamera;

}

Camera * SceneRender::GetMainCamera() const
{
	
	return mMainCamera;
}

void SceneRender::OnResize()
{
	D3DApp::OnResize();

	mMainCamera->GetNewPerspective();
}

void SceneRender::UpdateScene(float _deltaTime)
{
	mMainCamera->Update(_deltaTime);
}

void SceneRender::OnMouseMove()
{
	mMainCamera->OnMouseMove();
	D3DApp::OnMouseMove();
}

void SceneRender::OnMouseWheel()
{
	mMainCamera->OnMouseWheel();
}

void SceneRender::OnKeyDown()
{
	mMainCamera->OnKeyDown();
}

