#include "SceneRenderApp.h"


SceneRender::SceneRender(HINSTANCE _hInstance) : D3DApp(_hInstance)
{
	assert(mApp == nullptr);
	mApp = this;

	mMainCamera = new Camera();
	mDynamicCubeMapCam = new std::vector<Camera*>(6);
	for (size_t i = 0; i < mDynamicCubeMapCam->size(); i++)
		(*mDynamicCubeMapCam)[i] = new Camera();

}


SceneRender::~SceneRender()
{
	delete mMainCamera;

	for (size_t i = 0; i < mDynamicCubeMapCam->size(); i++)
		delete (*mDynamicCubeMapCam)[i];
	delete mDynamicCubeMapCam;

}

Camera * SceneRender::GetMainCamera() const
{	
	return mMainCamera;
}

 Camera** SceneRender::GetDynamicCubeMapCamera() const
{
	return mDynamicCubeMapCam->data();
}

 void SceneRender::SetDynamicCubeMapCamera(XMFLOAT3 _center)
 {
	 XMFLOAT3 forward[6] =
	 {
		 XMFLOAT3(+1,  0,  0), // +X
		 XMFLOAT3(-1,  0,  0), // -X
		 XMFLOAT3( 0, +1,  0), // +Y
		 XMFLOAT3( 0, -1,  0),  // -Y
		 XMFLOAT3( 0,  0, +1), // +Z
		 XMFLOAT3( 0,  0, -1), // -Z

	 };

	 XMFLOAT3 up[6] =
	 {
		 XMFLOAT3(0, 1, 0), // +X
		 XMFLOAT3(0, 1, 0), // -X
		 XMFLOAT3(0, 0, -1), // +Y
		 XMFLOAT3(0, 0, 1),  // -Y
		 XMFLOAT3(0, 1, 0), // +Z
		 XMFLOAT3(0, 1, 0), // -Z
	 };
	 
	 XMVECTOR F, U, R;
	 // derive the view matrix for cube map camera
	 for (size_t i = 0; i < 6; i++)
	 {
		 (*mDynamicCubeMapCam)[i]->SetPosition(_center);
		 
		 F = XMLoadFloat3(&forward[i]);
		 U = XMLoadFloat3(&up[i]);
		 R = XMVector3Cross(U, F);

		 (*mDynamicCubeMapCam)[i]->UpdateViewMatrix(F, R, U);
	 }

 }

void SceneRender::OnResize()
{
	D3DApp::OnResize();


	if (bIsFirstTimeOnResize)
	{
		for (size_t i = 0; i < mDynamicCubeMapCam->size(); i++)
		{
			(*mDynamicCubeMapCam)[i]->SetLen(0.5f*MathHelper::Pi, 1.0f, 0.3f, 200.0f);
			(*mDynamicCubeMapCam)[i]->OnResize();
		}
	}

	mMainCamera->SetLen(
		mMainCamera->GetFOV(), mApp->GetAspectRatio(),
		mMainCamera->GetNearPlane(), mMainCamera->GetFarPlane());
	mMainCamera->OnResize();

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

