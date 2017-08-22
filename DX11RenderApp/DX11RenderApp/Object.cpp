#include "Object.h"
#include "GeometryGenerator.h"
#include "SceneRenderApp.h"
#include "Camera.h"



Object::Object() 
{
	XMMATRIX I = XMMatrixIdentity();

	// world
	XMStoreFloat4x4(&mToVRAM_VS.world, I);

	// worldInvTranspose
	XMVECTOR det = XMMatrixDeterminant(I);
	XMStoreFloat4x4(&mToVRAM_VS.worldInvTranspose, XMMatrixInverse(&det, I));

	// texTransform
	XMStoreFloat4x4(&mToVRAM_VS.texTransform, I);

	mToVRAM_PS.renderSetting = { 0, 0, 0, 0 };
}

Object::~Object()
{
}

void Object::Edit(Mesh * _mesh, Material* _material, ID3D11ShaderResourceView * _mTexMap)
{
	mMesh     = _mesh;

	if (_material == nullptr)
		mToVRAM_PS.material = *Objects::GetDefMaterial();
	else
		mToVRAM_PS.material = *_material;

	if (_mTexMap != nullptr)
	{
		mTexMap = _mTexMap;
		mToVRAM_PS.renderSetting.x = 1;
	}
	else
	{
		mToVRAM_PS.renderSetting.x = 0;
	}
}



void Object::SortIndex(UINT _indexOffset, UINT _vertexOffset)
{
	mIndexOffset = _indexOffset;
	mVertexOffset = _vertexOffset;
}

void Object::SetScale(const XMFLOAT3 & _xyz)
{
	XMMATRIX scale = XMMatrixScaling(_xyz.x, _xyz.y, _xyz.z);
	XMStoreFloat4x4(&mToVRAM_VS.world, XMMatrixMultiply(scale, XMLoadFloat4x4(&mToVRAM_VS.world)));
	bWorldMatrixIsChanged = true;
}

void Object::SetPosition(const XMFLOAT3 & _pos)
{
	mToVRAM_VS.world(3, 0) = _pos.x;
	mToVRAM_VS.world(3, 1) = _pos.y;
	mToVRAM_VS.world(3, 2) = _pos.z;
	bWorldMatrixIsChanged = true;
}

void Object::SetTexTransform(float _x, float _y)
{
	mToVRAM_VS.texTransform(0, 0) = _x;
	mToVRAM_VS.texTransform(1, 1) = _y;
}

void Object::SetWorldMatrix(CXMMATRIX _matrix)
{
	XMStoreFloat4x4(&mToVRAM_VS.world, _matrix);
	bWorldMatrixIsChanged = true;
}

XMMATRIX Object::GetWorldMatrixXM() const
{
	return XMLoadFloat4x4(&mToVRAM_VS.world);
}

void Object::Update(float _deltaTime)
{

}

void Object::Draw(ID3D11DeviceContext * devContext, ID3D11Buffer * _VSCB, ID3D11Buffer* _PSCB, const Camera* const _mCam)
{
	// set texture
	devContext->PSSetShaderResources(0, 1, &mTexMap);

	D3D11_MAPPED_SUBRESOURCE mapResource;

#pragma region Upload cBuffer VS

	// set VS cbuffer
	XMMATRIX world = XMLoadFloat4x4(&mToVRAM_VS.world);

	// wvp, must be update every frame
	XMMATRIX wvp = world * _mCam->GetViewProj();
	XMStoreFloat4x4(&mToVRAM_VS.wvp, XMMatrixTranspose(wvp));


	if (bWorldMatrixIsChanged)
	{
		XMMATRIX world = XMLoadFloat4x4(&mToVRAM_VS.world);
		world.r[3] = XMVectorSet(0, 0, 0, 1);
		// worldInvTranspose
		XMVECTOR det = XMMatrixDeterminant(world);
 		XMStoreFloat4x4(&mToVRAM_VS.worldInvTranspose, XMMatrixInverse(&det, world));
		bWorldMatrixIsChanged = false;
	}

	// map VS cbuffer
	ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HR(devContext->Map(_VSCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));
	memcpy(mapResource.pData, &mToVRAM_VS, sizeof(mToVRAM_VS));
	devContext->Unmap(_VSCB, 0);

	devContext->VSSetConstantBuffers(1, 1, &_VSCB);

	
	ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HR(devContext->Map(_PSCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));
	memcpy(mapResource.pData, &mToVRAM_PS, sizeof(mToVRAM_PS));
	devContext->Unmap(_PSCB, 0);

	devContext->PSSetConstantBuffers(1, 1, &_PSCB);
#pragma endregion
		
	devContext->DrawIndexed((UINT)mMesh->indices.size(), mIndexOffset, mVertexOffset);
}

bool Object::IsTransparent() const
{
	return mToVRAM_PS.renderSetting.y == 1;
}

const Mesh* const Object::GetMesh() const
{
	return mMesh;
}

void Object::SetDiffuseColor(const FLOAT* _color, float _alpha)
{
	mToVRAM_PS.material.diffuseAlbedo.x = _color[0];
	mToVRAM_PS.material.diffuseAlbedo.y = _color[1];
	mToVRAM_PS.material.diffuseAlbedo.z = _color[2];
	mToVRAM_PS.material.diffuseAlbedo.w = _alpha;
}

void Object::SetDiffuseAlpha(float _val)
{
	mToVRAM_PS.material.diffuseAlbedo.w = MathHelper::Clamp(_val, 0.0f, 1.0f);
}

void Object::SetTransparent(bool _val)
{
	mToVRAM_PS.renderSetting.y = _val;
}





#pragma region Objects

Mesh*           Objects::testCube_mesh = nullptr;
Mesh*           Objects::car_mesh = nullptr;
Mesh*           Objects::defCube_mesh = nullptr;
Mesh*           Objects::defPlane_mesh = nullptr;

Material*       Objects::def_material = nullptr;


Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::grass_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::car_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::sky_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::floor_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::wall_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::ice_texture = nullptr;


Objects::Objects()
{
	testCube_mesh = new Mesh();
	car_mesh      = new Mesh();
	defCube_mesh  = new Mesh();
	defPlane_mesh = new Mesh();

	def_material = new Material();
	def_material->diffuseAlbedo = { 0.5f,0.5f,0.5f,1.0f };
	def_material->fresnelR0 = { 0,0,0 };
	def_material->shininess = 0;
}

Objects::~Objects()
{
	delete car_mesh;
	delete testCube_mesh;
	delete defCube_mesh;
	delete defPlane_mesh;

	delete def_material;
}

Mesh * Objects::GetTestCubeMesh()
{
	return testCube_mesh;
}

Mesh * Objects::GetCarMesh()
{
	return car_mesh;
}

Mesh * Objects::GetDefaultCubeMesh()
{
	return defCube_mesh;
}

Mesh * Objects::GetDefaultPlaneMesh()
{
	return defPlane_mesh;
}

Material * Objects::GetDefMaterial()
{
	return def_material;
}

ID3D11ShaderResourceView * Objects::GetCarTexture()
{
	return car_texture.Get();
}

ID3D11ShaderResourceView * Objects::GetGrassTexture()
{
	return grass_texture.Get();
}

ID3D11ShaderResourceView * Objects::GetSkyTexuture()
{
	return sky_texture.Get();
}

ID3D11ShaderResourceView * Objects::GetFloorTexture()
{
	return floor_texture.Get();
}

ID3D11ShaderResourceView * Objects::GetWallTexture()
{
	return wall_texture.Get();
}

ID3D11ShaderResourceView * Objects::GetIceTexture()
{
	return ice_texture.Get();
}


void Objects::LoadAssets(ID3D11Device* _device)
{

	Objects::LoadObjFile("../Models/cube.obj", testCube_mesh);
	Objects::LoadObjFile("../Models/car.obj", car_mesh);

	GeometryGenerator::CreateCube(1, 1, 1, defCube_mesh);
	GeometryGenerator::CreatePlane(1, 1, defPlane_mesh);

	HR(CreateWICTextureFromFile(_device, L"../Models/car.png", nullptr,        car_texture  .GetAddressOf()));
	HR(CreateWICTextureFromFile(_device, L"../Models/grass.png", nullptr,      grass_texture.GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/sunsetcube.dds", nullptr, sky_texture  .GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/checkboard.dds", nullptr, floor_texture.GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/brick01.dds", nullptr,    wall_texture.GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/ice.dds", nullptr, ice_texture.GetAddressOf()));


}

bool Objects::LoadObjFile(const char * _path, Mesh* _out)
{

	FILE* file;
	fopen_s(&file, _path, "r");
	if (file == nullptr)
	{
		std::wstring sdf = L"Can not open file at: " + TextHelper::ToString(_path);
		MessageBox(nullptr, sdf.c_str(), 0, 0);
		return false;
	}
	else
	{

#pragma region Go throught the file, and get the size of geometry data to read, and create temp memory

		UINT ip = 0;    // count of position
		UINT iuv = 0; // count of uv
		UINT in = 0; // count of normal
		UINT ii = 0; // count of indices

		while (true)
		{
			char lineHeader[128];
			int read = fscanf_s(file, "%s", lineHeader, 128);
			if (read == EOF)
			{
				break;
			}
			else if (strcmp(lineHeader, "v") == 0)
			{
				ip++;
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				iuv++;
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				in++;
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				ii++;
			}

		}

		ii = ii * 3;

		XMFLOAT3* pos = new XMFLOAT3[ip];
		XMFLOAT3* nor = new XMFLOAT3[in];
		XMFLOAT2* uv = new XMFLOAT2[iuv];

#pragma endregion

#pragma region Go throught the file again and read the data

		UINT outMeshIndices = 0;
		_out->indices.clear();
		_out->indices.resize(ii);

		// works as index from now
		ip = 0;
		in = 0;
		iuv = 0;

		rewind(file);


		unsigned(*hFunc)(const OBJVertexIndices&) = [](const OBJVertexIndices& a) {return (a.normalIndex + a.positionIndex + a.uvIndex) % 1000; };

		HTable<OBJVertexIndices> uniqueTable(1000, hFunc);

		std::vector<OBJVertexIndices> uniqueVertexIndices;

		OBJVertexIndices tempVertecIndices[3];

		while (true)
		{
			char lineHeader[128];
			int read = fscanf_s(file, "%s", lineHeader, 128);
			if (read == EOF)
			{
				break;
			}
			else if (strcmp(lineHeader, "v") == 0)
			{
				fscanf_s(file, "%f %f %fn", &(pos[ip].x), &(pos[ip].y), &(pos[ip].z));
				ip++;
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				fscanf_s(file, "%f %fn", &(uv[iuv].x), &(uv[iuv].y));
				iuv++;
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				fscanf_s(file, "%f %f %fn", &(nor[in].x), &(nor[in].y), &(nor[in].z));
				in++;
			}
			else if (strcmp(lineHeader, "f") == 0)
			{

				fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%dn",
					&(tempVertecIndices[2].positionIndex), &(tempVertecIndices[2].uvIndex), &(tempVertecIndices[2].normalIndex),
					&(tempVertecIndices[1].positionIndex), &(tempVertecIndices[1].uvIndex), &(tempVertecIndices[1].normalIndex),
					&(tempVertecIndices[0].positionIndex), &(tempVertecIndices[0].uvIndex), &(tempVertecIndices[0].normalIndex));

				const OBJVertexIndices* dupicate;
#pragma region Check if it is unique vertex, and then add to the unique vertex list, and assemble the indices array

				for (int it = 0; it <3; it++)
				{
					dupicate = uniqueTable.Find(tempVertecIndices[it]);
					if (dupicate != nullptr)
					{
						// it is not unique, put the duplicate index in our indices
						_out->indices[outMeshIndices] = dupicate->Index;
						outMeshIndices++;
					}
					else
					{
						// it is unique
						_out->indices[outMeshIndices] = (UINT)uniqueVertexIndices.size();
						tempVertecIndices[it].Index = (UINT)uniqueVertexIndices.size();
						uniqueVertexIndices.push_back(tempVertecIndices[it]);
						uniqueTable.Insert(tempVertecIndices[it]);
						outMeshIndices++;
					}
				}
#pragma endregion

			}
		}
#pragma endregion

#pragma region Build Our Vertex According to Unique vertex list

		_out->vertices.clear();
		_out->vertices.resize(uniqueVertexIndices.size());

		for (UINT i = 0; i < _out->vertices.size(); i++)
		{
			_out->vertices[i].pos = pos[(uniqueVertexIndices[i].positionIndex - 1)];
			_out->vertices[i].pos.z = -_out->vertices[i].pos.z;

			_out->vertices[i].nor = nor[(uniqueVertexIndices[i].normalIndex - 1)];
			_out->vertices[i].nor.z = -_out->vertices[i].nor.z;

			_out->vertices[i].uv = uv[(uniqueVertexIndices[i].uvIndex - 1)];

			_out->vertices[i].uv.y = 1 - _out->vertices[i].uv.y;
		}

#pragma endregion


		delete[] pos;
		delete[] nor;
		delete[] uv;
		return true;
	}
}

#pragma endregion

