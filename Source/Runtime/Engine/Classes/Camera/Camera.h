#pragma once

/**********************************************************************************************
*
*  File Name   : Camera.h
*
*  Description :
*      Represents a camera in 3D space.
*      Responsible for position, rotation, and view transformations.
*		Simple first person style camera class that lets the viewer explore the 3D scene.
*   -It keeps track of the camera coordinate system relative to the world space
*    so that the view matrix can be constructed.  
*   -It keeps track of the viewing frustum of the camera so that the projection
*    matrix can be obtained
*
**********************************************************************************************/


#ifndef CAMERA_H
#define CAMERA_H

#include "../../../../../Build/stdafx.h"
#include "../../../../../Math/MathHelper.h"

class Camera
{
public:
	Camera ();
	~Camera ();

	//Get/Set world Camera position
	DirectX::XMVECTOR GetPosition () const;
	DirectX::XMFLOAT3 GetPosition3f () const;
	
	void SetPosition (float x, float y, float z);
	void SetPosition (const DirectX::XMFLOAT3& v);

	// Get camera basis vector 
	DirectX::XMVECTOR GetRight ()const;
	DirectX::XMFLOAT3 GetRight3f () const;
	DirectX::XMVECTOR GetUp () const;
	DirectX::XMFLOAT3 GetUp3f () const;
	DirectX::XMVECTOR GetLook () const;
	DirectX::XMFLOAT3 GetLook3f () const;

	// Get frustum properpies 
	float GetNearZ () const;
	float GetFarZ  () const;
	float GetAspect () const;
	float GetFovY () const;
	float GetFovX () const;

	//Get near and far planes dimention in view space coordinates
	float GetNearWindowWidth () const;
	float GetNearWindowHeight () const;
	float GetFarWindowWidth () const;
	float GetFarWindowHeight () const;

	//Set Frustum / Projection system.
	void SetLens (float fovY, float aspect, float zn, float zf);

	//Define camera space via LookAt parameter
	void _LookAt_ (DirectX::XMVECTOR pos, DirectX::XMVECTOR target, DirectX::XMVECTOR proj);
	void LookAt (const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& proj);

	//Get view/proj matrices
	DirectX::XMMATRIX GetView () const;
	DirectX::XMMATRIX GetProj () const;

	DirectX::XMFLOAT4X4 GetView4x4f () const;
	DirectX::XMFLOAT4X4 GetProj4x4f () const;

	//Strafe/walk the camera a distance d
	void Strafe (float d);
	void Walk (float d);

	//Rotate the camera
	void Pitch (float angle);
	void Yaw (float angle);
	void Roll (float angle);
	void RotateY (float angle);

	//After modifying camera positioin/oritation, call to rebuild the view matrix
	void UpdateViewMatrix ();


		/** Updates camera state (if needed). */
	void Update ();



private:
		/** Camera position in world space. */
	float mPosX, mPosY, mPosZ;

	/** Camera rotation (pitch, yaw, roll). */
	float mPitch, mYaw, mRoll;

	/** Reset the camera after every change. */
	bool mViewDirty = true;

	DirectX::XMFLOAT3 mPosition = { .0f, .0f, .0f };
	DirectX::XMFLOAT4 mRotationQuat;									 //quaterion / identity 
	DirectX::XMFLOAT3 mRight;													// = { 1.f, .0f, .0f };
	DirectX::XMFLOAT3 mUp;														// = { .0f, 1.f, .0f };
	DirectX::XMFLOAT3 mLook;													// = { .0f, .0f, 1.f };

	// Cache frustum properties
	float mNearZ = .0f;
	float mFarZ = .0f;
	float mAspect = .0f;
	float mFovY = .0f;
	float mNearWindowHeight = .0f;
	float mFarWindowHeight = .0f;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4 ();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4 ();

};

#endif // !CAMERA_H

