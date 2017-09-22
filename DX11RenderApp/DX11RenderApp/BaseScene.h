#pragma once
#include "d3dUtil.h"

class BaseScene
{

public:
	BaseScene();
	BaseScene(const BaseScene& _other) = delete;
	BaseScene& operator= (const BaseScene& _other) = delete;
	virtual ~BaseScene();


public:

	static void LinkResource(class Engine* _engine);

protected:

	static class Shader* mShaders;
	static class Engine* mEngine;

	static ID3D11Device*             md3dDevice;
	static ID3D11DeviceContext*      md3dImmediateContext;
	
	class Camera* mCurrentCam = nullptr;


	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer       = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer        = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mInstanceBuffer     = nullptr;

	// const buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_VS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerObj_VS   = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_HS  = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_DS  = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_GS = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_PS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerObj_PS   = nullptr;

public:

	void InitScene();
	void Render();

	virtual void EditScene() = 0;

	virtual void Update(float _deltaTime) = 0;
	virtual void CleanScene();
	virtual void InitialRenderSettings() = 0;


	ID3D11Buffer* GetPSCBPerObj()     const;
	ID3D11Buffer* GetVSCBPerObj()     const;
	ID3D11Buffer* GetInstanceBuffer() const;
	class Camera* GetCurrentCamera() const;


protected:


	virtual void CreateGeometryBuffer() = 0;
	virtual void CreateConstantBuffer() = 0;

	virtual void RenderOffScreen();
	virtual void RenderToScreen();


	void GetIndexAndVertexSize(UINT& totalVerticeSize, UINT& totalIndicesSize, std::vector<class Object*>* _objList);
	void GetIndexAndVertexStartIndex(size_t& iOffset, size_t& vOffset, UINT* _indices, struct Vertex* _vertices, std::vector<class Object*>* _objList);
};




struct HSCBPerFrame
{
	XMFLOAT3 eyePos;
	float    pad;
};

struct DSCBPerFrame
{
	XMFLOAT4X4 worldViewProj;
};

class CountrySide : public BaseScene
{
public:
	CountrySide();
	~CountrySide();

	void Update(float _deltaTime) override;
	void CleanScene() override;
	void InitialRenderSettings() override;
	void EditScene() override;

private:

	void CreateGeometryBuffer() override;
	void CreateConstantBuffer() override; 

	void RenderOffScreen() override;
	void RenderToScreen() override;

	void SwitchToWireFrame();
	void SwitchToNormal();

private:

	HSCBPerFrame mHSCBPerFrame;
	DSCBPerFrame mDSCBPerFrame;

	class Terrian* mTerrian = nullptr;
};

class CarDemo : public BaseScene
{
public:
	CarDemo();

	~CarDemo();

	void Update(float _deltaTime);
	void CleanScene() override;
	void InitialRenderSettings() override;
	void EditScene() override;

private:

	void RenderOffScreen() override;
	void RenderToScreen() override;
	void CreateGeometryBuffer() override;
	void CreateConstantBuffer() override;


private:

	void DrawOpaques(bool _drawReflection);
	
	void DrawSkyBox();

	void DrawTransparents();

	void MapConstantBuffer();


private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> mGeoVB = nullptr;

	VSCBPerFrame mToVRAMPerFrame_VS;
	PSCBPerFrame mToVRAMPerFrame_PS;
	GSCBPerFrame mToVRAMPerFrame_GS;

	std::vector<class Object*>*      mReflectionObjs = nullptr;
	std::vector<class Object*>*      mOpagueObjs = nullptr;
	std::vector<class Object*>*      mTransparentObjs = nullptr;
	std::vector<class Object*>*      mInstances = nullptr;
	std::vector<InstanceData>*       mInstanceData = nullptr;


	class SkyBox*          mSkyBox = nullptr;

	// Dynamic Cube mapping

	UINT              mTreeAmount = 0;
	struct VertexBB*  mTrees = nullptr;
	PSCBPerObj        mTreeToVRAM;

	float pointSpeedY = 0;
	float spotSpeedY = 0;

};

