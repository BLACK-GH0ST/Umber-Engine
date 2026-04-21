

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
  if(!mCamera) return;

  if(mCamera->GetLMB () && !mCamera->GetRMB ())
  {
  //Rotate + allow WASD Movement 
    mYaw += dx * mSensitivity;
    mPitch += dy * mSensitivity * 3.40f;
  }

  if (mCamera->GetRMB () && !mCamera->GetLMB ())
  {
  //Free look (FPS style Camera controls)
  mYaw += dx * mSensitivity;
  mPitch += dy * mSensitivity * 3.40f;
  }
  if(mCamera->GetLMB () && mCamera->GetRMB ())
  {
  // move Vertically
    mposition.y += dy * mMoveSpeed;
  }

  //mPitch =  std::clamp (mPitch, -DirectX::XM_PIDIV2 + 0.1f, DirectX::XM_PIDIV2 - 0.1f); // Clamp pitch to avoid flipping
  mPitch = std::clamp (mPitch, -1.5f, 1.5f);
}

void CameraController::Update (float dt)
{
  if(!mCamera) return;

  mCamera->Yaw (mYaw);
  mCamera->Pitch (mPitch);
  

  mYaw = 0.0f;
  mPitch = 0.0f;

  // Movement for Keyboard (pseudo for now)
  if(GetAsyncKeyState ('W') & 0x8000) mCamera->Walk (mMoveSpeed * dt);
  if(GetAsyncKeyState ('S') & 0x8000) mCamera->Walk (-mMoveSpeed * dt);
  if(GetAsyncKeyState ('A') & 0x8000) mCamera->Strafe (-mMoveSpeed * dt);
  if(GetAsyncKeyState ('D') & 0x8000) mCamera->Strafe (mMoveSpeed * dt);
  if(GetAsyncKeyState ('Q') & 0x8000) mCamera->UpandDown (-mMoveSpeed * dt);
  if(GetAsyncKeyState ('E') & 0x8000) mCamera->UpandDown (mMoveSpeed * dt);
}