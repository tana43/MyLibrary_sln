#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <dxgi1_6.h>

//mutex:�X���b�h�ԂŎg�p���鋤�L���\�[�X��r�����䂷�邽�߂̃N���X�ł���B
//�܂蓯�����\�[�X�ɑ΂��镡���̍X�V�����ɂ���ăf�[�^�̐����������Ȃ��Ȃ邱�Ƃ�h���ł���
#include <mutex>

namespace Regal::Graphics
{
	class Graphics
	{
	public:
		Graphics(HWND hwnd,BOOL fullscreen);
		~Graphics();

		//�V���O���g�������ǊO�����琶������K�v������̂ŃR���X�g���N�^��public
		//�ɂ���K�v������@�L���C
		static Graphics& Instance() { return *instance; }

		// �f�o�C�X�擾
		ID3D11Device* GetDevice() const { return device.Get(); }

		// �f�o�C�X�R���e�L�X�g�擾
		ID3D11DeviceContext* GetDeviceContext() const { return immediateContext.Get(); }

		// �X���b�v�`�F�[���擾
		IDXGISwapChain* GetSwapChain() const { return swapChain.Get(); }

		// �����_�[�^�[�Q�b�g�r���[�擾
		ID3D11RenderTargetView* GetRenderTargetView() const { return renderTargetView.Get(); }

		// �f�v�X�X�e���V���r���[�擾
		ID3D11DepthStencilView* GetDepthStencilView() const { return depthStencilView.Get(); }

		// �V�F�[�_�[�擾
		//Shader* GetShader() const { return shader.get(); }

		// �X�N���[�����擾
		float GetScreenWidth() const { return framebufferDimensions.cx; }

		// �X�N���[�������擾
		float GetScreenHeight() const { return framebufferDimensions.cy; }

		//�~���[�e�b�N�X�擾
		std::mutex& GetMutex() { return mutex; }

	private:
		void CreateSwapChain(IDXGIFactory6* dxgiFactory6);

		void FullscreenState(BOOL fullscreen);
		void OnSizeChanged(UINT64 width, UINT height);

	private:
		static Graphics* instance;

		Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter;//�������̎g�p�󋵂̊m�F���ł���

		Microsoft::WRL::ComPtr<ID3D11Device>			device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>			swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;

		/*std::unique_ptr<Shader>							shader;
		std::unique_ptr<DebugRenderer>					debugRenderer;
		std::unique_ptr<LineRenderer>					lineRenderer;
		std::unique_ptr<ImGuiRenderer>					imguiRenderer;*/

		CONST HWND hwnd;
		SIZE framebufferDimensions;

		BOOL fullscreenMode{ FALSE };
		BOOL vsync{ FALSE };//��������
		BOOL tearingSupported{ FALSE };

		RECT windowedRect;
		DWORD windowedStyle;

		std::mutex mutex;
	};
}


