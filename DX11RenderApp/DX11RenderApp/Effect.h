#pragma once


#include "D3DUtil.h"

// effect base class that handles loading shaders, create activate effect framework
class Effect
{
public:
	Effect(ID3D11Device* _d3dDevice, const std::wstring& _path);
	virtual ~Effect();
	Effect(const Effect& _other) = delete;
	Effect& operator= (const Effect& _other) = delete;


protected:
	ID3DX11Effect* mFx;

};


// effect derived class contains light manipulation
class BaseEffect : public Effect
{
public: 
	BaseEffect(ID3D11Device* _d3dDevice, std::wstring _path);
	~BaseEffect() {};


	void SetDirectionLight(const DirectionalLight& _directionalLight);
	void SetPointLight(const PointLight& _pointLight);
	void SetSpotLight(const SpotLight& _spotLight);
	void SetEyePosition(const DirectX::XMFLOAT3& _eyePos);
	


	void SetMaterial(const Material& _material);	
	void SetWVPMatrix(DirectX::CXMMATRIX _wvp);
	void SetWorldMatrix(DirectX::CXMMATRIX _wMatrix);
	void SetWorldInvTranspMatrix(DirectX::CXMMATRIX _wiv);
	void SetTexTransform(DirectX::CXMMATRIX _tM);
	void SetTexResource(ID3D11ShaderResourceView* _tex);
	



public:

	ID3DX11EffectTechnique* mLightTech;
	ID3DX11EffectTechnique* mLightTechNoTex;

	// per frame
	ID3DX11EffectVariable* mFxDirectLight;
	ID3DX11EffectVariable* mFxPointLight;
	ID3DX11EffectVariable* mFxSpotLight;
	ID3DX11EffectVectorVariable* mFxEyePos;

	// per object
	ID3DX11EffectVariable* mFxMaterial;
	ID3DX11EffectMatrixVariable* mFxWorldMatrix;
	ID3DX11EffectMatrixVariable* mFxWorldInvTransposeMatrix;
	ID3DX11EffectMatrixVariable* mFxWVPMatrix;
	ID3DX11EffectMatrixVariable* mFxTexTransform;
	ID3DX11EffectShaderResourceVariable* mFxTexMap;
};



// In case we have multiple effects
class Effects
{
public:
	static void InitAll(ID3D11Device* d3dDevice);
	static void DestroyAll();

	static BaseEffect* BaseFx;

};

