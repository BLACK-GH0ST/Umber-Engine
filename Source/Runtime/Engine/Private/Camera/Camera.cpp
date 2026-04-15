
#include "../../../../../Build/stdafx.h"
#include "../../Classes/Camera/Camera.h"

using namespace DirectX;

Camera::Camera()
{
	SetLens(.25f*MathHelper::Pi, 1.f, 1.f, 1000.f);
	mPosition = XMFLOAT3 (0.0f, 0.0f, 0.0f);
	mRight = XMFLOAT3 (1.0f, 0.0f, 0.0f);
	mUp = XMFLOAT3 (0.0f, 1.0f, 0.0f);
	mLook = XMFLOAT3 (0.0f, 0.0f, 1.0f);
	UpdateViewMatrix ();
}

Camera::~Camera() {}

XMVECTOR Camera::GetPosition()const
{
	return XMLoadFloat3(&mPosition);
}
  
XMFLOAT3 Camera::GetPosition3f()const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
	mViewDirty = true;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	mPosition = v;
	mViewDirty = true;
}

XMVECTOR Camera::GetRight()const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight3f()const
{
	return mRight;
}

XMVECTOR Camera::GetUp()const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp3f()const
{
	return mUp;
}

XMVECTOR Camera::GetLook()const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook3f()const
{
	return mLook;
}

float Camera::GetNearZ()const
{
	return mNearZ;
}

float Camera::GetFarZ()const
{
	return mFarZ;
}

float Camera::GetAspect()const
{
	return mAspect;
}

float Camera::GetFovY()const
{
	return mFovY;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
	mViewDirty = true;
}

void Camera::_LookAt_(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);

	mViewDirty = true;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	_LookAt_(P, T, U);

	mViewDirty = true;
}

XMMATRIX Camera::GetView()const
{
	assert(!mViewDirty);
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProj()const
{
	return XMLoadFloat4x4(&mProj);
}


XMFLOAT4X4 Camera::GetView4x4f()const
{
	assert(!mViewDirty);
	return mView;
}

XMFLOAT4X4 Camera::GetProj4x4f()const
{
	return mProj;
}

void Camera::Strafe(float d)
{
	XMVECTOR q = XMLoadFloat4 (&mRotationQuat);
	XMVECTOR right = XMVector3Rotate (XMVectorSet (1.0f, 0.0f, 0.0f, 0.0f), q);

	XMVECTOR pos = XMLoadFloat3 (&mPosition);
	pos = XMVectorMultiplyAdd (XMVectorReplicate (d), right, pos);

	XMStoreFloat3 (&mPosition, pos);
	mViewDirty = true;
}

void Camera::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	mViewDirty = true;
}

void Camera::Pitch(float angle) // Local X
{
	XMVECTOR q = XMLoadFloat4 (&mRotationQuat);
	XMVECTOR axis = XMVector3Rotate (XMVectorSet (1.0f, 0.0f, 0.0f, 0.0f), q);

	XMVECTOR rot = XMQuaternionRotationAxis (axis, angle);
	q = XMQuaternionMultiply (rot, q);

	XMStoreFloat4 (&mRotationQuat, XMQuaternionNormalize (q));
	mViewDirty = true;
}

void Camera::Yaw (float angle) // Local Y
{
	XMVECTOR q = XMLoadFloat4 (&mRotationQuat);

	XMVECTOR rot = XMQuaternionRotationAxis (XMVectorSet (0.0f, 1.0f, 0.0f, 0.0f), angle);
	q = XMQuaternionMultiply (rot, q);

	XMStoreFloat4 (&mRotationQuat, XMQuaternionNormalize (q));
	mViewDirty = true;
}

void Camera::Roll (float angle) // Local Z
{
	XMVECTOR q = XMLoadFloat4 (&mRotationQuat);
	XMVECTOR forward = XMVector3Rotate (XMVectorSet (0.0f, 0.0f, 1.0f, 0.0f), q);
	
	XMVECTOR rot = XMQuaternionRotationAxis (forward, angle);
	q = XMQuaternionMultiply (rot, q);

	XMStoreFloat4 (&mRotationQuat, XMQuaternionNormalize (q));
	mViewDirty = true;
}

void Camera::RotateY(float angle)
{
	// Rotate the basis vectors about the world y-axis.

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight,   XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

	mViewDirty = true;
}

void Camera::UpdateViewMatrix()
{
	if(!mViewDirty) return;

	XMVECTOR pos = XMLoadFloat3 (&mPosition);
	XMVECTOR q = XMLoadFloat4 (&mRotationQuat);

	XMVECTOR right = XMVector3Rotate (XMVectorSet (1.0f, 0.0f, 0.0f, 0.0f), q);
	XMVECTOR up = XMVector3Rotate (XMVectorSet (0.0f, 1.0f, 0.0f, 0.0f), q);
	XMVECTOR forward = XMVector3Rotate (XMVectorSet (0.0f, 0.0f, 1.0f, 0.0f), q);

	XMMATRIX view = XMMatrixLookToLH (pos, forward, up);
	XMStoreFloat4x4 (&mView, view);
	XMStoreFloat3 (&mRight, right);
	XMStoreFloat3 (&mUp, up);
	XMStoreFloat3 (&mLook, forward);
	mViewDirty = false;
}
