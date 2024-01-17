#pragma once

//0 : DirectX�W���t���X�N���[���i�ł��ĂȂ��j 1 : ����搶
#define FULLSCREEN_VARSION 1

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <dxgi1_6.h>

//mutex:�X���b�h�ԂŎg�p���鋤�L���\�[�X��r�����䂷�邽�߂̃N���X�ł���B
//�܂蓯�����\�[�X�ɑ΂��镡���̍X�V�����ɂ���ăf�[�^�̐����������Ȃ��Ȃ邱�Ƃ�h���ł���
#include <mutex>

#include "Shader.h"

#if 1
CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
#else
CONST LONG SCREEN_WIDTH{ 1920 };
CONST LONG SCREEN_HEIGHT{ 1080 };
#endif // 1

namespace Regal::Graphics
{
	enum DEPTH_STATE { ZT_ON_ZW_ON, ZT_OFF_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_OFF };
	enum BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY };
	enum RASTER_STATE { SOLID, WIREFRAME, WIREFRAME_CULL_NONE, SOLID_REVERSE, CULL_NONE };

	class Graphics
	{
	public:
		Graphics(HWND hwnd,BOOL fullscreen);
		~Graphics();

		//�V���O���g�������ǊO�����琶������K�v������̂ŃR���X�g���N�^��public
		//�ɂ���K�v������@�L���C
		static Graphics& Instance() { return *instance_; }

		// �f�o�C�X�擾
		ID3D11Device* GetDevice() const { return device_.Get(); }

		// �f�o�C�X�R���e�L�X�g�擾
		ID3D11DeviceContext* GetDeviceContext() const { return immediateContext_.Get(); }

		// �X���b�v�`�F�[���擾
		IDXGISwapChain* GetSwapChain() const { return swapChain_.Get(); }

		// �����_�[�^�[�Q�b�g�r���[�擾
		ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView_.Get(); }

		// �f�v�X�X�e���V���r���[�擾
		ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView_.Get(); }

		// �V�F�[�_�[�擾
		Regal::Resource::Shader* GetShader() const { return shader_.get(); }

		// �X�N���[�����擾
		float GetScreenWidth() const { return framebufferDimensions_.cx; }

		// �X�N���[�������擾
		float GetScreenHeight() const { return framebufferDimensions_.cy; }

		//�~���[�e�b�N�X�擾(�r������@�Ώۂ����g�p���g�p������\��)
		std::mutex& GetMutex() { return mutex_; }

		void OnSizeChanged(UINT64 width, UINT height);

		//�e��X�e�[�g�̐���
		void CreateStates();

		//�e��X�e�[�g���Z�b�g
		void SetStates(int depthStencilState, int rastarizeState, int blendState);

		//2D�p�̃X�e�[�g���Z�b�g
		void Set2DStates();
		//3D�p�̃X�e�[�g���Z�b�g
		void Set3DStates();

		void DrawDebug();

		void FullscreenState(BOOL fullscreen);

		//�Ƃ肠���������悤�ɂ���������T���v���[�X�e�[�g�o�C���h�֐��u���Ƃ��i��ŏ����j
		void BindSamplersState()
		{
			immediateContext_->PSSetSamplers(0, 1, samplerStates_[0].GetAddressOf());
			immediateContext_->PSSetSamplers(1, 1, samplerStates_[1].GetAddressOf());
			immediateContext_->PSSetSamplers(2, 1, samplerStates_[2].GetAddressOf());
		}

	public:
		

		BOOL fullscreenMode_;
#if _DEBUG
		BOOL vsync_{ FALSE };//��������
		BOOL tearingSupported_{ TRUE };
#else
		BOOL vsync_{ TRUE };//��������
		BOOL tearingSupported_{ FALSE };
#endif // _DEBUG


	private:
		void CreateSwapChain(IDXGIFactory6* dxgiFactory6);

		size_t VideoMemoryUsage()
		{
			DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
			adapter_->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);
			return videoMemoryInfo.CurrentUsage / 1024 / 1024;
		}

	private:
		static Graphics* instance_;

		Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter_;//�������̎g�p�󋵂̊m�F���ł���


		Microsoft::WRL::ComPtr<ID3D11Device>			device_;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext_;

#if FULLSCREEN_VARSION
		Microsoft::WRL::ComPtr<IDXGISwapChain1>			swapChain_;
#else
		Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain_;
#endif // 

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView_;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView_;

		std::unique_ptr<Regal::Resource::Shader>		shader_;
		//std::unique_ptr<DebugRenderer>					debugRenderer;
		//std::unique_ptr<LineRenderer>					lineRenderer;
		//std::unique_ptr<ImGuiRenderer>					imguiRenderer;

		//�e��X�e�[�g
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates_[3];

		

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates_[4];
		ID3D11DepthStencilState* setting2DDepthStencilState_{ depthStencilStates_[ZT_ON_ZW_ON].Get() };
		ID3D11DepthStencilState* setting3DDepthStencilState_{ depthStencilStates_[ZT_ON_ZW_ON].Get() };

		Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates_[4];
		ID3D11BlendState* setting2DBlendState_{ blendStates_[ALPHA].Get() };
		ID3D11BlendState* setting3DBlendState_{ blendStates_[ALPHA].Get() };

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates_[5];
		ID3D11RasterizerState* setting2DRasterizerState_{ rasterizerStates_[SOLID].Get() };
		ID3D11RasterizerState* setting3DRasterizerState_{ rasterizerStates_[SOLID].Get() };

		CONST HWND hwnd_;
		SIZE framebufferDimensions_{SCREEN_WIDTH,SCREEN_HEIGHT};//�X�N���[���T�C�Y

		RECT windowedRect_;
		DWORD windowedStyle_;

		std::mutex mutex_;
	};
}


