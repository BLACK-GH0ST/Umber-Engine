//***************************************************************************************
// Engine.h by Umbra Engine (C) 2026 All Rights Reserved.
// UmberEngine.cpp : Defines the entry point for the application.
//***************************************************************************************



#pragma once

#include "../../../../../Build/stdafx.h"
#include "../../../../../Build/Resource.h"
#include "../Camera/Camera.h"

// Engine NameSpace ...
namespace Umbra
{

  class Engine
  {
  public:
    int Run ();
    bool Init (HINSTANCE hInstance, int nCmdShow);
    LRESULT CALLBACK    WndProc (HWND, UINT, WPARAM, LPARAM);
    INT_PTR CALLBACK    About (HWND, UINT, WPARAM, LPARAM);

  private:
    void Update ();
    void Render ();
    void Tick   ();

    Camera mCamera;
   CameraController mCameraController;
    bool InitWindow (HINSTANCE hInstance, int nCmdShow);

private:
    HWND m_hWnd = nullptr;
  };
} 