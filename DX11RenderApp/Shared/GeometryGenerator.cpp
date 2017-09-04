#include "GeometryGenerator.h"
#include "Object.h"


GeometryGenerator::GeometryGenerator()
{
}


GeometryGenerator::~GeometryGenerator()
{
}

void GeometryGenerator::CreateGrid(float _width, float _depth, UINT _x, float _z, Mesh* _mesh)
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

void GeometryGenerator::CreateSphere(float radius, UINT sliceCount, UINT stackCount, Mesh * meshData)
{
	meshData->vertices.clear();
	meshData->indices.clear();


	Vertex topVertex;
	topVertex.pos = { 0.0f, +radius, 0.0f };
	topVertex.nor = { 0.0f, +1.0f, 0.0f };
	topVertex.uv  = { 0,0 };
	

	Vertex bottomVertex;
	bottomVertex.pos = { 0.0f, -radius, 0.0f };
	bottomVertex.nor = { 0.0f, -1.0f, 0.0f };
	bottomVertex.uv = { 0.0f, 1.0f };
	
	meshData->vertices.push_back(topVertex);

	float phiStep = XM_PI / stackCount;
	float thetaStep = 2.0f*XM_PI / sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (UINT i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			Vertex v;

			// spherical to cartesian
			v.pos.x = radius*sinf(phi)*cosf(theta);
			v.pos.y = radius*cosf(phi);
			v.pos.z = radius*sinf(phi)*sinf(theta);

			XMVECTOR p = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.nor, XMVector3Normalize(p));

			v.uv.x = theta / XM_2PI;
			v.uv.y = phi / XM_PI;

			meshData->vertices.push_back(v);
		}
	}

	meshData->vertices.push_back(bottomVertex);

	//
	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring.
	//

	for (UINT i = 1; i <= sliceCount; ++i)
	{
		meshData->indices.push_back(0);
		meshData->indices.push_back(i + 1);
		meshData->indices.push_back(i);
	}

	//
	// Compute indices for inner stacks (not connected to poles).
	//

	// Offset the indices to the index of the first vertex in the first ring.
	// This is just skipping the top pole vertex.
	UINT baseIndex = 1;
	UINT ringVertexCount = sliceCount + 1;
	for (UINT i = 0; i < stackCount - 2; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			meshData->indices.push_back(baseIndex + i*ringVertexCount + j);
			meshData->indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			meshData->indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

			meshData->indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
			meshData->indices.push_back(baseIndex + i*ringVertexCount + j + 1);
			meshData->indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
		}
	}

	//
	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring.
	//

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)meshData->vertices.size() - 1;

	// Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData->indices.push_back(southPoleIndex);
		meshData->indices.push_back(baseIndex + i);
		meshData->indices.push_back(baseIndex + i + 1);
	}			

}

void GeometryGenerator::CreatePlane(float _width, float _depth, Mesh * _mesh)
{
	_mesh->indices.clear();
	_mesh->vertices.clear();

	_mesh->indices.resize(6);
	_mesh->vertices.resize(4);

	float halfWidth = _width * 0.5f;
	float halfDepth = _depth * 0.5f;
	XMFLOAT3 normal = { 0, 1, 0 };

	(_mesh->vertices)[0] = { XMFLOAT3(-halfWidth, 0, halfDepth), normal, XMFLOAT2(0 ,0) };
	(_mesh->vertices)[1] = { XMFLOAT3(halfWidth, 0, halfDepth), normal, XMFLOAT2(1 ,0) };
	(_mesh->vertices)[2] = { XMFLOAT3(halfWidth, 0, -halfDepth), normal, XMFLOAT2(1 ,1) };
	(_mesh->vertices)[3] = { XMFLOAT3(-halfWidth, 0, -halfDepth), normal, XMFLOAT2(0 ,1) };



	(_mesh->indices)[0] = 0;    (_mesh->indices)[1] = 1;    (_mesh->indices)[2] = 2;
	(_mesh->indices)[3] = 0; 	(_mesh->indices)[4] = 2;	(_mesh->indices)[5] = 3;
}


