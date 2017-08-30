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

protected:

	bool bWorldMatrixIsChanged = false;
	UINT mIndexOffset = 0;
	UINT mVertexOffset = 0;
	UINT mInstanceAmount = 0;
	float mDepth = 0;
	char mName[32] = "Object";

	Mesh*                     mMesh = nullptr;
	ID3D11ShaderResourceView* mTexMap = nullptr;

	PSCBPerObj mToVRAM_PS;
	VSCBPerObj mToVRAM_VS;

public:

	void SetIndexOffset(UINT _indexOffset, UINT _vertexOffset);

	void Edit(Mesh* _mesh, Material* _material, ID3D11ShaderResourceView* _mTexMap);

	void Update(float _deltaTime);

	

	void Draw(ID3D11DeviceContext* devContext, ID3D11Buffer* _VSCB, ID3D11Buffer* _PSCB, const class Camera* const _mCam);

	void OpagueDraw();

	// increment sort result
	static bool CompareDepth(const Object* const _left, const Object* const _right);



public:

	bool IsTransparent() const;



	void SetDiffuseColor(const FLOAT* _color, float _alpha = 1.0f);
	void SetDiffuseAlpha(float _val);
	void SetTransparent(bool _val);
	void SetScale(const XMFLOAT3& _xyz);
	void SetPosition(const XMFLOAT3 & _pos);
	void SetTexTransform(float _x, float _y);
	void SetWorldMatrix(CXMMATRIX _matrix);
	void SetName(const char* const _val);
	


	float GetDepth() const;
	XMFLOAT4 GetPosition() const;
	XMVECTOR GetPositionXM() const;
	const Mesh* const GetMesh() const;
	XMMATRIX GetWorldMatrixXM() const;

	void UpdateDepth(const class Camera* const _cam);

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
	static Mesh* GetDefaultPlaneMesh();


	static Material* GetDefMaterial();

	static ID3D11ShaderResourceView* GetCarTexture();
	static ID3D11ShaderResourceView* GetGrassTexture();
	static ID3D11ShaderResourceView* GetSkyTexuture();
	static ID3D11ShaderResourceView* GetFloorTexture();
	static ID3D11ShaderResourceView* GetWallTexture();
	static ID3D11ShaderResourceView* GetIceTexture();
	static ID3D11ShaderResourceView* GetTreeArrayTexture();


private:

	static Mesh*                     testCube_mesh;
	static Mesh*                     car_mesh;
	static Mesh*                     defCube_mesh;
	static Mesh*                     defPlane_mesh;

	static Material* def_material;

	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> car_texture;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grass_texture;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sky_texture;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floor_texture;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> wall_texture;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ice_texture;
	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> treeArray_texture;


public:
	static bool LoadObjFile(const char* _path, Mesh* _out);
	static void LoadAssets(ID3D11Device* _device);
};

