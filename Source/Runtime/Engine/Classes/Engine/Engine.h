#pragma once

/**********************************************************************************************
*
*  File Name   : Engine.h
*  Author      : Umbra Engine Team
*  Created     : 2026
*
*  Description :
*      Declares the core Engine class responsible for initializing the application,
*      managing the main loop, and coordinating update and render systems.
*      Core engine class responsible for:
*          - Window initialization
*          - Main game loop execution
*          - Input handling
*          - Updating and rendering systems
*
**********************************************************************************************/

// Implemented Headers
#include "../../../../../Build/Resource.h"

#include "../Camera/Camera.h"
#include "../../Classes\CameraController/CameraController.h"
#include "../../Classes/Renderer/Renderer.h"
#include "../../Classes/Timer/Timer.h"

class Camera;
class CameraController;
class Renderer;

// using Engine Space =>
namespace Umbra
{
  class Engine
  {
  public:

    /**
     * Initializes the engine systems and creates the main window.
     *
     * @param hInstance - Handle to the application instance.
     * @param nCmdShow  - Window display option.
     * @return True if initialization succeeds, false otherwise.
     */
    bool Init (HINSTANCE hInstance, int nCmdShow);

    /**
     * Starts the main execution loop of the engine.
     *
     * @return Exit code.
     */
    int Run ();

    /**
     * Handles key press events.
     */
    void OnKeyDown (WPARAM wparam);

    /**
     * Handles key release events.
     */
    void OnKeyUp (WPARAM wparam);

      /** Updates engine systems (called every frame). */
    void Update ();

    /** Renders the current frame. */
    void Render ();

    /** Per-frame tick function (timing, updates, etc.). */
    void Tick ();

    /** Initializes the application window. */
    bool InitWindow (HINSTANCE hInstance, int nCmdShow);

    /** Handles mouse movement (dx, dy). */
    void OnMouseMove (float dx, float dy);
    void OnMouseButton (bool left, bool down);

  private:

      /** Main application window handle. */
    HWND m_hWnd = nullptr;

    /** Main camera used for rendering the scene. */
    Camera* mCamera;

    /**
     * Controls camera movement and input.
     * Owned by the engine and managed via smart pointer.
     */
    std::unique_ptr<CameraController> mCameraController;

    /** Renderer subsystem. */
    std::unique_ptr<Renderer> mRenderer;

    /** Persistent frame timer used by the main loop. */
    Timer mTimer;
  };
}

