#pragma once

#include "d3dUtil.h"




class Terrian
{
public:
	struct TerrianInfo
	{
		std::wstring heightMapFileName;
		float heightScale;
		UINT heightMapWidth, heightMapDepth;
		float cellSpacing;
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


	std::vector<struct VertexTerrian>* mCtrlPts = nullptr;
	std::vector<UINT>*  mIndices = nullptr;

	DirectX::XMFLOAT4X4 mWorldMatrix;

public:
	
	const std::vector<struct VertexTerrian>* const GetCtrlPoints()  const;
	const std::vector<UINT>* const                 GetIndices()     const;


};

