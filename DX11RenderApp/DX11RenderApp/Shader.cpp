#include "Shader.h"
#include "Vertex.h"


Shader::Shader()
{
}

Shader::~Shader()
{
}

ID3D11VertexShader * Shader::GetObjVS() const
{
	return mobj_VS.Get();
}

ID3D11PixelShader * Shader::GetObjPS() const
{
	return mobj_PS.Get();
}

ID3D11PixelShader * Shader::GetObjShaderPS() const
{
	return mObjShadow_PS.Get();
}

ID3D11VertexShader * Shader::GetSkyVS() const
{
	return msky_VS.Get();
}

ID3D11PixelShader * Shader::GetSkyPS() const
{
	return msky_PS.Get();
}

ID3D11InputLayout * Shader::GetPosIL() const
{
	return mILpos.Get();
}

ID3D11InputLayout * Shader::GetBasic32IL() const
{
	return mILBasic32.Get();
}



void Shader::LoadShaders(ID3D11Device * _device)
{

	std::vector<char> byteCode;

	// Load Vertex shader for basic32 Vertex
	if (d3dHelper::LoadShaderByteCode(L"Shaders/VertexShader.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mobj_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDBasic32, _countof(InputLayoutDesc::IDBasic32), byteCode.data(), byteCode.size(), mILBasic32.GetAddressOf()));
	}

	// Load Pixel shader for basic32 Vertex
	if (d3dHelper::LoadShaderByteCode(L"Shaders/PixelShader.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mobj_PS.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/PSShadow.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mObjShadow_PS.GetAddressOf()));
	}

	// Load VS for SkyBox

	if (d3dHelper::LoadShaderByteCode(L"Shaders/SkyBoxVS.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, msky_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDPos, _countof(InputLayoutDesc::IDPos), byteCode.data(), byteCode.size(), mILpos.GetAddressOf()));
	}

	// Load PS for SkyBox

	if (d3dHelper::LoadShaderByteCode(L"Shaders/SkyBoxPS.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, msky_PS.GetAddressOf()))
	}

}

