#include "Object.h"
#include "GeometryGenerator.h"
#include "Scene.h"




Object::Object() 
{
	XMMATRIX I = XMMatrixIdentity();

	// world
	XMStoreFloat4x4(&mWorldMatrix, I);

	// texTransform
	XMStoreFloat4x4(&mToVRAM_VS.texTransform, I);


	mToVRAM_PS.renderSetting = { 0, 0, 0, 0 };
}

Object::~Object()
{
}


void Object::SetPS(Scene* const _app)
{

	UINT texCount =(UINT) mTexMaps.size();
	ID3D11DeviceContext* devContext = _app->GetDeviceContext();
	switch (texCount)
	{
	case 0:
		devContext->PSSetShader(_app->GetShaders()->GetPSt0(), nullptr, 0);
		break;
	case 1:
		devContext->PSSetShader(_app->GetShaders()->GetPSd1(), nullptr, 0);
		break;
	case 2:
		devContext->PSSetShader(_app->GetShaders()->GetPSd2(), nullptr, 0);
		break;
	}
}

void Object::SetIndexOffset(UINT _indexOffset, UINT _vertexOffset)
{
	mIndexOffset = _indexOffset;
	mVertexOffset = _vertexOffset;
}

void Object::Edit(Mesh * const _mesh, const Material * const _material, ID3D11ShaderResourceView * const* _mTexMap, UINT _texCount)
{
	mMesh = _mesh;

	if (_material == nullptr)
		mToVRAM_PS.material = *Objects::GetDefMaterial();
	else
		mToVRAM_PS.material = *_material;

	if (_texCount == 0)
	{
		mToVRAM_PS.renderSetting.x = 0;
		mTexMaps.clear();
	}
	else
	{
		mTexMaps.resize(_texCount);
		memcpy(mTexMaps.data(), _mTexMap, sizeof(ID3D11ShaderResourceView*)*_texCount);
		mToVRAM_PS.renderSetting.x = 1;
	}
}

void Object::SetScale(const XMFLOAT3 & _xyz)
{
	XMMATRIX scale = XMMatrixScaling(_xyz.x, _xyz.y, _xyz.z);
	XMStoreFloat4x4(&mWorldMatrix, XMMatrixMultiply(scale, XMLoadFloat4x4(&mWorldMatrix)));
}

void Object::SetPosition(const XMFLOAT3 & _pos)
{
	mWorldMatrix(3, 0) = _pos.x;
	mWorldMatrix(3, 1) = _pos.y;
	mWorldMatrix(3, 2) = _pos.z;
}

void Object::SetTexTransform(float _x, float _y)
{
	mToVRAM_VS.texTransform(0, 0) = _x;
	mToVRAM_VS.texTransform(1, 1) = _y;
}

void Object::SetWorldMatrix(CXMMATRIX _matrix)
{
	XMStoreFloat4x4(&mWorldMatrix, _matrix);
}

void Object::SetName(const char* const _val)
{
	UINT size = _countof(mName);
	ZeroMemory(mName, size);
	strcpy_s(mName, size, _val);
}

void Object::SetInstanceCount(UINT _value)
{
	mInstanceAmount = _value;
}

XMMATRIX Object::GetWorldMatrixXM() const
{
	return XMLoadFloat4x4(&mWorldMatrix);
}

void Object::UpdateDepth(const Camera * const _cam)
{
	XMFLOAT4 camViewCol2 =
	{
		_cam->GetView()(0, 2),
		_cam->GetView()(1, 2),
		_cam->GetView()(2, 2),
		_cam->GetView()(3, 2)
	};

	mDepth =
		camViewCol2.x * mWorldMatrix(3, 0) +
		camViewCol2.y * mWorldMatrix(3, 1) +
		camViewCol2.z * mWorldMatrix(3, 2) +
		camViewCol2.w * mWorldMatrix(3, 3);
}

void Object::Rotate(float _pitch, float _yaw, float _roll)
{
	_pitch = XMConvertToRadians(_pitch);
	_yaw = XMConvertToRadians(_yaw);
	_roll = XMConvertToRadians(_roll);
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll);
	XMMATRIX world = XMLoadFloat4x4(&mWorldMatrix) * rot;
	XMStoreFloat4x4(&mWorldMatrix, world);
}

void Object::Update(float _deltaTime)
{

}

void Object::Draw()
{
	Scene* const app = Scene::GetBaseScene();
	ID3D11DeviceContext* devContext = app->GetDeviceContext();

	// set PS
	SetPS(app);

	// set texture
	devContext->PSSetShaderResources(0, (UINT)mTexMaps.size(), mTexMaps.data());

	D3D11_MAPPED_SUBRESOURCE mapResource;

#pragma region Upload cBuffer VS

	// set VS cbuffer
	XMMATRIX world = XMLoadFloat4x4(&mWorldMatrix);
	XMStoreFloat4x4(&mToVRAM_VS.world, XMMatrixTranspose(world));

	// wvp, must be update every frame
	XMMATRIX wvp = world * app->GetMainCamera()->GetViewProjXM();
	XMStoreFloat4x4(&mToVRAM_VS.wvp, XMMatrixTranspose(wvp));


	ID3D11Buffer* _VSCB = app->GetVSCBPerObj();
	ID3D11Buffer* _PSCB = app->GetPSCBPerObj();
	// map VS cbuffer
	ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	HR(app->GetDeviceContext()->Map(_VSCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));
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

void Object::Draw(ID3D11DeviceContext * devContext, ID3D11Buffer * _VSCB, ID3D11Buffer* _PSCB, const Camera* const _mCam)
{

	// set texture
	devContext->PSSetShaderResources(0, (UINT)mTexMaps.size(), mTexMaps.data());

	D3D11_MAPPED_SUBRESOURCE mapResource;

#pragma region Upload cBuffer VS

	// set VS cbuffer
	XMMATRIX world = XMLoadFloat4x4(&mWorldMatrix);

	// wvp, must be update every frame
	XMMATRIX wvp = world * _mCam->GetViewProjXM();
	XMStoreFloat4x4(&mToVRAM_VS.wvp, XMMatrixTranspose(wvp));

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

void Object::DrawInstance()
{

	Scene* const app = Scene::GetBaseScene();


	// set PS
	SetPS(app);

	// Set Texture
	app->GetDeviceContext()->PSSetShaderResources(0, (UINT)mTexMaps.size(), mTexMaps.data());
	// updating CB for Ps
	D3D11_MAPPED_SUBRESOURCE mapResource;
	ID3D11Buffer* PSCBPerObj = app->GetPSCBPerObj();
	ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HR(app->GetDeviceContext()->Map(PSCBPerObj, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource));
	memcpy(mapResource.pData, &mToVRAM_PS, sizeof(mToVRAM_PS));
	app->GetDeviceContext()->Unmap(PSCBPerObj, 0);

	app->GetDeviceContext()->PSSetConstantBuffers(1, 1, &PSCBPerObj);


	app->GetDeviceContext()->DrawIndexedInstanced(
		(UINT)mMesh->indices.size(), mInstanceAmount, mIndexOffset, mVertexOffset, 0);
}

bool Object::CompareDepth(const Object * const _left, const Object * const _right)
{
	if (_left->GetDepth() < _right->GetDepth())
		return true;
	else
		return false;
}

bool Object::IsTransparent() const
{
	return mToVRAM_PS.renderSetting.y == 1;
}

UINT Object::GetInstanceAmount() const
{
	return mInstanceAmount;
}

float Object::GetDepth() const
{
	return mDepth;
}

XMFLOAT4 Object::GetPosition() const
{
	return XMFLOAT4(mWorldMatrix(3, 0), mWorldMatrix(3, 1), mWorldMatrix(3, 2), mWorldMatrix(3, 3));
}

XMVECTOR Object::GetPositionXM() const
{
	XMVECTOR out = XMVectorSet(mWorldMatrix(3, 0), mWorldMatrix(3, 1), mWorldMatrix(3, 2), mWorldMatrix(3, 3));
	return out;
}

const Mesh* const Object::GetMesh() const
{
	return mMesh;
}

XMFLOAT4X4 Object::GetWorldMatrix() const
{
	return mWorldMatrix;
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
Mesh*           Objects::defSphere_mesh = nullptr;

Material*       Objects::def_material = nullptr;
Material*       Objects::silver_material = nullptr;

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::grass_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::car_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::sky_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::floor_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::wall_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::ice_texture = nullptr;
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Objects::treeArray_texture = nullptr;


Objects::Objects()
{
	testCube_mesh = new Mesh();
	car_mesh      = new Mesh();
	defCube_mesh  = new Mesh();
	defPlane_mesh = new Mesh();
	defSphere_mesh = new Mesh();

	def_material = new Material();
	def_material->diffuseAlbedo = { 1.0f,1.0f,1.0f,1.0f };
	def_material->fresnelR0 = { 0,0,0 };
	def_material->shininess = 0;

	silver_material = new Material();
	silver_material->diffuseAlbedo = XMFLOAT4(DirectX::Colors::Silver);
	silver_material->fresnelR0 = { 0.9f, 0.9f, 0.9f};
	silver_material->shininess = 8;


	
}

Objects::~Objects()
{
	delete car_mesh;
	delete testCube_mesh;
	delete defCube_mesh;
	delete defPlane_mesh;
	delete defSphere_mesh;
	
	delete silver_material;
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

Mesh * Objects::GetSphereMesh()
{
	return defSphere_mesh;
}

Material * Objects::GetSilverMaterial()
{
	return silver_material;
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

ID3D11ShaderResourceView * Objects::GetTreeArrayTexture()
{
	return treeArray_texture.Get();
}


void Objects::LoadAssets(ID3D11Device* _device)
{

	Objects::LoadObjFile("../Models/cube.obj", testCube_mesh);
	Objects::LoadObjFile("../Models/car.obj", car_mesh);

	GeometryGenerator::CreateCube(1, 1, 1, defCube_mesh);
	GeometryGenerator::CreatePlane(1, 1, defPlane_mesh);
	GeometryGenerator::CreateSphere(5, 16, 16,  defSphere_mesh);

	HR(CreateWICTextureFromFile(_device, L"../Models/car.png", nullptr,        car_texture  .GetAddressOf()));
	HR(CreateWICTextureFromFile(_device, L"../Models/grass.png", nullptr,      grass_texture.GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/sunsetcube.dds", nullptr, sky_texture  .GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/checkboard.dds", nullptr, floor_texture.GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/brick01.dds", nullptr,    wall_texture.GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/ice.dds", nullptr, ice_texture.GetAddressOf()));
	HR(CreateDDSTextureFromFile(_device, L"../Models/treearray.dds", nullptr, treeArray_texture.GetAddressOf()));


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

