
#pragma once

/* some note for later ...*/

#include "../FrameWork/Controller.h"

class Camera;

class CameraController : public Controller
{
public:
  CameraController ();
  CameraController (Camera* cam);

  void Update (float deltaTime) override;

  void OnMouseMove (float dx, float dy);

private:
  Camera* mCamera;

  float mSensitivity = 0.002f;
  float mMoveSpeed = 10.0f;

  float mYaw = 0.0f;
  float mPitch = 0.0f;
  float mRoll = 0.0f;
};

 