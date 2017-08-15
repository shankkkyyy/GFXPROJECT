#include "GeometryGenerator.h"
#include "Object.h"


GeometryGenerator::GeometryGenerator()
{
}


GeometryGenerator::~GeometryGenerator()
{
}

void GeometryGenerator::CreateCube(float _width, float _height, float _depth, Mesh* _mesh)
{
	_mesh->indices.clear();
	_mesh->vertices.clear();

	_mesh->vertices.resize(24);
	_mesh->indices.resize(36);

	//
	// Create the vertices.
	//


	float w2 = 0.5f*_width;
	float h2 = 0.5f*_height;
	float d2 = 0.5f*_depth;

	// Fill in the front face vertex data.
	_mesh->vertices[0] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) };
	_mesh->vertices[1] = { XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) };
	_mesh->vertices[2] = { XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) };
	_mesh->vertices[3] = { XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) };

	// Fill in the back face vertex data.
	_mesh->vertices[4] = { XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }; 
	_mesh->vertices[5] = { XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }; 
	_mesh->vertices[6] = { XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }; 
	_mesh->vertices[7] = { XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }; 

	// Fill in the top face vertex data.
	_mesh->vertices[8]  = { XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f,  1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
	_mesh->vertices[9]  = { XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f,  1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }; 
	_mesh->vertices[10] = { XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f,  1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
	_mesh->vertices[11] = { XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f,  1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };

	// Fill in the bottom face vertex data.


	_mesh->vertices[12] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };
	_mesh->vertices[13] = { XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
	_mesh->vertices[14] = { XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };
	_mesh->vertices[15] = { XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };


	// Fill in the left face vertex data.

	_mesh->vertices[16] = { XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
	_mesh->vertices[17] = { XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };
	_mesh->vertices[18] = { XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
	_mesh->vertices[19] = { XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };

	// Fill in the right face vertex data.

	_mesh->vertices[20] = { XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) };
	_mesh->vertices[21] = { XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) };
	_mesh->vertices[22] = { XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) };
	_mesh->vertices[23] = { XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) };


	//
	// Create the indices.
	//


	// Fill in the front face index data
	_mesh->indices[0] = 0; _mesh->indices[1] = 1; _mesh->indices[2] = 2;
	_mesh->indices[3] = 0; _mesh->indices[4] = 2; _mesh->indices[5] = 3;

	// Fill in the back face index data
	_mesh->indices[6] = 4; _mesh->indices[7] = 5;  _mesh->indices[8] = 6;
	_mesh->indices[9] = 4; _mesh->indices[10] = 6; _mesh->indices[11] = 7;

	// Fill in the top face index data
	_mesh->indices[12] = 8; _mesh->indices[13] = 9;  _mesh->indices[14] = 10;
	_mesh->indices[15] = 8; _mesh->indices[16] = 10; _mesh->indices[17] = 11;

	// Fill in the bottom face index data
	_mesh->indices[18] = 12; _mesh->indices[19] = 13; _mesh->indices[20] = 14;
	_mesh->indices[21] = 12; _mesh->indices[22] = 14; _mesh->indices[23] = 15;

	// Fill in the left face index data
	_mesh->indices[24] = 16; _mesh->indices[25] = 17; _mesh->indices[26] = 18;
	_mesh->indices[27] = 16; _mesh->indices[28] = 18; _mesh->indices[29] = 19;

	// Fill in the right face index data
	_mesh->indices[30] = 20; _mesh->indices[31] = 21; _mesh->indices[32] = 22;
	_mesh->indices[33] = 20; _mesh->indices[34] = 22; _mesh->indices[35] = 23;


}

void GeometryGenerator::CreateSphere(float _radius, UINT _stack, Mesh * _mesh)
{

	_mesh->vertices.clear();
	_mesh->indices.clear();

	float deltaRad_y = DirectX::XM_PI / _stack;
	float deltaRad_xz = 2 * deltaRad_y;

	UINT vertexPerStack = _stack + 1;

	// Generate vertex
	float x, y, z;
	Vertex newVertex;
	for (UINT iy = 0; iy < vertexPerStack; iy++)
	{
		y = XMScalarCos(deltaRad_y * iy) * _radius;
		float xzRadius = XMScalarSin(deltaRad_y * iy) * _radius;
		for (UINT ixz = 0; ixz < vertexPerStack; ixz++)
		{
			x = XMScalarSin(deltaRad_xz * ixz);
			z = XMScalarCos(deltaRad_xz * ixz);
			newVertex.pos = { x, y, z };
			_mesh->vertices.push_back(newVertex);
		}
	}

	// Generate Index
	for (UINT iy = 0; iy < _stack; iy++)
	{
		for (UINT ixz = 0; ixz < _stack; ixz++)
		{

			UINT current = iy * vertexPerStack + ixz;
			_mesh->indices.push_back(current);
			_mesh->indices.push_back(current + 1);
			_mesh->indices.push_back(current + vertexPerStack);

			_mesh->indices.push_back(current + vertexPerStack);
			_mesh->indices.push_back(current + 1);
			_mesh->indices.push_back(current + vertexPerStack + 1);
		}
	}
	
}


