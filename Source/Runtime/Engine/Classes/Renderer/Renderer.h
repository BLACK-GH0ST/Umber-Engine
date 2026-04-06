#pragma once

/**********************************************************************************************
*
*  File Name   : Renderer.h
*
*  Description :
*      Minimal Direct3D11 renderer capable of clearing the screen and drawing a world grid
*      as colored lines using the current camera view/projection.
*
**********************************************************************************************/

#include <d3d11.h>
#include <wrl/client.h>

class Camera;

class Renderer
{
public:
	Renderer () = default;
	~Renderer () = default;

	bool Init (HWND hwnd, int backbufferWidth, int backbufferHeight);

	void BeginFrame (float r = 0.05f, float g = 0.05f, float b = 0.08f, float a = 1.0f);
	void EndFrame ();

	void DrawGrid (const Camera& camera);

private:
	struct VSConstants
	{
		float viewProj[16];
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVSConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mGridVB;
	UINT mGridVertexCount = 0;
};

