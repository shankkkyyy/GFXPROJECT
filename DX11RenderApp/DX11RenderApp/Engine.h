#pragma once

#include "SceneRenderApp.h"
#include "BaseScene.h"
#include "Shader.h"
#include "Object.h"


class Engine : public SceneRender
{
public:

	Engine(HINSTANCE _hInstance);
	~Engine();

private:

	static Engine* mEngine;
	Shader*  mShaders = nullptr;
	Objects* mAssetsPool = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mScreenVertexBuffer = nullptr;

	std::vector<BaseScene*>* mAllScenes = nullptr;
	CountrySide*             mCountrySideScene = nullptr;
	CarDemo*                 mCarDemo = nullptr;

	UINT         mCurrentScene = 0;

	bool bBlur = false;

public:

	bool InitApp() override;
	void AddBlur();


public:
	static Engine* GetEngine();
	Shader*        GetShaders() const;
	BaseScene*     GetCurrentScene() const; 
	ID3D11Buffer*  GetScreenVertexBuffer() const;

	bool           GetIsBlurOn() const;


private:

	void LoadAndLinkResource();

	void CreateScreenBuffer();

	void InitRenderSettings();

	void UpdateScene(float _deltaTime) override;

	void DrawScene() override;	

	void OverallInput();

};



