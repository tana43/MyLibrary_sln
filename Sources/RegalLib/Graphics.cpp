#include "Graphics.h"
#include "Misc.h"
#include "../../External/imgui/imgui.h"

#include <string>

//GPU情報を取得
void AcquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDXGIAdapter3> enumeratedAdapter;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(enumeratedAdapter.ReleaseAndGetAddressOf())); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		hr = enumeratedAdapter->GetDesc1(&adapterDesc);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//VenterID 各企業毎に発行される企業固有のID
		if (adapterDesc.VendorId == 0x1002/*AMD*/ || adapterDesc.VendorId == 0x10DE/*NVIDIA*/)
		{
			OutputDebugStringW((std::wstring(adapterDesc.Description) + L" has been selected.\n").c_str());
			OutputDebugStringA(std::string("\tVendorId:" + std::to_string(adapterDesc.VendorId) + '\n').c_str());
			OutputDebugStringA(std::string("\tDeviceId:" + std::to_string(adapterDesc.DeviceId) + '\n').c_str());
			OutputDebugStringA(std::string("\tSubSysId:" + std::to_string(adapterDesc.SubSysId) + '\n').c_str());
			OutputDebugStringA(std::string("\tRevision:" + std::to_string(adapterDesc.Revision) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedVideoMemory:" + std::to_string(adapterDesc.DedicatedVideoMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedSystemMemory:" + std::to_string(adapterDesc.DedicatedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tSharedSystemMemory:" + std::to_string(adapterDesc.SharedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapterDesc.AdapterLuid.HighPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapterDesc.AdapterLuid.LowPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tFlags:" + std::to_string(adapterDesc.Flags) + '\n').c_str());
			break;
		}
	}

	//GPUの情報を取得
	*dxgiAdapter3 = enumeratedAdapter.Detach();
}

namespace Regal::Graphics
{
	Graphics* Graphics::instance_ = nullptr;

    Graphics::Graphics(HWND hwnd, BOOL fullscreen):
		hwnd_(hwnd),
		fullscreenMode_(fullscreen),
		windowedStyle_(static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE)))
    {
		//標準か福井先生のフルスクリーン切り替え
#if FULLSCREEN_VARSION
		//インスタンス設定
		_ASSERT_EXPR(instance_ == nullptr, "already instantiated");
		instance_ = this;

		if (fullscreen)
		{
			FullscreenState(TRUE);
		}

		//画面サイズ取得
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		framebufferDimensions_.cx = clientRect.right - clientRect.left;
		framebufferDimensions_.cy = clientRect.bottom - clientRect.top;

		HRESULT hr{ S_OK };

		UINT createFactoryFlags{};
#ifdef _DEBUG
		createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#else

#endif // _DEBUG
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
		hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(dxgiFactory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//アダプター最適化（搭載GPUに対応、よー分からん）
		AcquireHighPerformanceAdapter(dxgiFactory6.Get(), adapter_.GetAddressOf());

		UINT createDeviceFlags{ 0 };
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

#if 1
		D3D_FEATURE_LEVEL featureLevels[]
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};
		D3D_FEATURE_LEVEL featureLevel;
		hr = D3D11CreateDevice(
			adapter_.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0,
			createDeviceFlags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
			device_.ReleaseAndGetAddressOf(), &featureLevel, immediateContext_.ReleaseAndGetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		_ASSERT_EXPR(!(featureLevel < D3D_FEATURE_LEVEL_11_0), L"Direct3D Feature Level 11 unsupported.");
#else
		D3D_FEATURE_LEVEL featureLevels{ D3D_FEATURE_LEVEL_11_1, };
		hr = D3D11CreateDevice(
			adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0,
			createDeviceFlags, &featureLevels, 1, D3D11_SDK_VERSION,
			device.ReleaseAndGetAddressOf(), NULL, immediateContext_.ReleaseAndGetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif // 1

		CreateSwapChain(dxgiFactory6.Get());
#else
		HRESULT hr{ S_OK };

		
		UINT createDeviceFlags{ 0 };
		#ifdef _DEBUG
			createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif
		
		D3D_FEATURE_LEVEL featureLevels{ D3D_FEATURE_LEVEL_11_0 };
		
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = framebufferDimensions.cx;
		swapChainDesc.BufferDesc.Height = framebufferDimensions.cy;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hwnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Windowed = !fullscreen;
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
		&featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc,
		swapChain.GetAddressOf(), device.GetAddressOf(), NULL, immediateContext_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
		hr = swapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, &renderTargetView);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = framebufferDimensions.cx;
		texture2dDesc.Height = framebufferDimensions.cy;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, NULL, &depthStencilBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = texture2dDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(framebufferDimensions.cx);
		viewport.Height = static_cast<float>(framebufferDimensions.cy);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediateContext_->RSSetViewports(1, &viewport);

		swapChain->SetFullscreenState(fullscreen, 0);
#endif // 0

		//シェーダー
		{
			shader_ = std::make_unique<Regal::Resource::Shader>();
		}

		CreateStates();
    }

	Graphics::~Graphics()
	{
	}

    void Graphics::CreateSwapChain(IDXGIFactory6* dxgiFactory6)
    {
#if FULLSCREEN_VARSION
		HRESULT hr{ S_OK };

		if (swapChain_)
		{
			//ウィンドウサイズ変更処理
			ID3D11RenderTargetView* nullRenderTargetView{};
			immediateContext_->OMSetRenderTargets(1, &nullRenderTargetView, NULL);
			renderTargetView_.Reset();

			DXGI_SWAP_CHAIN_DESC swapChainDesc{};
			swapChain_->GetDesc(&swapChainDesc);
			hr = swapChain_->ResizeBuffers(
				swapChainDesc.BufferCount,
				framebufferDimensions_.cx, framebufferDimensions_.cy,
				swapChainDesc.BufferDesc.Format, swapChainDesc.Flags
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
			hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(renderTargetBuffer.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			D3D11_TEXTURE2D_DESC texture2dDesc;
			renderTargetBuffer->GetDesc(&texture2dDesc);

			hr = device_->CreateRenderTargetView(renderTargetBuffer.Get(), NULL, renderTargetView_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			BOOL allowTearing = FALSE;
			if (SUCCEEDED(hr))
			{
				hr = dxgiFactory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
			}
			tearingSupported_ = SUCCEEDED(hr) && allowTearing;

			DXGI_SWAP_CHAIN_DESC1 swapChainDesk1{};
			swapChainDesk1.Width = framebufferDimensions_.cx;
			swapChainDesk1.Height = framebufferDimensions_.cy;
			swapChainDesk1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesk1.Stereo = FALSE;
			swapChainDesk1.SampleDesc.Count = 1;
			swapChainDesk1.SampleDesc.Quality = 0;
			swapChainDesk1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesk1.BufferCount = 2;
			swapChainDesk1.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesk1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesk1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			swapChainDesk1.Flags = tearingSupported_ ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
			hr = dxgiFactory6->CreateSwapChainForHwnd(device_.Get(), hwnd_, &swapChainDesk1, nullptr, nullptr, swapChain_.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//標準のフルスクリーン入力 alt+enter を無効にしている
			hr = dxgiFactory6->MakeWindowAssociation(hwnd_, DXGI_MWA_NO_ALT_ENTER);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
			hr = swapChain_->GetBuffer(0,
				IID_PPV_ARGS(backBuffer.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			hr = device_->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView_.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = framebufferDimensions_.cx;
		texture2dDesc.Height = framebufferDimensions_.cy;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device_->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device_->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView_.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(framebufferDimensions_.cx);
		viewport.Height = static_cast<float>(framebufferDimensions_.cy);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediateContext_->RSSetViewports(1, &viewport);
#endif // FULLSCREEN_VAR

		

		CreateStates();
    }

	void Graphics::FullscreenState(BOOL fullscreen)
	{
#if FULLSCREEN_VARSION
		fullscreenMode_ = fullscreen;
		if (fullscreen)
		{
			GetWindowRect(hwnd_, &windowedRect_);

			//ウィンドウスタイルの指定
			SetWindowLongPtrA(
				hwnd_, GWL_STYLE,
				WS_OVERLAPPEDWINDOW &
				~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
			);//WS_OVERLAPP...~...WS_CHICKFRAME)は WS_OVERLAPPED のこと？

			RECT fullscreenWindowRect;

			{
				DEVMODE devmode{};
				devmode.dmSize = sizeof(DEVMODE);
				EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

				fullscreenWindowRect = {
					devmode.dmPosition.x,
					devmode.dmPosition.y,
					devmode.dmPosition.x + static_cast<LONG>(devmode.dmPelsWidth),
					devmode.dmPosition.y + static_cast<LONG>(devmode.dmPelsHeight),
				};
			}
			SetWindowPos(
				hwnd_,
#ifdef _DEBUG
				NULL,
#else
				HWND_TOPMOST,
#endif // _DEBUG
				fullscreenWindowRect.left,
				fullscreenWindowRect.top,
				fullscreenWindowRect.right,
				fullscreenWindowRect.bottom,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);

			ShowWindow(hwnd_, SW_MAXIMIZE);
		}
		else
		{
			SetWindowLongPtrA(hwnd_, GWL_STYLE, windowedStyle_);
			SetWindowPos(
				hwnd_,
				HWND_NOTOPMOST,
				windowedRect_.left,
				windowedRect_.top,
				windowedRect_.right - windowedRect_.left,
				windowedRect_.bottom - windowedRect_.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);

			ShowWindow(hwnd_, SW_NORMAL);
		}
#endif // FULLSCREEN_VAR

		
	}

	void Graphics::OnSizeChanged(UINT64 width, UINT height)
	{
#if FULLSCREEN_VARSION
		//最小化対応
		if (width == 0 || height == 0)return;

		HRESULT hr{ S_OK };
		if (width != framebufferDimensions_.cx || height != framebufferDimensions_.cy)
		{
			framebufferDimensions_.cx = static_cast<LONG>(width);
			framebufferDimensions_.cy = height;

			Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
			hr = swapChain_->GetParent(IID_PPV_ARGS(dxgiFactory6.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			CreateSwapChain(dxgiFactory6.Get());
		}
#else
		//最小化対応
		if (width == 0 || height == 0)return;

		HRESULT hr{ S_OK };
		if (width != framebufferDimensions.cx || height != framebufferDimensions.cy)
		{
			framebufferDimensions.cx = static_cast<LONG>(width);
			framebufferDimensions.cy = height;
		}
#endif // !FULLSCREEN_VAR
	}

	void Graphics::CreateStates()
	{

		HRESULT hr{ S_OK };

		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device_->CreateSamplerState(&samplerDesc, samplerStates_[0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = device_->CreateSamplerState(&samplerDesc, samplerStates_[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = device_->CreateSamplerState(&samplerDesc, samplerStates_[2].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//深度ステンシルステートオブジェクトの生成
		{
			//深度テスト：オン,深度ライト：オン
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[0].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//深度テスト：オフ,深度ライト：オン
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[1].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//深度テスト：オン,深度ライト：オフ
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[2].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//深度テスト：オフ,深度ライト：オフ
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[3].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		//ブレンディングステートオブジェクト作成
		{
			D3D11_BLEND_DESC blend_desc{};
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = FALSE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device_->CreateBlendState(&blend_desc, blendStates_[static_cast<size_t>(BLEND_STATE::NONE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device_->CreateBlendState(&blend_desc, blendStates_[static_cast<size_t>(BLEND_STATE::ALPHA)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //D3D11_BLEND_ONE D3D11_BLEND_SRC_ALPHA
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device_->CreateBlendState(&blend_desc, blendStates_[static_cast<size_t>(BLEND_STATE::ADD)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO; //D3D11_BLEND_DEST_COLOR
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR; //D3D11_BLEND_SRC_COLOR
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device_->CreateBlendState(&blend_desc, blendStates_[static_cast<size_t>(BLEND_STATE::MULTIPLY)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		//ラスタライザーステートオブジェクト生成
		{
			D3D11_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.FrontCounterClockwise = FALSE;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0;
			rasterizerDesc.SlopeScaledDepthBias = 0;
			rasterizerDesc.DepthClipEnable = TRUE;
			rasterizerDesc.ScissorEnable = FALSE;
			rasterizerDesc.MultisampleEnable = FALSE;
			rasterizerDesc.AntialiasedLineEnable = FALSE;
			hr = device_->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[0].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//面カリングを逆向きにするステート
			rasterizerDesc.FrontCounterClockwise = TRUE;
			hr = device_->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[3].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.AntialiasedLineEnable = TRUE;
			hr = device_->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[1].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			rasterizerDesc.AntialiasedLineEnable = TRUE;
			hr = device_->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[2].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//面カリング無し
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.CullMode = D3D11_CULL_NONE;
			rasterizerDesc.AntialiasedLineEnable = TRUE;
			hr = device_->CreateRasterizerState(&rasterizerDesc, rasterizerStates_[4].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		//各種ステートオブジェクトセット
		{
			setting2DDepthStencilState_ = depthStencilStates_[DEPTH_STATE::ZT_OFF_ZW_OFF].Get();
			setting3DDepthStencilState_ = depthStencilStates_[DEPTH_STATE::ZT_ON_ZW_ON].Get();
			setting2DRasterizerState_ = rasterizerStates_[RASTER_STATE::SOLID].Get();
			setting3DRasterizerState_ = rasterizerStates_[RASTER_STATE::SOLID].Get();
			setting2DBlendState_ = blendStates_[BLEND_STATE::ALPHA].Get();
			setting3DBlendState_ = blendStates_[BLEND_STATE::ALPHA].Get();
		}
	}

	void Graphics::SetStates(int depthStencilState, int rastarizerState, int blendState)
	{
		immediateContext_->RSSetState(rasterizerStates_[rastarizerState].Get());
		immediateContext_->OMSetDepthStencilState(depthStencilStates_[depthStencilState].Get(), 1);
		immediateContext_->OMSetBlendState(blendStates_[blendState].Get(), nullptr, 0xFFFFFFFF);
	}

	void Graphics::Set2DStates()
	{
		immediateContext_->OMSetDepthStencilState(setting2DDepthStencilState_, 1);
		immediateContext_->RSSetState(setting2DRasterizerState_);
		immediateContext_->OMSetBlendState(setting2DBlendState_, nullptr, 0xFFFFFFFF);
	}

	void Graphics::Set3DStates()
	{
		immediateContext_->OMSetDepthStencilState(setting3DDepthStencilState_, 1);
		immediateContext_->RSSetState(setting3DRasterizerState_);
		immediateContext_->OMSetBlendState(setting3DBlendState_, nullptr, 0xFFFFFFFF);
	}

	void Graphics::DrawDebug()
	{
		if (ImGui::BeginMenu("Graphics"))
		{
			if (ImGui::BeginMenu("2D"))
			{
				/*if (ImGui::TreeNode("SpriteColor"))
				{
					ImGui::ColorPicker4("color", spriteColors, ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					ImGui::TreePop();
				}*/

				static bool selectDFlag[4] = { false,false,false,true };
				if (ImGui::BeginMenu("DepthStencilState"))
				{
					if (ImGui::MenuItem("Z_Test ON  : Z_Write ON", "", selectDFlag[0]))
					{
						setting2DDepthStencilState_ = depthStencilStates_[0].Get();
						selectDFlag[0] = true; selectDFlag[1] = false; selectDFlag[2] = false; selectDFlag[3] = false;
					}
					if (ImGui::MenuItem("Z_Test OFF : Z_Write ON", "", selectDFlag[1]))
					{
						setting2DDepthStencilState_ = depthStencilStates_[1].Get();
						selectDFlag[0] = false; selectDFlag[1] = true; selectDFlag[2] = false; selectDFlag[3] = false;
					}
					if (ImGui::MenuItem("Z_Test ON  : Z_Write OFF", "", selectDFlag[2]))
					{
						setting2DDepthStencilState_ = depthStencilStates_[2].Get();
						selectDFlag[0] = false; selectDFlag[1] = false; selectDFlag[2] = true; selectDFlag[3] = false;
					}
					if (ImGui::MenuItem("Z_Test OFF : Z_Write OFF", "", selectDFlag[3]))
					{
						setting2DDepthStencilState_ = depthStencilStates_[3].Get();
						selectDFlag[0] = false; selectDFlag[1] = false; selectDFlag[2] = false; selectDFlag[3] = true;
					}

					ImGui::EndMenu();
				}


				static bool selectRFlag[3] = { true,false,false };
				if (ImGui::BeginMenu("RasterizerState"))
				{
					if (ImGui::MenuItem("Solid", "", selectRFlag[0]))
					{
						setting2DRasterizerState_ = rasterizerStates_[0].Get();
						selectRFlag[0] = true; selectRFlag[1] = false; selectRFlag[2] = false;
					}
					if (ImGui::MenuItem("Wireframe", "", selectRFlag[1]))
					{
						setting2DRasterizerState_ = rasterizerStates_[1].Get();
						selectRFlag[0] = false; selectRFlag[1] = true; selectRFlag[2] = false;
					}
					if (ImGui::MenuItem("Wireframe Culling Off", "", selectRFlag[2]))
					{
						setting2DRasterizerState_ = rasterizerStates_[2].Get();
						selectRFlag[0] = false; selectRFlag[1] = false; selectRFlag[2] = true;
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("3D"))
			{
				static bool selectDFlag[4] = { true,false,false,false };
				if (ImGui::BeginMenu("DepthStencilState"))
				{
					if (ImGui::MenuItem("Z_Test ON  : Z_Write ON", "", selectDFlag[0]))
					{
						setting3DDepthStencilState_ = depthStencilStates_[0].Get();
						selectDFlag[0] = true; selectDFlag[1] = false; selectDFlag[2] = false; selectDFlag[3] = false;
					}
					if (ImGui::MenuItem("Z_Test OFF : Z_Write ON", "", selectDFlag[1]))
					{
						setting3DDepthStencilState_ = depthStencilStates_[1].Get();
						selectDFlag[0] = false; selectDFlag[1] = true; selectDFlag[2] = false; selectDFlag[3] = false;
					}
					if (ImGui::MenuItem("Z_Test ON  : Z_Write OFF", "", selectDFlag[2]))
					{
						setting3DDepthStencilState_ = depthStencilStates_[2].Get();
						selectDFlag[0] = false; selectDFlag[1] = false; selectDFlag[2] = true; selectDFlag[3] = false;
					}
					if (ImGui::MenuItem("Z_Test OFF : Z_Write OFF", "", selectDFlag[3]))
					{
						setting3DDepthStencilState_ = depthStencilStates_[3].Get();
						selectDFlag[0] = false; selectDFlag[1] = false; selectDFlag[2] = false; selectDFlag[3] = true;
					}

					ImGui::EndMenu();
				}


				static bool selectRFlag[4] = { true,false,false,false };
				if (ImGui::BeginMenu("RasterizerState"))
				{
					if (ImGui::MenuItem("Solid", "", selectRFlag[0]))
					{
						setting3DRasterizerState_ = rasterizerStates_[0].Get();
						selectRFlag[0] = true; selectRFlag[1] = false; selectRFlag[2] = false; selectRFlag[3] = false;
					}
					if (ImGui::MenuItem("Wireframe", "", selectRFlag[1]))
					{
						setting3DRasterizerState_ = rasterizerStates_[1].Get();
						selectRFlag[0] = false; selectRFlag[1] = true; selectRFlag[2] = false; selectRFlag[3] = false;
					}
					if (ImGui::MenuItem("Wireframe Culling Off", "", selectRFlag[2]))
					{
						setting3DRasterizerState_ = rasterizerStates_[2].Get();
						selectRFlag[0] = false; selectRFlag[1] = false; selectRFlag[2] = true; selectRFlag[3] = false;
					}
					if (ImGui::MenuItem("Solid Reverse", "", selectRFlag[3]))
					{
						setting3DRasterizerState_ = rasterizerStates_[3].Get();
						selectRFlag[0] = false; selectRFlag[1] = false; selectRFlag[2] = false; selectRFlag[3] = true;
					}

					ImGui::EndMenu();
				}

				/*ImGui::DragFloat("BoneTranslation", &boneTranslationX);

				ImGui::SliderFloat("BlendAnimation", &blendAnimation, 0.0f, 1.0f);*/

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}
}

