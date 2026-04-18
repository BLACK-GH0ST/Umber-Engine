

#include "../../Classes/CameraController/CameraController.h"
#include "../../Classes/Camera/Camera.h"

// CPP Headers
#include <algorithm> // for clamp
#include <DirectXMath.h>

CameraController::CameraController () {}

CameraController::CameraController (Camera* cam)
  : mCamera (cam)
{}

void CameraController::OnMouseMove (float dx, float dy)
{
  mYaw += dx * mSensitivity;
  mPitch += dy * mSensitivity * 3.40;

  //mPitch =  std::clamp (mPitch, -DirectX::XM_PIDIV2 + 0.1f, DirectX::XM_PIDIV2 - 0.1f); // Clamp pitch to avoid flipping

}

void CameraController::Update (float dt)
{
  if(!mCamera) return;

  mCamera->Yaw (mYaw);
  mCamera->Pitch (mPitch);
  

  mYaw = 0.0f;
  mPitch = 0.0f;

  // Movement (pseudo for now)
  if(GetAsyncKeyState ('W') & 0x8000) mCamera->Walk (mMoveSpeed * dt);
  if(GetAsyncKeyState ('S') & 0x8000) mCamera->Walk (-mMoveSpeed * dt);
  if(GetAsyncKeyState ('A') & 0x8000) mCamera->Strafe (-mMoveSpeed * dt);
  if(GetAsyncKeyState ('D') & 0x8000) mCamera->Strafe (mMoveSpeed * dt);
  if(GetAsyncKeyState ('Q') & 0x8000) mCamera->Strafe (mMoveSpeed * dt);
  if(GetAsyncKeyState ('E') & 0x8000) mCamera->Strafe (mMoveSpeed * dt);
}