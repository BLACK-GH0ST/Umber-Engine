#include "../../Classes/Renderer/Renderer.h"
#include "../../Classes/Camera/Camera.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

struct GridVertex
{
	XMFLOAT3 pos;
	XMFLOAT4 col;
};

static void BuildGridVertices (std::vector<GridVertex>& outVerts, int halfLines, float spacing)
{
	outVerts.clear ();
	outVerts.reserve ((halfLines * 4 + 6));

	for(int i = -halfLines; i <= halfLines; ++i)
	{
		float z = i * spacing;
		float x = i * spacing;

		XMFLOAT4 col = (i == 0) ? XMFLOAT4 (1, 0, 0, 1) : XMFLOAT4 (0.25f, 0.25f, 0.3f, 1); // X-axis red
		outVerts.push_back ({ XMFLOAT3 (-halfLines * spacing, 0, z), col });
		outVerts.push_back ({ XMFLOAT3 ( halfLines * spacing, 0, z), col });

		col = (i == 0) ? XMFLOAT4 (0, 0, 1, 1) : XMFLOAT4 (0.25f, 0.25f, 0.3f, 1); // Z-axis blue
		outVerts.push_back ({ XMFLOAT3 (x, 0, -halfLines * spacing), col });
		outVerts.push_back ({ XMFLOAT3 (x, 0,  halfLines * spacing), col });
	}

	// Add Y-axis (vertical) in green through the origin
	outVerts.push_back ({ XMFLOAT3 (0, -halfLines * spacing, 0), XMFLOAT4 (0, 1, 0, 1) });
	outVerts.push_back ({ XMFLOAT3 (0,  halfLines * spacing, 0), XMFLOAT4 (0, 1, 0, 1) });
}

bool Renderer::Init (HWND hwnd, int backbufferWidth, int backbufferHeight)
{
	UINT flags = 0;
#if defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = backbufferWidth;
	sd.BufferDesc.Height = backbufferHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL featureLevels [] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	D3D_FEATURE_LEVEL obtained = {};
	if(FAILED (D3D11CreateDeviceAndSwapChain (
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		featureLevels, _countof (featureLevels), D3D11_SDK_VERSION,
		&sd, mSwapChain.ReleaseAndGetAddressOf(), mDevice.ReleaseAndGetAddressOf(),
		&obtained, mContext.ReleaseAndGetAddressOf())))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	if(FAILED (mSwapChain->GetBuffer (0, __uuidof (ID3D11Texture2D), (void**)backBuffer.ReleaseAndGetAddressOf ())))
		return false;
	if(FAILED (mDevice->CreateRenderTargetView (backBuffer.Get (), nullptr, mRTV.ReleaseAndGetAddressOf ())))
		return false;

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<FLOAT> (backbufferWidth);
	viewport.Height = static_cast<FLOAT> (backbufferHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	mContext->RSSetViewports (1, &viewport);

	// Compile and create shaders
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob, errBlob;
	// vertex shader of main map area 
	HRESULT hr;
	hr = D3DCompileFromFile (L"Assets/Shaders/Color_Axis.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob, &errBlob);
	if (FAILED (hr))
	{
		if(errBlob)
		OutputDebugStringA ((char*)errBlob->GetBufferPointer ());
		return false;
	}
	// Pixel Shader on main map area
	hr = D3DCompileFromFile (L"Assets/Shaders/Color_Axis.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob, &errBlob);
	if(FAILED (hr))
	{
		if(errBlob)
			OutputDebugStringA ((char*)errBlob->GetBufferPointer ());
		return false;
	}
	if(FAILED (mDevice->CreateVertexShader (vsBlob->GetBufferPointer (), vsBlob->GetBufferSize (), nullptr, mVertexShader.ReleaseAndGetAddressOf ())))
		return false;
	if(FAILED (mDevice->CreatePixelShader (psBlob->GetBufferPointer (), psBlob->GetBufferSize (), nullptr, mPixelShader.ReleaseAndGetAddressOf ())))
		return false;

	// Input layout
	D3D11_INPUT_ELEMENT_DESC layout [] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (GridVertex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof (GridVertex, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	if(FAILED (mDevice->CreateInputLayout (layout, _countof (layout), vsBlob->GetBufferPointer (), vsBlob->GetBufferSize (), mInputLayout.ReleaseAndGetAddressOf ())))
		return false;

	// Constant buffer
	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.ByteWidth = sizeof (VSConstants);
	if(FAILED (mDevice->CreateBuffer (&cbd, nullptr, mVSConstantBuffer.ReleaseAndGetAddressOf ())))
		return false;

	// Build grid geometry
	std::vector<GridVertex> verts;
	BuildGridVertices (verts, 50, 1.0f);
	mGridVertexCount = (UINT)verts.size ();

	D3D11_BUFFER_DESC vbd = {};
	vbd.ByteWidth = UINT (verts.size () * sizeof (GridVertex));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = verts.data ();
	if(FAILED (mDevice->CreateBuffer (&vbd, &initData, mGridVB.ReleaseAndGetAddressOf ())))
		return false;

	return true;
}

void Renderer::BeginFrame (float r, float g, float b, float a)
{
	const float clear [4] = { r, g, b, a };
	mContext->OMSetRenderTargets (1, mRTV.GetAddressOf (), nullptr);
	mContext->ClearRenderTargetView (mRTV.Get (), clear);
}

void Renderer::EndFrame ()
{
	mSwapChain->Present (1, 0);
}

static void StoreFloat4x4 (float dst[16], const XMFLOAT4X4& m)
{
	const float* p = &m._11;
	for(int i = 0; i < 16; ++i) dst[i] = p[i];
}

void Renderer::DrawGrid (const Camera& camera)
{
	mContext->IASetInputLayout (mInputLayout.Get ());
	mContext->VSSetShader (mVertexShader.Get (), nullptr, 0);
	mContext->PSSetShader (mPixelShader.Get (), nullptr, 0);

	UINT stride = sizeof (GridVertex);
	UINT offset = 0;
	ID3D11Buffer* vb = mGridVB.Get ();
	mContext->IASetVertexBuffers (0, 1, &vb, &stride, &offset);
	mContext->IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// Update constant buffer with ViewProj
	XMMATRIX V = camera.GetView ();
	XMMATRIX P = camera.GetProj ();
	XMMATRIX VP = XMMatrixMultiply (V, P);
	XMFLOAT4X4 vpFloat4x4;
	XMStoreFloat4x4 (&vpFloat4x4, XMMatrixTranspose (VP));

	D3D11_MAPPED_SUBRESOURCE mapped = {};
	if(SUCCEEDED (mContext->Map (mVSConstantBuffer.Get (), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		VSConstants* c = reinterpret_cast<VSConstants*>(mapped.pData);
		StoreFloat4x4 (c->viewProj, vpFloat4x4);
		mContext->Unmap (mVSConstantBuffer.Get (), 0);
	}
	ID3D11Buffer* cb = mVSConstantBuffer.Get ();
	mContext->VSSetConstantBuffers (0, 1, &cb);

	mContext->Draw (mGridVertexCount, 0);
}

