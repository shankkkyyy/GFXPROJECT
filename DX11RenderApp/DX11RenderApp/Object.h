#pragma once
#include "d3dUtil.h"
#include "Vertex.h"


struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
};


class Object
{
public:
	Object();
	virtual ~Object();




public:
	XMFLOAT3 mPos;
	XMFLOAT3 mUp;
	XMFLOAT3 mRight;
	XMFLOAT3 mForward;

	XMFLOAT4X4 mTexTransform;

	XMFLOAT4X4 mWorld;

protected:

	Mesh* mMesh = nullptr;
	Material* mMaterial = nullptr;

private:
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	ID3D11ShaderResourceView* mTexMap;

public:

	void Edit(Mesh* _mesh, Material* _material, ID3D11ShaderResourceView* _mTexMap);

	void BuildVIBuffer(ID3D11Device* const _d3dDevice, D3D11_BUFFER_DESC& _vbd, D3D11_BUFFER_DESC& _ibd, D3D11_SUBRESOURCE_DATA& _initData);

	void SetPosition(const XMFLOAT3& _pos);

	void Update(float _deltaTime);

	void Draw(ID3D11DeviceContext* const _d3dImmediateContext, 
		UINT& _stride, UINT& _offset, CXMMATRIX _viewProj, UINT passIndex);



};

class Objects
{
public:
	Objects();
	~Objects();
	Objects(const Objects& _other) = delete;
	Objects& operator=(const Objects& _other) = delete;

	static Mesh* GetTestCubeMesh();
	static Mesh* GetCarMesh();
	static Mesh* GetDefaultCubeMesh();


	static Material* GetDefMaterial();

	static ID3D11ShaderResourceView* GetCarTexture();

private:
	static Mesh*                     testCube_mesh;
	static Mesh*                     car_mesh;
	static Mesh*                     defCube_mesh;

	static Material* def_material;

	static ID3D11ShaderResourceView* car_texture;


public:
	static bool LoadObjFile(const char* _path, Mesh* _out);
	static void LoadAssets(ID3D11Device* _device);
};

