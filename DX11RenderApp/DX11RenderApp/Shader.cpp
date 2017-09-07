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

ID3D11VertexShader * Shader::GetInstVS() const
{
	return mInst_VS.Get();
}

ID3D11VertexShader * Shader::GetRTTVS() const
{
	return mRTT_VS.Get();
}

ID3D11PixelShader * Shader::GetPSt0() const
{
	return mobj_PSt0.Get();
}

ID3D11PixelShader * Shader::GetPSd1() const
{
	return mobj_PSd1.Get();
}

ID3D11PixelShader * Shader::GetPSd2() const
{
	return mobj_PSd2.Get();
}

ID3D11PixelShader * Shader::GetRTTPSRed() const
{
	return mRTT_PSRed.Get();
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

ID3D11VertexShader * Shader::GetBBVS() const
{
	return mBB_VS.Get();
}

ID3D11GeometryShader * Shader::GetBBGS() const
{
	return mBB_GS.Get();
}

ID3D11PixelShader * Shader::GetBBPS() const
{
	return mBB_PS.Get();
}

ID3D11InputLayout * Shader::GetPosIL() const
{
	return mILpos.Get();
}

ID3D11InputLayout * Shader::GetBasic32ILInst() const
{
	return mILBasic32Inst.Get();
}

ID3D11InputLayout * Shader::GetBasic32IL() const
{
	return mILBasic32.Get();
}

ID3D11InputLayout * Shader::GetPosSize() const
{
	return mILposSize.Get();
}



void Shader::LoadShaders(ID3D11Device * _device)
{

	std::vector<char> byteCode;

#pragma region Load shaders for basic32 Vertex

	if (d3dHelper::LoadShaderByteCode(L"Shaders/VertexShader.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mobj_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDBasic32, _countof(InputLayoutDesc::IDBasic32), byteCode.data(), byteCode.size(), mILBasic32.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/VSInstance.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mInst_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDBasic32Inst, _countof(InputLayoutDesc::IDBasic32Inst), byteCode.data(), byteCode.size(), mILBasic32Inst.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/VSRTT.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mRTT_VS.GetAddressOf()));

	}



	// Load Pixel shader for basic32 Vertex
	if (d3dHelper::LoadShaderByteCode(L"Shaders/PSt0.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mobj_PSt0.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/PSd1.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mobj_PSd1.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/PSd2.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mobj_PSd2.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/PSShadow.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mObjShadow_PS.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/PSRTT_red.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mRTT_PSRed.GetAddressOf()));
	}


#pragma endregion

#pragma region Load shaders for SkyBox

	if (d3dHelper::LoadShaderByteCode(L"Shaders/SkyBoxVS.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, msky_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDPos, _countof(InputLayoutDesc::IDPos), byteCode.data(), byteCode.size(), mILpos.GetAddressOf()));
	}
	// Load PS for SkyBox
	if (d3dHelper::LoadShaderByteCode(L"Shaders/SkyBoxPS.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, msky_PS.GetAddressOf()));
	}
#pragma endregion

#pragma region Load Shaders for Billboard

	if (d3dHelper::LoadShaderByteCode(L"Shaders/VSBillboard.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mBB_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDPosSize, _countof(InputLayoutDesc::IDPosSize), byteCode.data(), byteCode.size(), mILposSize.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/PSBillboard.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mBB_PS.GetAddressOf()));
	}

	if (d3dHelper::LoadShaderByteCode(L"Shaders/GSBillboard.cso", byteCode))
	{
		HR(_device->CreateGeometryShader (byteCode.data(), byteCode.size(), nullptr, mBB_GS.GetAddressOf()));
	}
#pragma endregion

}

