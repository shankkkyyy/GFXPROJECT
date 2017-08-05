#pragma once
#include "d3dUtil.h"

struct IVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 nor;
	DirectX::XMFLOAT2 uv;

	IVertex() {}
	IVertex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _nor, const DirectX::XMFLOAT2& _uv) :
		pos(_pos), nor(_nor), uv(_uv)
	{}

};


struct Mesh
{
	std::vector<IVertex> vertices;
	std::vector<UINT> indices;
};


class Object
{
public:
	Object();
	~Object();

public:
	static void LoadObjFile(const char* _path, Mesh& _out);
};

