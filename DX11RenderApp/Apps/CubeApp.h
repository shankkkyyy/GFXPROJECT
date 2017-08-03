#pragma once
#include "SceneRenderApp.h"

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
};

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
	ID3D11InputLayout* mInputLayout;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	DirectX::XMFLOAT4X4 mCubeWorld;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;

	float mYaw;
	float mPitch;
	float mRadius;

	

private:

	void BuildGeometryBuffer();

	void BuildFX();

	void BuildVertexLayout();

	// resize back and depth buffer and update the proj matrix
	void OnResize() override;

	void DrawScene() override;

	void UpdateScene(float _deltaTime) override;

};

