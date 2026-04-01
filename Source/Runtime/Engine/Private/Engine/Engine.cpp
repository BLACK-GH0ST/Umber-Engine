
#include <Windowsx.h>

#include "..\..\Classes\Engine/Engine.h"
#include "../../../../../Build/stdafx.h"
#include "..\..\..\..\..\Build\Resource.h"
#include "../../Classes/Camera/Camera.h"
#include "../../Classes/EngineTimer/EngineTimer.h"
#include "..\..\Classes\CameraController\CameraController.h"

#define MAX_LOADSTRING 100

HINSTANCE hInst;                               
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
Umbra::Engine* gEngine = nullptr;               

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_UMBERENGINE, szWindowClass, MAX_LOADSTRING);
     
    if(!gEngine->Init (hInstance, nCmdShow))
    {
      return -1;
    }
    return gEngine->Run();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_MOUSEMOVE:
      {
        static float lastX = 0;
        static float lastY = 0;

        float x = (float)GET_X_LPARAM (lParam);
        float y = (float)GET_Y_LPARAM (lParam);

        float dx = x - lastX;
        float dy = y - lastY;

        lastX = x;
        lastY = y;

      }
      break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


namespace Umbra // Umbra Engine =>
{
  int Engine::Run ()
  {
    MSG msg = {};
    while(msg.message != WM_QUIT)
    {
      if(PeekMessage (&msg, nullptr, 0, 0, PM_REMOVE))
      {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
      } 
        else 
      { 
        Tick (); 
      }
     
    }
    return (int)msg.wParam;
  }

  bool Engine::Init (HINSTANCE hInstance, int nCmdShow)
  {
    if(!Engine::InitWindow (hInstance, nCmdShow))
      return false;

    return true;
  }

  bool Engine::InitWindow (HINSTANCE hInstance, int nCmdShow)
  {
    const wchar_t CLASS_NAME [] = L"UmbraWindowClass";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof (WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.lpfnWndProc    = WndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = hInstance;
    wc.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UMBERENGINE));
    wc.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 3);
    wc.lpszMenuName   = MAKEINTRESOURCEW(IDC_UMBERENGINE);
    wc.hIconSm        = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW (&wc);
    m_hWnd = CreateWindowExW (0, CLASS_NAME, L"Umbra Engine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 1280, 720, 0, 0, nullptr, hInstance,  nullptr);
    if(!m_hWnd)
      return false;

    ShowWindow (m_hWnd, nCmdShow);
    UpdateWindow (m_hWnd);
    
    return true;
  }

  void Engine::Tick ()
  {
    EngineTimer timer;
    float dt = timer.DeltaTime ();

    mCameraController.Update (dt);
    mCamera.UpdateViewMatrix ();

  }
}