#pragma once

#include "d3dUtil.h"


class Shader
{
public:
	Shader();
	~Shader();
	Shader(const Shader& _other) = delete;
	Shader& operator= (const Shader& _other) = delete;

private:
	
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mobj_VS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mInst_VS;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mRTT_VS;


	Microsoft::WRL::ComPtr<ID3D11PixelShader> mobj_PSt0;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mobj_PSd1;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mobj_PSd2;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mobj_PSCube;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mRTT_PSRed;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> mBlurHor_CS;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> mBlurVert_CS;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> mObjShadow_PS;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> msky_VS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  msky_PS;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>   mBB_VS;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> mBB_GS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>    mBB_PS;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> mTerrian_VS;
	Microsoft::WRL::ComPtr<ID3D11HullShader>   mTerrian_HS;
	Microsoft::WRL::ComPtr<ID3D11DomainShader> mTerrian_DS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  mTess_PS;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> mILpos;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mILBasic32;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mILBasic32Inst;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mILposSize;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mILTerrian;


public:

	ID3D11VertexShader* GetObjVS() const;
	ID3D11VertexShader* GetInstVS() const;
	ID3D11VertexShader* GetRTTVS() const;


	ID3D11PixelShader*  GetPSt0() const;
	ID3D11PixelShader*  GetPSd1() const;
	ID3D11PixelShader*  GetPSd2() const;
	ID3D11PixelShader*  GetRTTPSRed()  const;
	ID3D11PixelShader*  GetPSCube() const;

	ID3D11ComputeShader* GetBlurHorCS() const;
	ID3D11ComputeShader* GetBlurVerCS() const;


	ID3D11PixelShader*  GetObjShaderPS() const;

	ID3D11VertexShader* GetSkyVS() const;
	ID3D11PixelShader*  GetSkyPS() const;

	ID3D11VertexShader*    GetBBVS() const;
	ID3D11GeometryShader*  GetBBGS() const;
	ID3D11PixelShader*     GetBBPS() const;

	ID3D11VertexShader* GetTerrianVS() const;
	ID3D11HullShader*   GetTerrianHS() const;
	ID3D11DomainShader* GetTerrianDS() const;
	ID3D11PixelShader*  GetTerrianPS() const;


	ID3D11InputLayout* GetPosIL() const;
	ID3D11InputLayout* GetBasic32ILInst() const;
	ID3D11InputLayout* GetBasic32IL() const;
	ID3D11InputLayout* GetPosSize() const;
	ID3D11InputLayout* GetTerrianIL() const;

	// Loading shaders and create Input Layout
	void LoadShaders(ID3D11Device* _device);
};

