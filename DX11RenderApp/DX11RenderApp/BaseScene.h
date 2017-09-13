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

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer = nullptr;

public:
	void InitScene();
	virtual void Update(float _deltaTime) = 0;
	virtual void Render() = 0;
	virtual void CleanScene() = 0;
	virtual void InitialRenderSettings() = 0;

protected:


	virtual void EditScene() = 0;
	virtual void CreateGeometryBuffer() = 0;
	virtual void CreateConstantBuffer() = 0;


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
	void Render() override;
	void CleanScene() override;
	void InitialRenderSettings() override;

private:

	void EditScene() override;
	void CreateGeometryBuffer() override;
	void CreateConstantBuffer() override; 

private:

	HSCBPerFrame mHSCBPerFrame;
	DSCBPerFrame mDSCBPerFrame;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBHSPerFrame;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBDSPerFrame;

};


class CarDemo : public BaseScene
{
public:
	CarDemo();

	~CarDemo();

	void Update(float _deltaTime);
	void Render() override;
	void CleanScene() override;

private:

	void EditScene() override;
	void CreateGeometryBuffer() override;
	void CreateConstantBuffer() override;


private:

	void DrawOpaques();
	
	void DrawSkyBox();

	void DrawTransparents();

private:

	Microsoft::WRL::ComPtr<ID3D11Buffer> mGeoVB = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mInstB = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_VS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerObj_VS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_PS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerObj_PS = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mCBPerFrame_GS = nullptr;

	VSCBPerFrame mToVRAMPerFrame_VS;
	PSCBPerFrame mToVRAMPerFrame_PS;
	GSCBPerFrame mToVRAMPerFrame_GS;

	std::vector<class Object*>*      mOpagueObjs = nullptr;
	std::vector<class Object*>*      mTransparentObjs = nullptr;
	std::vector<class Object*>*      mInstances = nullptr;
	std::vector<InstanceData>*       mInstanceData = nullptr;


	class SkyBox*          mSkyBox = nullptr;


#pragma region Geometry Shader pratice

	UINT              mTreeAmount = 0;
	struct VertexBB*  mTrees = nullptr;
	PSCBPerObj        mTreeToVRAM;

#pragma endregion

	float pointSpeedY = 0;
	float spotSpeedY = 0;
};

