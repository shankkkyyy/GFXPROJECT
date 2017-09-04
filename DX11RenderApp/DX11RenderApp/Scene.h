#pragma once

#include "SceneRenderApp.h"
#include "Shader.h"
#include "Object.h"


class Scene : public SceneRender
{
public:
	Scene(HINSTANCE _hInstance);
	~Scene();
private:

	static Scene* mBaseScene;

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
	Objects mAssetsPool;

	VSCBPerFrame mToVRAMPerFrame_VS;
	PSCBPerFrame mToVRAMPerFrame_PS;
	GSCBPerFrame mToVRAMPerFrame_GS;

	std::vector<InstanceData>* mInstanceData = nullptr;

	class SkyBox*          mSkyBox = nullptr;

	std::vector<Object*>*  mOpagueObjs      = nullptr;
	std::vector<Object*>*  mTransparentObjs = nullptr;
	std::vector<Object*>*  mInstances       = nullptr;


#pragma region Geometry Shader pratice

	UINT mTreeAmount = 0;
	VertexBB* mTrees = nullptr;
	PSCBPerObj mTreeToVRAM;

#pragma endregion


	float pointSpeedY = 0;
	float spotSpeedY = 0;


public:

	bool InitApp() override;

public:

	static Scene* GetBaseScene();



	ID3D11Buffer* GetPSCB();
	//ID3D11Buffer* Get
	//ID3D11Buffer* Get
	//ID3D11Buffer* Get
	//ID3D11Buffer* Get
	//ID3D11Buffer* Get
	//ID3D11Buffer* Get




private:

	void CreateConstantBuffer();

	// assign mesh, texture and material
	void ObjInitAndEdit();


	void GetIndexAndVertexSize(UINT& objVerticeSize, UINT& objIndicesSize, std::vector<Object*>* _objList);

	void GetIndexAndVertexStartIndex(size_t& iOffset, size_t& vOffset, UINT* _indices, Vertex* _vertices, std::vector<Object*>* _objList);

	// build the geometry according to the objs to draw
	void BuildGeometry();

	void InitRenderSettings();

	void UpdateScene(float _deltaTime) override;

	void DrawScene() override;	

	void DrawOpaques();

	void DrawSkyBox();

	void DrawTransparents();
};



