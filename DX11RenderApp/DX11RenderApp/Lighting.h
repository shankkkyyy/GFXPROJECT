#pragma once
#include <windows.h>
#include <directXmath.h>
/* Lighting defination mirror to HLSL */


struct Material
{
	DirectX::XMFLOAT4 diffuseAlbedo;
	DirectX::XMFLOAT3 fresnelR0;
	float             shininess;
	DirectX::XMFLOAT4 reflection;
};

struct Light
{
	DirectX::XMFLOAT3 lightColor;
	float fallOffStart;
	DirectX::XMFLOAT3 direction;
	float fallOffend;
	DirectX::XMFLOAT3 position;
	float spotAngle; //48
};


