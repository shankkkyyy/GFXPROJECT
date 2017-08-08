#pragma once
#include <windows.h>
#include <directXmath.h>
/* Lighting defination mirror to HLSL */

struct Material
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	Material() { ZeroMemory(this, sizeof(Material)); }
};

struct DirectionalLight
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	DirectX::XMFLOAT3 direction;
	float padding;

	DirectionalLight() { ZeroMemory(this, sizeof(DirectionalLight)); }
};

struct PointLight
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	DirectX::XMFLOAT3 position;
	float range;

	DirectX::XMFLOAT3 attenuation;
	float padding;

	PointLight() { ZeroMemory(this, sizeof(PointLight)); }
};

struct SpotLight
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	DirectX::XMFLOAT3 position;
	float range;

	DirectX::XMFLOAT3 direction;
	float spot;

	DirectX::XMFLOAT3 attenuation;
	float padding;

	SpotLight() { ZeroMemory(this, sizeof(SpotLight)); }


};