#pragma once
#include <DirectXMath.h>

class Camera
{



public:

	Camera();
	~Camera();


	DirectX::XMMATRIX GetView() const;
	DirectX::XMMATRIX GetProj() const;
	DirectX::XMMATRIX GetViewProj() const;


	void Update(float _deltaTime);

	// Get new projection matrix when fov, np, fp, ratio get changed
	void GetNewPerspective();

	void OnMouseMove();

	void OnMouseWheel();

	void OnKeyDown();
	
private:

	float mFieldOfView;

	float mNP;
	
	float mFP;



	DirectX::XMFLOAT3 mUp;

	DirectX::XMFLOAT3 mRight;

	DirectX::XMFLOAT3 mForward;

	DirectX::XMFLOAT3 mPosition;

	DirectX::XMFLOAT4X4 mView;

	DirectX::XMFLOAT4X4 mProj;

private:


	// move along world Z
	void MoveForward(float _delta);

	// move along world X
	void MoveRight(float _delta);

	// move along forward
	void Zoom(float _delta);

	// move along right
	void Strafe(float _delta);

	// rotate on local X
	void LookUp(float _delta);

	// rotate on world Y
	void LookRight(float _delta);

};

