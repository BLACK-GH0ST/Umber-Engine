#pragma once

/**********************************************************************************************
*
*  File Name   : Controller.h
*
*  Description :
*      Abstract base class for all controller types in the engine.
*      Controllers define behavior and logic that update every frame.
*
**********************************************************************************************/

class Controller
{
public:

  Controller () = default;
  virtual ~Controller () = default;

  /**
   * Called every frame to update controller logic.
   * @param deltaTime - Time elapsed since last frame.
   */
  virtual void Update (float deltaTime) = 0;

};