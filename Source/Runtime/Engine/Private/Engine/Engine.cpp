
// CPP headers
#include "../../../../../Build/stdafx.h"
#include <memory>
#include <Windowsx.h>

#include "..\..\Classes\Engine/Engine.h"
#include "..\..\..\..\..\Build\Resource.h"
#include "../../Classes/Camera/Camera.h"

#define MAX_LOADSTRING 100

class CameraController;

HINSTANCE hInst;                               
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            

Umbra::Engine gEngineInstance;               

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_UMBRAENGINE, szWindowClass, MAX_LOADSTRING);
     
    // Initialize and run the engine instance

    if(!gEngineInstance.Init (hInstance, nCmdShow))
    {
      return -1;
    }
    return gEngineInstance.Run();
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
        } break;

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

        gEngineInstance.OnMouseMove (dx, dy);
      } break;

    case WM_KEYDOWN:
      gEngineInstance.OnKeyDown (wParam);
      break;

    case WM_KEYUP:
      gEngineInstance.OnKeyUp (wParam);
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

  bool Engine::Init (HINSTANCE hInstance, int nCmdShow)
  {
    if(!Engine::InitWindow (hInstance, nCmdShow))
      return false;
    // Initialize core runtime objects
    mCamera = new Camera();
    mCameraController = std::make_unique<CameraController> (mCamera);

    // Initialize renderer
    RECT rc;
    GetClientRect (m_hWnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    mRenderer = std::make_unique<Renderer> ();
    if(!mRenderer->Init (m_hWnd, width, height))
      return false;

    // Camera setup
    float aspect = (height != 0) ? (float)width / (float)height : 1.0f;
    mCamera->SetLens (.25f * MathHelper::Pi, aspect, 0.1f, 1000.0f);
    mCamera->SetPosition (0.0f, 2.0f, -5.0f);
    mCamera->LookAt ({ 0,2,-5 }, { 0,0,0 }, { 0,1,0 });
    mTimer.Reset ();

    return true;
  }


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

  void Engine::OnKeyDown (WPARAM wparam)
  {}

  void Engine::OnKeyUp (WPARAM wparam)
  {}
  
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
    wc.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UMBRAENGINE));
    wc.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 3);
    wc.lpszMenuName   = MAKEINTRESOURCEW(IDC_UMBRAENGINE);
    wc.hIconSm        = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW (&wc);
    m_hWnd = CreateWindowExW (0, CLASS_NAME, L"Umbra Engine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, nullptr, nullptr, hInstance,  nullptr);
    if(!m_hWnd)
      return false;

    ShowWindow (m_hWnd, nCmdShow);
    UpdateWindow (m_hWnd);
    
    return true;
  }

  void Engine::Update ()
  {}

  void Engine::Render ()
  {
    if(!mRenderer) return;
    mRenderer->BeginFrame ();
    mRenderer->DrawGrid (*mCamera);
    mRenderer->EndFrame ();
  }

  void Engine::Tick ()
  {
    mTimer.Tick ();
    float dt = mTimer.DeltaTime ();

    if(mCameraController)
      mCameraController->Update (dt);
    if(mCamera)
      mCamera->UpdateViewMatrix ();

    Render ();
  }

  void Engine::OnMouseMove (float dx, float dy)
  {
    if(mCameraController)
      mCameraController->OnMouseMove (dx, dy);
  }
}
