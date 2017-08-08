#pragma once
#include "SceneRenderApp.h"
#include "Object.h"
#include "Effect.h"



class CubeApp :
	public SceneRender
{
public:
	CubeApp(HINSTANCE _hInstance);
	~CubeApp();

	bool InitApp() override;

private:

	ID3D11Buffer* mCubeVB;
	ID3D11Buffer* mCubeIB;
	ID3D11ShaderResourceView *mTexMap;

	UINT indicesAmount;


	SpotLight        mSpotLight;
	PointLight       mPointLight;
	DirectionalLight mDirLight;
	
	Material         mMaterial;
	Objects          mObjects;


	Object*   Car;
	Object*   Plane;

private:

	void BuildGeometryBuffer();

	// resize back and depth buffer and update the proj matrix
	void OnResize() override;

	void DrawScene() override;

	void UpdateScene(float _deltaTime) override;

};

