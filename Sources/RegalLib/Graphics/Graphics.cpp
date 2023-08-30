#include "Graphics.h"
#include "../Other/Misc.h"

#include <string>

//GPU�����擾
void AcquireHighPerformanceAdapter(IDXGIFactory6* dxgiFactory6, IDXGIAdapter3** dxgiAdapter3)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDXGIAdapter3> enumeratedAdapter;
	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory6->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(enumeratedAdapter.ReleaseAndGetAddressOf())); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		hr = enumeratedAdapter->GetDesc1(&adapterDesc);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//VenterID �e��Ɩ��ɔ��s������ƌŗL��ID
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

	//GPU�̏����擾
	*dxgiAdapter3 = enumeratedAdapter.Detach();
}

namespace Regal::Graphics
{
    Graphics::Graphics(HWND hwnd, BOOL fullscreen):
		hwnd(hwnd),
		fullscreenMode(fullscreen),
		windowedStyle(static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE)))
    {
        //�C���X�^���X�ݒ�
        _ASSERT_EXPR(instance == nullptr, "already instantiated");
        instance = this;

        //��ʃT�C�Y�擾
        RECT rc;
        GetClientRect(hwnd, &rc);
        UINT screenWidth = rc.right - rc.left;
        UINT screenHeight = rc.bottom - rc.top;

        this->framebufferDimensions.cx = static_cast<float>(screenWidth);
        this->framebufferDimensions.cy = static_cast<float>(screenHeight);

        HRESULT hr{ S_OK };

		UINT createFactoryFlags{};
#ifdef _DEBUG
		createFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#else

#endif // _DEBUG
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
		hr = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(dxgiFactory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//�A�_�v�^�[�œK���i����GPU�ɑΉ��A��[�������j
		AcquireHighPerformanceAdapter(dxgiFactory6.Get(), adapter.GetAddressOf());

		UINT createDeviceFlags{ 0 };
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		/*D3D_FEATURE_LEVEL featureLevels[]
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};
		D3D_FEATURE_LEVEL featureLevel;
		hr = D3D11CreateDevice(
			adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0,
			createDeviceFlags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
			device.ReleaseAndGetAddressOf(), &featureLevel, immediateContext.ReleaseAndGetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		_ASSERT_EXPR(!(featureLevel < D3D_FEATURE_LEVEL_11_0), L"Direct3D Feature Level 11 unsupported.");*/
		D3D_FEATURE_LEVEL featureLevels{ D3D_FEATURE_LEVEL_11_1, };
		hr = D3D11CreateDevice(
			adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0,
			createDeviceFlags, &featureLevels, 1, D3D11_SDK_VERSION,
			device.ReleaseAndGetAddressOf(), NULL, immediateContext.ReleaseAndGetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		CreateSwapChain(dxgiFactory6.Get());
    }

    void Graphics::CreateSwapChain(IDXGIFactory6* dxgiFactory6)
    {
		HRESULT hr{ S_OK };

		if (swapChain)
		{
			//�E�B���h�E�T�C�Y�ύX����
			ID3D11RenderTargetView* nullRenderTargetView{};
			immediateContext->OMSetRenderTargets(1, &nullRenderTargetView, NULL);
			renderTargetView.Reset();

			DXGI_SWAP_CHAIN_DESC swapChainDesc{};
			swapChain->GetDesc(&swapChainDesc);
			hr = swapChain->ResizeBuffers(
				swapChainDesc.BufferCount,
				framebufferDimensions.cx, framebufferDimensions.cy,
				swapChainDesc.BufferDesc.Format, swapChainDesc.Flags
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
			hr = swapChain->GetBuffer(0, IID_PPV_ARGS(renderTargetBuffer.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			D3D11_TEXTURE2D_DESC texture2dDesc;
			renderTargetBuffer->GetDesc(&texture2dDesc);

			hr = device->CreateRenderTargetView(renderTargetBuffer.Get(), NULL, renderTargetView.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			BOOL allowTearing = FALSE;
			if (SUCCEEDED(hr))
			{
				hr = dxgiFactory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
			}
			tearingSupported = SUCCEEDED(hr) && allowTearing;

			DXGI_SWAP_CHAIN_DESC1 swapChainDesk1{};
			swapChainDesk1.Width = framebufferDimensions.cx;
			swapChainDesk1.Height = framebufferDimensions.cy;
			swapChainDesk1.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesk1.Stereo = FALSE;
			swapChainDesk1.SampleDesc.Count = 1;
			swapChainDesk1.SampleDesc.Quality = 0;
			swapChainDesk1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesk1.BufferCount = 2;
			swapChainDesk1.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesk1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesk1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			swapChainDesk1.Flags = tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
			hr = dxgiFactory6->CreateSwapChainForHwnd(device.Get(), hwnd, &swapChainDesk1, nullptr, nullptr, swapChain.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//�W���̃t���X�N���[������ alt+enter �𖳌��ɂ��Ă���
			dxgiFactory6->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
			hr = swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer{};
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = framebufferDimensions.cx;
		texture2dDesc.Height = framebufferDimensions.cy;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, NULL, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(framebufferDimensions.cx);
		viewport.Height = static_cast<float>(framebufferDimensions.cy);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediateContext->RSSetViewports(1, &viewport);
    }

	void Graphics::FullscreenState(BOOL fullscreen)
	{
		fullscreenMode = fullscreen;
		if (fullscreen)
		{
			GetWindowRect(hwnd, &windowedRect);

			//�E�B���h�E�X�^�C���̎w��
			SetWindowLongPtrA(
				hwnd, GWL_STYLE,
				WS_OVERLAPPEDWINDOW &
				~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME)
			);//WS_OVERLAPP...~...WS_CHICKFRAME)�� WS_OVERLAPPED �̂��ƁH

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
				hwnd,
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

			ShowWindow(hwnd, SW_MAXIMIZE);
		}
		else
		{
			SetWindowLongPtrA(hwnd, GWL_STYLE, windowedStyle);
			SetWindowPos(
				hwnd,
				HWND_NOTOPMOST,
				windowedRect.left,
				windowedRect.top,
				windowedRect.right - windowedRect.left,
				windowedRect.bottom - windowedRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);

			ShowWindow(hwnd, SW_NORMAL);
		}
	}

	void Graphics::OnSizeChanged(UINT64 width, UINT height)
	{
		//�ŏ����Ή�
		if (width == 0 || height == 0)return;

		HRESULT hr{ S_OK };
		if (width != framebufferDimensions.cx || height != framebufferDimensions.cy)
		{
			framebufferDimensions.cx = static_cast<LONG>(width);
			framebufferDimensions.cy = height;

			Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
			hr = swapChain->GetParent(IID_PPV_ARGS(dxgiFactory6.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			CreateSwapChain(dxgiFactory6.Get());
		}
	}
}

