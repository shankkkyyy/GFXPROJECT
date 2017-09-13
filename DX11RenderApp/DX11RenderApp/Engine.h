#pragma once

#include "SceneRenderApp.h"
#include "BaseScene.h"
#include "Shader.h"
#include "Object.h"


class Engine : public SceneRender
{
public:
	Engine(HINSTANCE _hInstance);
	~Engine();
private:

	static Engine* mEngine;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVB            = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mGeoVB         = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIB 		    = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mInstB         = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_VS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerObj_VS   = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_PS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerObj_PS   = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_GS = nullptr;

	Shader* mShaders = nullptr;
	Objects* mAssetsPool = nullptr;

	VSCBPerFrame mToVRAMPerFrame_VS;
	PSCBPerFrame mToVRAMPerFrame_PS;
	GSCBPerFrame mToVRAMPerFrame_GS;


	class SkyBox*          mSkyBox = nullptr;

	std::vector<Object*>*  mOpagueObjs      = nullptr;
	std::vector<Object*>*  mTransparentObjs = nullptr;
	std::vector<Object*>*  mInstances       = nullptr;
	std::vector<InstanceData>* mInstanceData = nullptr;


	

#pragma region Geometry Shader pratice

	UINT       mTreeAmount = 0;
	VertexBB*  mTrees = nullptr;
	PSCBPerObj mTreeToVRAM;

#pragma endregion


#pragma region ScreenQuad

	Mesh mScreenQuadGeo;
	UINT mScreenQuadIOffset, mScreenQuadVOffset;


#pragma endregion


	float pointSpeedY = 0;
	float spotSpeedY = 0;
	bool  bBlur = false;

	CountrySide* mCountrySideScene = nullptr;

public:

	bool InitApp() override;

public:

	static Engine* GetEngine();

	ID3D11Buffer* GetPSCBPerObj() const;
	ID3D11Buffer* GetVSCBPerObj() const;
	ID3D11Buffer* GetVSIBPerFrame() const;
	Shader*  GetShaders() const;

private:

	void LoadAndLinkResource();

	void CreateConstantBuffer();

	// assign mesh, texture and material
	void ObjInitAndEdit();

	void GetIndexAndVertexSize(UINT& totalVerticeSize, UINT& totalIndicesSize, std::vector<Object*>* _objList);

	void GetIndexAndVertexStartIndex(size_t& iOffset, size_t& vOffset, UINT* _indices, Vertex* _vertices, std::vector<Object*>* _objList);

	// build the geometry according to the objs to draw
	void BuildGeometry();

	void InitRenderSettings();

	void UpdateScene(float _deltaTime) override;

	void DrawOffscreen();

	void DrawRTT();

	void DrawScene() override;	

	void DrawOpaques();

	void DrawSkyBox();

	void DrawTransparents();

	void Input();

};



