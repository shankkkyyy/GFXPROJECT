#pragma once
#include "d3dUtil.h"

class Camera
{



public:

	Camera();
	~Camera();


public:

	void SetPosition(const DirectX::XMFLOAT3& _val);
	void SetRotation(float _pitch, float _yaw, float _raw);


	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMVECTOR GetPositionXM() const;
	DirectX::XMFLOAT4X4 GetView() const;
	DirectX::XMMATRIX GetViewXM() const;
	DirectX::XMMATRIX GetProj() const;
	DirectX::XMFLOAT4X4 GetViewProj() const;
	DirectX::XMMATRIX GetViewProjXM() const;

	DirectX::XMMATRIX GetWorld() const;
	const DirectX::BoundingFrustum* const GetFrustum() const;
	
	void SetLen(float _fov, float _ratio, float _np, float _fp);

	void Update(float _deltaTime);

	// Get new projection matrix when fov, np, fp, ratio get changed
	void OnResize();

	void OnMouseMove();

	void OnMouseWheel();

	void OnKeyDown();


	float GetFOV() const;
	float GetAspect()const;
	float GetNearPlane() const;
	float GetFarPlane() const;

	
private:

	float mFieldOfView;

	float mNP;
	
	float mFP;

	float mAspect;

	DirectX::BoundingFrustum mFrustumView;

	DirectX::XMFLOAT3 mUp;

	DirectX::XMFLOAT3 mRight;

	DirectX::XMFLOAT3 mForward;

	DirectX::XMFLOAT3 mPosition;

	DirectX::XMFLOAT4X4 mView;

	DirectX::XMFLOAT4X4 mProj;

	DirectX::XMFLOAT4X4 mWorld;

	DirectX::XMFLOAT4X4 mViewProj;

public:

	// move along forward in X, Z space
	void MoveForward(float _delta);

	// move along right in X, Z space
	void MoveRight(float _delta);

	// move up world Y
	void MoveY(float _delta);

	// move along forward
	void Zoom(float _delta);

	// move along right
	void Strafe(float _delta);

	// rotate on local X
	void LookUp(float _delta);

	// rotate on world Y
	void LookRight(float _delta);

	void UpdateViewMatrix(CXMVECTOR forward, CXMVECTOR right, CXMVECTOR up);


};

