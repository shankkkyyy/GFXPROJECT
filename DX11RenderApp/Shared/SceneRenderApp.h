#pragma once

#include "d3dApp.h"
#include "Camera.h"



// The Base Scene Render with D3DApp handles windows message loop and DX11 setup, and contain embedded camera
class SceneRender :
	public D3DApp
{
public:

	SceneRender(HINSTANCE _hInstance);

	virtual ~SceneRender();

public:

	Camera*                   GetMainCamera() const;

protected:

	virtual void OnResize();

	virtual void UpdateScene(float _deltaTime) override;

	virtual void OnMouseMove() override;

	virtual void OnMouseWheel() override;

	virtual void OnKeyDown() override;

private:

	Camera*   mMainCamera;
};

