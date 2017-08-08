#include "Object.h"
#include "GeometryGenerator.h"
#include "SceneRenderApp.h"
#include "Effect.h"



Object::Object() :
	mVB(nullptr),
	mIB(nullptr),
	mMesh(nullptr),
	mMaterial(nullptr),
	mTexMap(nullptr)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mTexTransform, I);
	XMStoreFloat4x4(&mWorld, I);

	mForward = { mWorld(0,0), mWorld(0,1), mWorld(0,2) };
	mRight   = { mWorld(1,0), mWorld(1,1), mWorld(1,2) };
	mUp      = { mWorld(2,0), mWorld(2,1), mWorld(2,2) };
	mPos     = { mWorld(3,0), mWorld(3,1), mWorld(3,2) };
}


Object::~Object()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}

void Object::Edit(Mesh * _mesh, Material * _material, ID3D11ShaderResourceView * _mTexMap)
{
	mMesh = _mesh;
	mMaterial = _material;
	mTexMap = _mTexMap;
}

void Object::BuildVIBuffer(ID3D11Device* const _d3dDevice, D3D11_BUFFER_DESC& _vbd, D3D11_BUFFER_DESC& _ibd, D3D11_SUBRESOURCE_DATA& _initData)
{
	if (mMesh != nullptr)
	{

		// VB:
		ZeroMemory(&_vbd, sizeof(D3D11_BUFFER_DESC));
		_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		_vbd.ByteWidth = mMesh->vertices.size() * sizeof(Vertex);
		_vbd.Usage = D3D11_USAGE_IMMUTABLE;
		_vbd.CPUAccessFlags = 0;
		_vbd.MiscFlags = 0;
		_vbd.StructureByteStride = 0;

		ZeroMemory(&_initData, sizeof(D3D11_SUBRESOURCE_DATA));
		_initData.pSysMem = mMesh->vertices.data();

		HR(_d3dDevice->CreateBuffer(&_vbd, &_initData, &mVB));


		// IB:
		ZeroMemory(&_ibd, sizeof(D3D11_BUFFER_DESC));
		_ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		_ibd.ByteWidth = mMesh->indices.size() * sizeof(UINT);
		_ibd.Usage = D3D11_USAGE_IMMUTABLE;
		_ibd.CPUAccessFlags = 0;
		_ibd.MiscFlags = 0;
		_ibd.StructureByteStride = 0;

		ZeroMemory(&_initData, sizeof(_initData));
		_initData.pSysMem = mMesh->indices.data();

		HR(_d3dDevice->CreateBuffer(&_ibd, &_initData, &mIB));
	}
}

void Object::SetPosition(const XMFLOAT3 & _pos)
{
	mWorld(3, 0) = _pos.x;
	mWorld(3, 1) = _pos.y;
	mWorld(3, 2) = _pos.z;
}

void Object::Update(float _deltaTime)
{

}

void Object::Draw(ID3D11DeviceContext * const _d3dImmediateContext, UINT& _stride, UINT& _offset, CXMMATRIX _viewProj, UINT passIndex)
{

	if (mMesh != nullptr)
	{
		_d3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &_stride, &_offset);
		_d3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);



		XMMATRIX world = XMLoadFloat4x4(&mWorld);
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
		XMMATRIX worldViewProj = world * _viewProj;

		Effects::BaseFx->SetWorldMatrix(world);
		Effects::BaseFx->SetWorldInvTranspMatrix(worldInvTranspose);
		Effects::BaseFx->SetWVPMatrix(worldViewProj);
		Effects::BaseFx->SetTexTransform(XMLoadFloat4x4(&mTexTransform));
		Effects::BaseFx->SetMaterial(*mMaterial);
		
		ID3DX11EffectTechnique* _activeTech;

		if (mTexMap != nullptr)
		{
			Effects::BaseFx->SetTexResource(mTexMap);
			_activeTech = Effects::BaseFx->mLightTech;
		}
		else
		{
			_activeTech = Effects::BaseFx->mLightTechNoTex;
		}

		_activeTech->GetPassByIndex(passIndex)->Apply(0, _d3dImmediateContext);
		_d3dImmediateContext->DrawIndexed(mMesh->indices.size(), 0, 0);
	}
}




#pragma region Objects

Mesh*           Objects::testCube_mesh = nullptr;
Mesh*           Objects::car_mesh = nullptr;
Mesh*           Objects::defCube_mesh = nullptr;
Material*       Objects::def_material = nullptr;


ID3D11ShaderResourceView* Objects::car_texture = nullptr;



Objects::Objects()
{
	testCube_mesh = new Mesh();
	car_mesh      = new Mesh();
	defCube_mesh  = new Mesh();

	def_material = new Material();

	def_material->ambient = { 0.2f,0.2f,0.2f,1.0f };
	def_material->diffuse = { 1, 1, 1, 1 };
	def_material->specular = { 0, 0, 0, 16.0f };
}

Objects::~Objects()
{
	delete car_mesh;
	delete testCube_mesh;
	delete defCube_mesh;

	delete def_material;

	ReleaseCOM(car_texture);
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

Material * Objects::GetDefMaterial()
{
	return def_material;
}

ID3D11ShaderResourceView * Objects::GetCarTexture()
{
	return car_texture;
}


void Objects::LoadAssets(ID3D11Device* _device)
{

	Objects::LoadObjFile("../Models/cube.obj", testCube_mesh);
	Objects::LoadObjFile("../Models/car.obj", car_mesh);

	GeometryGenerator::CreateCube(1, 1, 1, defCube_mesh);


	HR(CreateWICTextureFromFile(_device, L"../Models/car.png", nullptr, &car_texture));
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
						_out->indices[outMeshIndices] = uniqueVertexIndices.size();
						tempVertecIndices[it].Index = uniqueVertexIndices.size();
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

			_out->vertices[i].nor
				= {-nor[(uniqueVertexIndices[i].normalIndex - 1)].x, -nor[(uniqueVertexIndices[i].normalIndex - 1)].y, -nor[(uniqueVertexIndices[i].normalIndex - 1)].z };

			_out->vertices[i].uv = uv[(uniqueVertexIndices[i].uvIndex - 1)];

			_out->vertices[i].uv.y = 1 - _out->vertices[i].uv.y;
		}

		//for (UINT i = 0; i < _out->vertices.size(); i+=3)
		//{
		//	_out->vertices[i].pos = pos[(uniqueVertexIndices[i].positionIndex - 1)];
		//	_out->vertices[i].nor = nor[(uniqueVertexIndices[i].normalIndex - 1)];
		//	_out->vertices[i].uv = uv[(uniqueVertexIndices[i+2].uvIndex - 1)];

		//	_out->vertices[i+1].pos = pos[(uniqueVertexIndices[i+1].positionIndex - 1)];
		//	_out->vertices[i+1].nor = nor[(uniqueVertexIndices[i+1].normalIndex - 1)];
		//	_out->vertices[i+1].uv   = uv[(uniqueVertexIndices[i+1].uvIndex - 1)];


		//	_out->vertices[i+2].pos = pos[(uniqueVertexIndices[i+2].positionIndex - 1)];
		//	_out->vertices[i+2].nor = nor[(uniqueVertexIndices[i+2].normalIndex - 1)];
		//	_out->vertices[i+2].uv   = uv[(uniqueVertexIndices[i].uvIndex - 1)];


		//	//_out->vertices[i].uv.x = _out->vertices[i].uv.x - (_out->vertices[i].uv.x - 0.5f) * 2 + .25f;
		//	//_out->vertices[i].uv.x = _out->vertices[i].uv.x  - 1;
		//	//_out->vertices[i].uv.y = _out->vertices[i].uv.y - .25f;
		//	_out->vertices[i].uv.y = 1 - _out->vertices[i].uv.y;
		//	_out->vertices[i+1].uv.y = 1 - _out->vertices[i+1].uv.y; 
		//	_out->vertices[i+2].uv.y = 1 - _out->vertices[i+2].uv.y;
		//	//_out->vertices[i].uv.y = _out->vertices[i].uv.y - (_out->vertices[i].uv.y - 0.5f) * 2;
		//	//_out->vertices[i].uv.y = 1 - _out->vertices[i].uv.y;
		//	//_out->vertices[i].uv.x = 1 - _out->vertices[i].uv.x + 0.25f;
		//}


#pragma endregion


		delete[] pos;
		delete[] nor;
		delete[] uv;
		return true;
	}
}

#pragma endregion

