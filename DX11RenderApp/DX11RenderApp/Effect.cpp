#include "Effect.h"


#pragma region Effect
Effect::Effect(ID3D11Device * _d3dDevice, const std::wstring & _path)
{
	std::ifstream fin;
	fin.open(_path, std::ios::binary);
	if (fin.is_open())
	{
		fin.seekg(0, std::ios_base::end);
		int byteSize = (int)(fin.tellg());
		fin.seekg(0, std::ios_base::beg);
		std::vector<char> byteCode(byteSize);
		fin.read(byteCode.data(), byteSize);
		fin.close();

		HR(D3DX11CreateEffectFromMemory(byteCode.data(), byteSize, 0, _d3dDevice, &mFx));
	}
	else
	{
		std::wstring message = L"Can not open shader file at path: " + _path;
		MessageBox(nullptr, message.c_str(), 0, 0);
	}
}

Effect::~Effect()
{
	ReleaseCOM(mFx);
}
#pragma endregion


#pragma region BaseEffect

BaseEffect::BaseEffect(ID3D11Device * _d3dDevice, std::wstring _path) : Effect(_d3dDevice, _path)
{
	mLightTech = mFx->GetTechniqueByName("lightTech");
	mLightTechNoTex = mFx->GetTechniqueByName("lightTechNoTex");

	mFxDirectLight = mFx->GetVariableByName("gDirectLight");
	mFxSpotLight = mFx->GetVariableByName("gSpotLight");
	mFxPointLight = mFx->GetVariableByName("gPointLight");

	mFxEyePos = mFx->GetVariableByName("gEyePos_w")->AsVector();


	mFxMaterial     = mFx->GetVariableByName("gMaterial");
	mFxWorldMatrix  = mFx->GetVariableByName("gWorld_w")->AsMatrix();
	mFxWVPMatrix    = mFx->GetVariableByName("gWorldViewProj_w")->AsMatrix();
	mFxTexMap       = mFx->GetVariableByName("gTexMap")->AsShaderResource();
	mFxTexTransform = mFx->GetVariableByName("gTexTransform_w")->AsMatrix();
	mFxWorldInvTransposeMatrix = mFx->GetVariableByName("gWorldInvTranspose_w")->AsMatrix();
}

void BaseEffect::SetDirectionLight(const DirectionalLight & _directionalLight)
{
	mFxDirectLight->SetRawValue(&_directionalLight, 0, sizeof(DirectionalLight));
}

void BaseEffect::SetPointLight(const PointLight & _pointLight)
{
	mFxPointLight->SetRawValue(&_pointLight, 0, sizeof(PointLight));
}

void BaseEffect::SetSpotLight(const SpotLight & _spotLight)
{
	mFxSpotLight->SetRawValue(&_spotLight, 0, sizeof(SpotLight));
}

void BaseEffect::SetEyePosition(const DirectX::XMFLOAT3 & _eyePos)
{
	mFxEyePos->SetFloatVector(reinterpret_cast<const float*>(&_eyePos));
}

void BaseEffect::SetMaterial(const Material & _material)
{
	mFxMaterial->SetRawValue(&_material, 0, sizeof(Material));
}

void BaseEffect::SetWVPMatrix(DirectX::CXMMATRIX _wvp)
{
	mFxWVPMatrix->SetMatrix(reinterpret_cast<const float*>(&_wvp));
}

void BaseEffect::SetWorldMatrix(DirectX::CXMMATRIX _wMatrix)
{
	mFxWorldMatrix->SetMatrix(reinterpret_cast<const float*>(&_wMatrix));
}

void BaseEffect::SetWorldInvTranspMatrix(DirectX::CXMMATRIX _wiv)
{
	mFxWorldInvTransposeMatrix->SetMatrix(reinterpret_cast<const float*>(&_wiv));
}

void BaseEffect::SetTexTransform(DirectX::CXMMATRIX _tM)
{
	mFxTexTransform->SetMatrix(reinterpret_cast<const float*>(&_tM));
}

void BaseEffect::SetTexResource(ID3D11ShaderResourceView * _tex)
{
	mFxTexMap->SetResource(_tex);
}


#pragma endregion


#pragma region Effects


BaseEffect* Effects::BaseFx = nullptr;

void Effects::InitAll(ID3D11Device * d3dDevice)
{
	BaseFx = new BaseEffect(d3dDevice, L"FXs/Shaders.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BaseFx);
}

#pragma endregion

