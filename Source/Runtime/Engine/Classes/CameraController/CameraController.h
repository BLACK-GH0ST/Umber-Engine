#pragma once

/**********************************************************************************************
*
*  File Name   : CameraController.h
*
*  Description :
*      Implements a controller responsible for handling camera movement and rotation
*      based on user input. Inherits from the base Controller class.
*
*      This class processes mouse movement and updates camera orientation using
*      yaw, pitch, and roll values.
*
**********************************************************************************************/

#include "../FrameWork/Controller.h"

class Camera;

class CameraController : public Controller
{
public:

    /**
     * Default constructor.
     */
  CameraController ();

  /**
   * Constructor with camera binding.
   * @param cam - Pointer to the camera being controlled.
   */
  CameraController (Camera* cam);

  /**
   * Updates the controller every frame.
   * @param deltaTime - Time elapsed since last frame.
   */
  void Update (float deltaTime) override;

  /**
   * Handles mouse movement input.
   * @param dx - Change in X (mouse movement).
   * @param dy - Change in Y (mouse movement).
   */
  void OnMouseMove (float dx, float dy);

private:

    /** Pointer to the controlled camera (not owned). */
  Camera* mCamera = nullptr;

  /** Mouse sensitivity for camera rotation. */
  float mSensitivity = 0.0002f;

  /** Movement speed of the camera. */
  float mMoveSpeed = 10.0f;

  /** Rotation values */
  float mYaw = 0.0f;
  float mPitch = 0.0f;
  float mRoll = 0.0f;
};