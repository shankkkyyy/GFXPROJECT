#pragma once

#include "d3dUtil.h"



class GeometryGenerator
{

public:

	GeometryGenerator();

	~GeometryGenerator();

	static void CreateGrid(float _width, float _depth, UINT _x, float _z,struct Mesh* _mesh);

	static void CreateCube(float _width, float _height, float _depth, struct Mesh* _mesh);

	static void CreateSphere(float _radius, UINT _stack, struct Mesh* _mesh);

	static void CreatePlane(float _width, float _depth, struct Mesh* _mesh);
	
};

