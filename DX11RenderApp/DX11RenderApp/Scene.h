#pragma once

#include "SceneRenderApp.h"
#include "Shader.h"
#include "Object.h"


class Scene : public SceneRender
{
public:
	Scene(HINSTANCE _hInstance);
	~Scene();

	bool InitApp() override;

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVB           = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIB 		   = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_VS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerObj_VS   = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_PS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerObj_PS   = nullptr;

	Shader* mShaders = nullptr;
	Objects mAssetsPool;

	VSCBPerFrame mToVRAMPerFrame_VS;
	PSCBPerFrame mToVRAMPerFrame_PS;

	class SkyBox*        mSkyBox = nullptr;

	std::vector<Object>* mObjects;

	UINT* mOpaqueIndices = nullptr;
	UINT* mTransparentIndices = nullptr;
	UINT mOpagueAmount = 0;
	UINT mTransparentAmount = 0;

	float pointSpeedY = 0;
	float spotSpeedY = 0;


private:

	void CreateConstantBuffer();

	// assign mesh, texture and material
	void ObjInitAndEdit();

	// build the geometry according to the objs to draw
	void BuildGeometry();


	void UpdateScene(float _deltaTime) override;

	void DrawScene() override;	

	void DrawOpaques();

	void DrawSkyBox();

	void DrawTransparents();

};



