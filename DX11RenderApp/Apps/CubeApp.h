#pragma once
#include "d3dApp.h"


struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
};

class CubeApp :
	public D3DApp
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

	POINT mMouseLastPos;

private:

	void BuildGeometryBuffer();

	void BuildFX();

	void BuildVertexLayout();

	// resize back and depth buffer and update the proj matrix
	void OnResize() override;

	void DrawScene() override;

	void UpdateScene() override;


	// Update x, y while mouse down
	void OnMouseDown(WPARAM _btnState, int x, int y) override;

	// Stop x, y while mouse up
	void OnMouseUp(WPARAM _btnState, int x, int y) override;

	// Up pitch and yaw for updating the view matrix
	void OnMouseMove(WPARAM _btnState, int x, int y) override;

};

