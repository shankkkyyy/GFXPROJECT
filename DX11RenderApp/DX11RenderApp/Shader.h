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
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mobj_PS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mObjShadow_PS;


	Microsoft::WRL::ComPtr<ID3D11VertexShader> msky_VS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> msky_PS;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> mILpos;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mILBasic32;


public:

	ID3D11VertexShader* GetObjVS() const;
	ID3D11PixelShader*  GetObjPS() const;
	ID3D11PixelShader*  GetObjShaderPS() const;




	ID3D11VertexShader* GetSkyVS() const;
	ID3D11PixelShader*  GetSkyPS() const;

	ID3D11InputLayout* GetPosIL() const;
	ID3D11InputLayout* GetBasic32IL() const;

	// Loading shaders and create Input Layout
	void LoadShaders(ID3D11Device* _device);
};

