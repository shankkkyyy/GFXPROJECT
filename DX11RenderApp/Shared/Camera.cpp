
#include "Camera.h"
#include "SceneRenderApp.h"


Camera::Camera() : 
	mFieldOfView(MathHelper::Pi * 0.25f),
	mNP(.3f),
	mFP(800.0f)
{

	XMFLOAT4X4 r;
	XMStoreFloat4x4(&r, XMMatrixRotationRollPitchYaw(0.0f, 0.0f , 0.0f)); //1.5f*MathHelper::Pi

	mRight   = { r(0, 0) , r(0, 1), r(0, 2) };
	mUp      = { r(1, 0) , r(1, 1), r(1, 2) };
	mForward = { r(2, 0) , r(2, 1), r(2, 2) };
	mPosition = { 0, 0, 0 };

	Zoom(-5.0f);	
}

Camera::~Camera()
{
}

void Camera::SetPosition(const XMFLOAT3& _val)
{
	mPosition = _val;
}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
	return mPosition;
}

DirectX::XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPosition);
}

DirectX::XMFLOAT4X4 Camera::GetView() const
{
	return mView;
}

DirectX::XMMATRIX Camera::GetViewXM() const
{
	return XMLoadFloat4x4(&mView);
}

DirectX::XMMATRIX Camera::GetProj() const
{
	return XMLoadFloat4x4(&mProj);
}

DirectX::XMFLOAT4X4 Camera::GetViewProj() const
{
	return mViewProj;
}

DirectX::XMMATRIX Camera::GetViewProjXM() const
{
	return XMLoadFloat4x4(&mViewProj);
}

DirectX::XMMATRIX Camera::GetWorld() const
{
	return XMLoadFloat4x4(&mWorld);
}

const DirectX::BoundingFrustum * const Camera::GetFrustum() const
{
	return &mFrustumView;
}



void Camera::Update(float _deltaTime)
{

#pragma region Response To Input

	if (GetAsyncKeyState(KEY_W) & 0x8000)
	{
		MoveForward(10.0f * _deltaTime);
	}
	else if (GetAsyncKeyState(KEY_S) & 0x8000)
	{
		MoveForward(-10.0f * _deltaTime);
	}

	if (GetAsyncKeyState(KEY_A) & 0x8000)
	{
		MoveRight(-10.0f * _deltaTime);
	}
	else if (GetAsyncKeyState(KEY_D) & 0x8000)
	{
		MoveRight(10.0f * _deltaTime);
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		MoveY(10.0f * _deltaTime);
	}
	else if (GetAsyncKeyState(VK_LSHIFT) & 0x8000)
	{
		MoveY(-10.0f * _deltaTime);
	}






#pragma endregion

#pragma region Orthogonal Correction

	// right, up, forward, right, up

	XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&mForward));

	XMVECTOR up      = XMVector3Normalize(XMVector3Cross(forward, XMLoadFloat3(&mRight)));

	XMVECTOR right   = XMVector3Cross(up, forward);

#pragma endregion

#pragma region Update View Matrix

	XMVECTOR position = XMLoadFloat3(&mPosition);

	mView(3, 0) = -XMVectorGetX(XMVector3Dot(position, right));
	mView(3, 1) = -XMVectorGetX(XMVector3Dot(position, up));
	mView(3, 2) = -XMVectorGetX(XMVector3Dot(position, forward));
	mView(3, 3) = 1.0f;

	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mForward, forward);
	XMStoreFloat3(&mUp, up);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;

	mView(0, 2) = mForward.x;
	mView(1, 2) = mForward.y;
	mView(2, 2) = mForward.z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;

	XMStoreFloat4x4(&mViewProj, XMLoadFloat4x4(&mView) * XMLoadFloat4x4(&mProj));

#pragma endregion

}

void Camera::OnResize()
{
	// calculate new projection matrix
	XMMATRIX proj = XMMatrixPerspectiveFovLH(mFieldOfView, SceneRender::GetApp()->GetAspectRatio(), mNP, mFP);
	XMStoreFloat4x4(&mProj, proj);

	// calculate new frustum
	BoundingFrustum::CreateFromMatrix(mFrustumView, proj);
}

void Camera::OnMouseMove()
{
	const WndInput* const input = SceneRender::GetApp()->GetWndInput();
	if (input->mButton & MK_LBUTTON)
	{
		float yaw = XMConvertToRadians(0.1f * static_cast<float>(input->deltaXY.x));
		float pitch = XMConvertToRadians(0.1f * static_cast<float>(input->deltaXY.y));
		LookUp(pitch);
		LookRight(yaw);
	}
}

void Camera::OnMouseWheel()
{
	const WndInput* const input = SceneRender::GetApp()->GetWndInput();
	Zoom(input->wheelDistance * 0.01f);
}

void Camera::OnKeyDown()
{


}


void Camera::MoveForward(float _delta)
{
	XMVECTOR scaler = XMVectorSet(_delta, 0.0f, _delta, _delta);
	XMVECTOR forward = XMLoadFloat3(&mForward);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(forward, scaler, XMLoadFloat3(&mPosition)));
}

void Camera::MoveRight(float _delta)
{
	XMVECTOR scaler = XMVectorSet(_delta, 0.0f, _delta, _delta);
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(right, scaler, XMLoadFloat3(&mPosition)));
}

void Camera::MoveY(float _delta)
{
	mPosition.y += _delta;
}

void Camera::Zoom(float _delta)
{
	//p += forward * d;
	XMVECTOR scaler = XMVectorReplicate(_delta);
	XMVECTOR forward = XMLoadFloat3(&mForward);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(forward, scaler, XMLoadFloat3(&mPosition)));
}

void Camera::Strafe(float _delta)
{
	XMVECTOR scaler = XMVectorReplicate(_delta);
	XMVECTOR right = XMLoadFloat3(&mRight);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(right, scaler, XMLoadFloat3(&mPosition)));
}

void Camera::LookUp(float _delta)
{

	// Right Remain the same, rotate Up, Forward

	XMMATRIX r = XMMatrixRotationAxis(XMLoadFloat3(&mRight), _delta);

	XMStoreFloat3(&mForward, XMVector3TransformNormal(XMLoadFloat3(&mForward), r));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), r));

}

void Camera::LookRight(float _delta)
{
	XMMATRIX r = XMMatrixRotationY(_delta);

	XMStoreFloat3(&mForward, XMVector3TransformNormal(XMLoadFloat3(&mForward), r));
	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), r));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), r));
}


