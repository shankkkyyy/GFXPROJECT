#pragma once

#include "d3dUtil.h"


class GeometryGenerator
{

public:

	struct MeshVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangentU;
		DirectX::XMFLOAT2 textureC;


		MeshVertex() {}
		MeshVertex(const DirectX::XMFLOAT3& _pos, 
			const DirectX::XMFLOAT3& _nor,
			const DirectX::XMFLOAT3& _tang,
			const DirectX::XMFLOAT2& _tc)
			: position(_pos), normal(_nor), tangentU(_tang), textureC(_tc) {}
		MeshVertex
		(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v
		) : position(px, py, pz), normal(nx, ny, nz), tangentU(tx, ty, tz), textureC(u, v)
		{}
	};

	struct MeshData
	{
		std::vector<MeshVertex> vertices;
		std::vector<UINT> indices;
	};

public:
	GeometryGenerator();
	~GeometryGenerator();

	static void CreateGird(float width, float depth, UINT hor, UINT vert, MeshData& _mesh);
};

