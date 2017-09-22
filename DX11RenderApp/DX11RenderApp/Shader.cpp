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

ID3D11PixelShader * Shader::GetPSCube() const
{
	return mobj_PSCube.Get();
}

ID3D11ComputeShader * Shader::GetBlurHorCS() const
{
	return mBlurHor_CS.Get();
}

ID3D11ComputeShader * Shader::GetBlurVerCS() const
{
	return mBlurVert_CS.Get();
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

ID3D11VertexShader * Shader::GetTerrianVS() const
{
	return mTerrian_VS.Get();
}

ID3D11HullShader * Shader::GetTerrianHS() const
{
	return mTerrian_HS.Get();
}

ID3D11DomainShader * Shader::GetTerrianDS() const
{
	return mTerrian_DS.Get();
}

ID3D11PixelShader * Shader::GetTessPS() const
{
	return mTess_PS.Get();
}

ID3D11PixelShader * Shader::GetTerrianPS() const
{
	return mTerrian_PS.Get();
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

ID3D11InputLayout * Shader::GetTerrianIL() const
{
	return mILTerrian.Get();
}



void Shader::LoadShaders(ID3D11Device * _device)
{

	std::vector<char> byteCode;

#pragma region Load shaders for basic32 Vertex

	if (d3dHelper::LoadByteCode(L"Shaders/VertexShader.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mobj_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDBasic32, _countof(InputLayoutDesc::IDBasic32), byteCode.data(), byteCode.size(), mILBasic32.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/VSInstance.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mInst_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDBasic32Inst, _countof(InputLayoutDesc::IDBasic32Inst), byteCode.data(), byteCode.size(), mILBasic32Inst.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/VSRTT.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mRTT_VS.GetAddressOf()));
	}

	// Load Pixel shader for basic32 Vertex
	if (d3dHelper::LoadByteCode(L"Shaders/PSt0.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mobj_PSt0.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/PSd1.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mobj_PSd1.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/PSd2.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mobj_PSd2.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/PSShadow.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mObjShadow_PS.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/PSRTT_red.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mRTT_PSRed.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/PSCubeMap.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mobj_PSCube.GetAddressOf()));
	}


#pragma endregion

#pragma region Load shaders for SkyBox

	if (d3dHelper::LoadByteCode(L"Shaders/SkyBoxVS.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, msky_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDPos, _countof(InputLayoutDesc::IDPos), byteCode.data(), byteCode.size(), mILpos.GetAddressOf()));
	}
	// Load PS for SkyBox
	if (d3dHelper::LoadByteCode(L"Shaders/SkyBoxPS.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, msky_PS.GetAddressOf()));
	}
#pragma endregion

#pragma region Load Shaders for Billboard

	if (d3dHelper::LoadByteCode(L"Shaders/VSBillboard.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mBB_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDPosSize, _countof(InputLayoutDesc::IDPosSize), byteCode.data(), byteCode.size(), mILposSize.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/PSBillboard.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mBB_PS.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/GSBillboard.cso", byteCode))
	{
		HR(_device->CreateGeometryShader (byteCode.data(), byteCode.size(), nullptr, mBB_GS.GetAddressOf()));
	}
#pragma endregion


#pragma region Load Shaders for Blur
	if (d3dHelper::LoadByteCode(L"Shaders/CSHorBlur.cso", byteCode))
	{
		HR(_device->CreateComputeShader(byteCode.data(), byteCode.size(), nullptr, mBlurHor_CS.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/CSVertBlur.cso", byteCode))
	{
		HR(_device->CreateComputeShader(byteCode.data(), byteCode.size(), nullptr, mBlurVert_CS.GetAddressOf()));
	}
#pragma endregion


#pragma region Load Shaders for Tess

	if (d3dHelper::LoadByteCode(L"Shaders/VSTerrian.cso", byteCode))
	{
		HR(_device->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, mTerrian_VS.GetAddressOf()));
		HR(_device->CreateInputLayout(
			InputLayoutDesc::IDTerrian, _countof(InputLayoutDesc::IDTerrian), byteCode.data(), byteCode.size(), mILTerrian.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/HSTerrian.cso", byteCode))
	{
		HR(_device->CreateHullShader(byteCode.data(), byteCode.size(), nullptr, mTerrian_HS.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/DSTerrian.cso", byteCode))
	{
		HR(_device->CreateDomainShader(byteCode.data(), byteCode.size(), nullptr, mTerrian_DS.GetAddressOf()));
	}


	if (d3dHelper::LoadByteCode(L"Shaders/PSTerr.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mTerrian_PS.GetAddressOf()));
	}

	if (d3dHelper::LoadByteCode(L"Shaders/PSTess.cso", byteCode))
	{
		HR(_device->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, mTess_PS.GetAddressOf()));
	}

#pragma endregion

}

