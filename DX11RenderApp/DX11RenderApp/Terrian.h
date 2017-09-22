#pragma once

#include "d3dUtil.h"




class Terrian
{
public:
	struct TerrianInfo
	{
		// assuming the size of height map is known
		float heightScale, cellSpacing;
		UINT heightMapWidth, heightMapDepth;
		std::wstring heightMap;
	};

public:
	Terrian();
	~Terrian();


	void Initialize(const TerrianInfo& _info);
	void ReleaseResource();
	void CreateGeometryBuffer();
	void Draw();

private:

	TerrianInfo mInfo;

	UINT       mNumCtrlPtsRows, mNumCtrlPtsCols, mTotalCtrlPts, mTotalQuadFaces;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mQuadCtrlPtsBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mQuadIndicesBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mHeightSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mBlendMapSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mLayersMapSRV;

	
	std::vector<struct VertexTerrian>* mCtrlPts = nullptr;
	std::vector<UINT>*  mIndices = nullptr;

	DirectX::XMFLOAT4X4 mWorldMatrix;

public:
	
	const std::vector<struct VertexTerrian>* const GetCtrlPoints()  const;
	const std::vector<UINT>* const                 GetIndices()     const;

	ID3D11ShaderResourceView*     GetHeightSRV() const;
	ID3D11ShaderResourceView*     GetLayerSRV() const;
	ID3D11ShaderResourceView*     GetBlendSRV() const;

private:

	void LoadHeightMap();
	void LoadTexture();

};

