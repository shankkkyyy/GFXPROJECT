#include "GeometryGenerator.h"


GeometryGenerator::GeometryGenerator()
{
}


GeometryGenerator::~GeometryGenerator()
{
}

void GeometryGenerator::CreateGird(float width, float depth, UINT hor, UINT vert, MeshData & _mesh)
{

	UINT verticesCount = hor * vert;
	UINT triCount = 2 * (hor - 1) * (vert - 1);
	_mesh.vertices.resize(verticesCount);

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float du = 1.0f / hor;
	float dv = 1.0f / vert;
	float dx = width * du;
	float dz = depth * dv;




	float x, z;
	UINT currentVertex;
	for (UINT i = 0; i < hor; i++)
	{
		x = -halfWidth + dx * i;
		for (UINT j = 0; j < vert; j++)
		{
		    z = halfDepth - dz * j;
			
			currentVertex = hor * j + i;
			_mesh.vertices[currentVertex].position = XMFLOAT3(x, 0.0f, z);
			_mesh.vertices[currentVertex].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			_mesh.vertices[currentVertex].tangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);
			_mesh.vertices[currentVertex].textureC = XMFLOAT2(i * du, j * dv);
		}
	}

	_mesh.indices.resize(triCount * 3);
	UINT index = 0;
	for (UINT i = 0; i < hor - 1; i++)
	{
		for (UINT j = 0; j < vert - 1; j++)
		{
			currentVertex = j * hor + i;
			_mesh.indices[index] = currentVertex;
			_mesh.indices[index + 1] = currentVertex + 1;
			_mesh.indices[index + 2] = currentVertex + hor;

			_mesh.indices[index + 3] = currentVertex + hor;
			_mesh.indices[index + 4] = currentVertex + 1;
			_mesh.indices[index + 5] = currentVertex + hor + 1;

			index += 6;
		}
	}
}
