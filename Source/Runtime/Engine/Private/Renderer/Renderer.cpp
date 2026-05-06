#include "../../Classes/Renderer/Renderer.h"
#include "../../Classes/Camera/Camera.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <Windows.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

static std::wstring NarrowToWide (const char* s, size_t len)
{
	std::wstring out;
	if(!s) return out;
	out.reserve (len);
	for(size_t i = 0; i < len && s[i] != '\0'; ++i)
		out.push_back ((wchar_t)(unsigned char)s[i]);
	return out;
}

static bool FileExistsW (const std::wstring& path)
{
	const DWORD a = GetFileAttributesW (path.c_str ());
	return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

static std::wstring NormalizePathW (const std::wstring& path)
{
	wchar_t buf[MAX_PATH] = {};
	const DWORD n = GetFullPathNameW (path.c_str (), MAX_PATH, buf, nullptr);
	if(n == 0 || n >= MAX_PATH) return path;
	return std::wstring (buf);
}

struct GridVertex
{
	XMFLOAT3 pos;
	XMFLOAT4 col;
	float isAxis;
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
	//outVerts.push_back ({ XMFLOAT3 (0, -halfLines * spacing, 0), XMFLOAT4 (0, 1, 0, 1) });
	//outVerts.push_back ({ XMFLOAT3 (0,  halfLines * spacing, 0), XMFLOAT4 (0, 1, 0, 1) });
}

static std::wstring GetExeDirectory ()
{
	wchar_t path[MAX_PATH] = {};
	DWORD len = GetModuleFileNameW (nullptr, path, MAX_PATH);
	if(len == 0 || len >= MAX_PATH) return L"";

	for(int i = (int)len - 1; i >= 0; --i)
	{
		if(path[i] == L'\\' || path[i] == L'/')
		{
			path[i + 1] = L'\0';
			break;
		}
	}
	return std::wstring (path);
}

static std::wstring ResolveShaderPath (std::wstring* triedPaths)
{
	// Prefer paths relative to the executable so running outside VS still works.
	// Walk upward from the exe folder looking for Assets/Shaders/Color_Axis.hlsl
	const std::wstring exeDir = GetExeDirectory ();
	if(!exeDir.empty ())
	{
		std::wstring dir = NormalizePathW (exeDir);
		for(int depth = 0; depth < 16; ++depth)
		{
			const std::wstring p = NormalizePathW (dir + L"Assets\\Shaders\\Color_Axis.hlsl");
			if(triedPaths)
			{
				*triedPaths += p;
				*triedPaths += L"\n";
			}
			if(FileExistsW (p)) return p;

			const std::wstring parent = NormalizePathW (dir + L"..\\");
			if(parent.size () >= dir.size () || parent == dir) break;
			dir = parent;
		}
	}

	const std::wstring cwdRel = NormalizePathW (L".\\Assets\\Shaders\\Color_Axis.hlsl");
	if(triedPaths)
	{
		*triedPaths += cwdRel;
		*triedPaths += L"\n";
	}
	if(FileExistsW (cwdRel)) return cwdRel;

	return NormalizePathW (L"Assets\\Shaders\\Color_Axis.hlsl");
}

bool Renderer::Init (HWND hwnd, int backbufferWidth, int backbufferHeight)
{
	m_lastInitError.clear ();

	if(backbufferWidth < 1) backbufferWidth = 1;
	if(backbufferHeight < 1) backbufferHeight = 1;

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
	HRESULT hrDevice = D3D11CreateDeviceAndSwapChain (
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		featureLevels, _countof (featureLevels), D3D11_SDK_VERSION,
		&sd, mSwapChain.ReleaseAndGetAddressOf (), mDevice.ReleaseAndGetAddressOf (),
		&obtained, mContext.ReleaseAndGetAddressOf ());
#if defined(_DEBUG)
	if(FAILED (hrDevice) && (flags & D3D11_CREATE_DEVICE_DEBUG))
	{
		// Debug device often fails if the optional Graphics Tools / SDK layer is not installed.
		flags &= ~D3D11_CREATE_DEVICE_DEBUG;
		mSwapChain.Reset ();
		mDevice.Reset ();
		mContext.Reset ();
		hrDevice = D3D11CreateDeviceAndSwapChain (
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
			featureLevels, _countof (featureLevels), D3D11_SDK_VERSION,
			&sd, mSwapChain.ReleaseAndGetAddressOf (), mDevice.ReleaseAndGetAddressOf (),
			&obtained, mContext.ReleaseAndGetAddressOf ());
	}
#endif
	if(FAILED (hrDevice))
	{
		wchar_t buf[160] = {};
		swprintf_s (buf, L"D3D11CreateDeviceAndSwapChain failed (HRESULT 0x%08X).", (unsigned)hrDevice);
		m_lastInitError = buf;
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	if(FAILED (mSwapChain->GetBuffer (0, __uuidof (ID3D11Texture2D), (void**)backBuffer.ReleaseAndGetAddressOf ())))
	{
		m_lastInitError = L"Swap chain GetBuffer failed.";
		return false;
	}
	if(FAILED (mDevice->CreateRenderTargetView (backBuffer.Get (), nullptr, mRTV.ReleaseAndGetAddressOf ())))
	{
		m_lastInitError = L"CreateRenderTargetView failed.";
		return false;
	}

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
	std::wstring tried;
	const std::wstring shaderPath = ResolveShaderPath (&tried);
	// vertex shader of main map area 
	HRESULT hr;
	hr = D3DCompileFromFile (shaderPath.c_str (), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob, &errBlob);
	if (FAILED (hr))
	{
		if(errBlob)
			OutputDebugStringA ((char*)errBlob->GetBufferPointer ());
		m_lastInitError = L"Vertex shader compile failed.\nFile: ";
		m_lastInitError += shaderPath;
		m_lastInitError += L"\nHRESULT: ";
		{
			wchar_t b[48] = {};
			swprintf_s (b, L"0x%08X\n", (unsigned)hr);
			m_lastInitError += b;
		}
		if(errBlob && errBlob->GetBufferPointer ())
			m_lastInitError += NarrowToWide ((const char*)errBlob->GetBufferPointer (), errBlob->GetBufferSize ());
		m_lastInitError += L"\nTried paths:\n";
		m_lastInitError += tried;
		return false;
	}
	// Pixel Shader on main map area
	hr = D3DCompileFromFile (shaderPath.c_str (), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob, &errBlob);
	if(FAILED (hr))
	{
		if(errBlob)
			OutputDebugStringA ((char*)errBlob->GetBufferPointer ());
		m_lastInitError = L"Pixel shader compile failed.\nFile: ";
		m_lastInitError += shaderPath;
		m_lastInitError += L"\nHRESULT: ";
		{
			wchar_t b[48] = {};
			swprintf_s (b, L"0x%08X\n", (unsigned)hr);
			m_lastInitError += b;
		}
		if(errBlob && errBlob->GetBufferPointer ())
			m_lastInitError += NarrowToWide ((const char*)errBlob->GetBufferPointer (), errBlob->GetBufferSize ());
		m_lastInitError += L"\nTried paths:\n";
		m_lastInitError += tried;
		return false;
	}
	if(FAILED (mDevice->CreateVertexShader (vsBlob->GetBufferPointer (), vsBlob->GetBufferSize (), nullptr, mVertexShader.ReleaseAndGetAddressOf ())))
	{
		m_lastInitError = L"CreateVertexShader failed.";
		return false;
	}
	if(FAILED (mDevice->CreatePixelShader (psBlob->GetBufferPointer (), psBlob->GetBufferSize (), nullptr, mPixelShader.ReleaseAndGetAddressOf ())))
	{
		m_lastInitError = L"CreatePixelShader failed.";
		return false;
	}

	// Input layout
	D3D11_INPUT_ELEMENT_DESC layout [] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (GridVertex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof (GridVertex, col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	if(FAILED (mDevice->CreateInputLayout (layout, _countof (layout), vsBlob->GetBufferPointer (), vsBlob->GetBufferSize (), mInputLayout.ReleaseAndGetAddressOf ())))
	{
		m_lastInitError = L"CreateInputLayout failed.";
		return false;
	}

	// Constant buffer
	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.ByteWidth = sizeof (VSConstants);
	if(FAILED (mDevice->CreateBuffer (&cbd, nullptr, mVSConstantBuffer.ReleaseAndGetAddressOf ())))
	{
		m_lastInitError = L"CreateBuffer (VS constant buffer) failed.";
		return false;
	}

	// Build grid geometry
	std::vector<GridVertex> verts;
	BuildGridVertices (verts, 500/* how many grid shows in a map*/, 1.0f);
	mGridVertexCount = (UINT)verts.size ();

	D3D11_BUFFER_DESC vbd = {};
	vbd.ByteWidth = UINT (verts.size () * sizeof (GridVertex));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = verts.data ();
	if(FAILED (mDevice->CreateBuffer (&vbd, &initData, mGridVB.ReleaseAndGetAddressOf ())))
	{
		m_lastInitError = L"CreateBuffer (grid vertex buffer) failed.";
		return false;
	}

	m_lastInitError.clear ();
	return true;
}

void Renderer::BeginFrame (float r, float g, float b, float a)
{
	const float clear [4] = {0.02f, 0.02f, 0.02f, 1.0f};
	mContext->OMSetRenderTargets (1, mRTV.GetAddressOf (), nullptr);
	mContext->ClearRenderTargetView (mRTV.Get (), clear);
	
	// when i start depth b
	//mContext->OMSetRenderTargets(1, mRTV.GetAddressOf(), mDepthStencilView.Get());
	//mContext->ClearDepthStencilView (mDepthStencilView.Get (), D3D11_CLEAR_DEPTH, 1.0f, 0);
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
		// Update constant buffer with fog and cameara Pos
		VSConstants* cb = reinterpret_cast<VSConstants*>(mapped.pData);
		DirectX::XMStoreFloat3 (&cb->cameraPos, camera.GetPosition ());

		cb->nearFog = 20.0f;
		cb->farFog = 30.0f;
		cb->fogColor = { 0.02f, 0.02f, 0.02f };

		StoreFloat4x4 (cb->viewProj, vpFloat4x4);
		mContext->Unmap (mVSConstantBuffer.Get (), 0);
	}
	ID3D11Buffer* cb = mVSConstantBuffer.Get ();
	mContext->VSSetConstantBuffers (0, 1, &cb);

	mContext->Draw (mGridVertexCount, 0);
}

