#pragma once

#include "SceneRenderApp.h"
#include "Object.h"


struct VSCBPerFrame
{
	XMFLOAT4X4 camWVP;
};

struct VSCBPerObj
{
	XMFLOAT4X4 wvp;
	XMFLOAT4X4 world;
	XMFLOAT4X4 worldInvTranspose;
	XMFLOAT4X4 texTransform;
};


struct PSCBPerFrame
{
	XMFLOAT3  eyePosition; //3
	float     pad;
	Light     light; //4
	XMFLOAT4  ambientLight;
};

struct PSCBPerObj
{
	Material material; // 4
};



class Scene : public SceneRender
{
public:
	Scene(HINSTANCE _hInstance);
	~Scene();

	bool InitApp() override;

private:


#pragma region  Objects
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	ID3D11Buffer* mCBVSPerFrame;
	ID3D11Buffer* mCBVSPerObj;
	ID3D11Buffer* mCBPSPerFrame;
	ID3D11Buffer* mCBPSPerObj;


	ID3D11VertexShader* mVS;
	ID3D11PixelShader*  mPS;

	ID3D11InputLayout* mIL;
	ID3D11SamplerState* mSS;

	Mesh* mMeshToDraw;

	Light mDirLight;

	UINT mIndexToDraw;

	Objects mAssetsPool;

	VSCBPerFrame mVSPerFrame;

	VSCBPerObj mVSPerObj;

	PSCBPerFrame mPSPerFrame;

	PSCBPerObj mPSPerObj;

	int speedDir = 1;;


#pragma endregion




#pragma region For Sky Box

	Microsoft::WRL::ComPtr<ID3D11Buffer> skyVB = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> skyIB = nullptr;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> skyVS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> skyPS = nullptr;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyMap = nullptr;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> skyIL = nullptr;

	UINT skyIndexSize = 0;

#pragma endregion



private:

	void BuildSkyBoxGeometry();

	void LoadSkyBoxShaderAndCreateInputLayout();

	void DrawSkyBox();

	void BuildGeometryBuffer();

	void CreateConstantBuffer();

	void CreateShaders();

	void CreateSampleState();

	void DrawScene() override;
	
};



