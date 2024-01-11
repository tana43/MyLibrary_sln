#include "Camera.h"
#include <d3d11.h>
#include "Graphics.h"

#include "../../../External/imgui/imgui.h"

#include "Input.h"

namespace Regal::Game
{
    void Camera::Initialize()
    {
		transform_.SetPosition(DirectX::XMFLOAT3(0, 0, -10));
		//transform.SetRotationY(DirectX::XMConvertToRadians(180));
    }

    void Camera::Update(const float& elapsedTime)
    {
#if _DEBUG
		DebugCameraMovement(elapsedTime);
#endif // _DEBUG

    }

	void Camera::UpdateViewProjectionMatrix()
	{
		D3D11_VIEWPORT viewport{};
		UINT numViewports{ 1 };
		auto immediateContext{
			Regal::Graphics::Graphics::Instance().GetDeviceContext()
		};
		immediateContext->RSGetViewports(&numViewports, &viewport);

		parameters_.width_ = viewport.Width;
		parameters_.height_ = viewport.Height;
		parameters_.aspectRatio_ = viewport.Width / viewport.Height;

		const DirectX::XMMATRIX P{CalcProjectionMatrix()};
		const DirectX::XMMATRIX V{CalcViewMatrix()};

		ViewProjection_ = V * P;
	}

	void Camera::DebugCameraMovement(const float& elapsedTime)
	{
		//回転	
		{
			//右クリックを押している間のみマウスでのカメラ回転を可能にする
			if (!Regal::Input::Mouse::Instance().GetButtonState().rightButton)return;

			auto& mouse = Regal::Input::Mouse::Instance();
			static DirectX::XMFLOAT2 lastMousePos = { static_cast<float>(mouse.GetPosX()),static_cast<float>(mouse.GetPosY()) };
			const DirectX::XMFLOAT2 mousePos = { static_cast<float>(mouse.GetPosX()),static_cast<float>(mouse.GetPosY()) };

			//前回とのカーソル座標の差でカメラが大きく回転することを防止する
			if (Regal::Input::Mouse::Instance().GetButtonDown(Regal::Input::Mouse::BTN_RIGHT))
			{
				lastMousePos = mousePos;
				return;
			}

			//マウスの移動量をカメラの回転値に変換
			const DirectX::XMFLOAT2 mousePosDistance = mousePos - lastMousePos;
			const float rotValueX = mousePosDistance.y * debugCameraRollSpeed_ * elapsedTime;
			const float rotValueY = mousePosDistance.x * debugCameraRollSpeed_ * elapsedTime;
			transform_.AddRotation(DirectX::XMFLOAT3(rotValueX, rotValueY, 0));

			lastMousePos = mousePos;
		}

		//移動
		{
			const auto& keyState = Regal::Input::Keyboard::Instance().GetKeyState();

			DirectX::XMFLOAT3 velocity{};
			if (keyState.W)
			{
				velocity = velocity + transform_.CalcForward();
			}
			if (keyState.A)
			{
				velocity = velocity - transform_.CalcRight();
			}
			if (keyState.S)
			{
				velocity = velocity - transform_.CalcForward();
			}
			if (keyState.D)
			{
				velocity = velocity + transform_.CalcRight();
			}

			if (keyState.LeftShift)velocity = velocity * 3.0f;

			transform_.AddPosition(velocity * debugCameraMoveSpeed_ * elapsedTime);
		}
	}

	DirectX::XMMATRIX Camera::CalcViewMatrix() const
    {
		DirectX::XMFLOAT3 forward = transform_.CalcForward();

		/*-----------------------------視点設定-------------------------------------*/
		DirectX::XMVECTOR Eye;

		DirectX::XMFLOAT4 eye
		{
			transform_.GetPosition().x,
			transform_.GetPosition().y,
			transform_.GetPosition().z,
			1.0f
		};
		
		Eye = DirectX::XMLoadFloat4(&eye);
		/*-----------------------------------------------------------------------*/

		/*---------------------------注視点設定-----------------------------------*/
		DirectX::XMVECTOR Focus;

		DirectX::XMFLOAT4 focus
		{
			eye.x + forward.x * 10.0f,
			eye.y + forward.y * 10.0f,
			eye.z + forward.z * 10.0f,
			1.0f
		};
		
		Focus = DirectX::XMLoadFloat4(&focus);
		/*-----------------------------------------------------------------------*/

		//Eye.m128_f32[3] = Focus.m128_f32[3] = 1.0f;

		DirectX::XMVECTOR Up{DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f)};

		//DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(0,1,0);
		//DirectX::XMVECTOR Up{DirectX::XMLoadFloat3(&up)};

		return DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
    }

    DirectX::XMMATRIX Camera::CalcProjectionMatrix() const
    {
        return DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(parameters_.fov_), 
			parameters_.aspectRatio_, 
			parameters_.nearZ_, 
			parameters_.farZ_
		);
    }

	//シーン用のImguiメニューに出す
	void Camera::DrawDebug()
	{
		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::TreeNode("Debug Movement"))
			{
				ImGui::DragFloat("MoveSpeed",&debugCameraMoveSpeed_,0.1f,0.01f);

				ImGui::TreePop();
			}

			ImGui::SliderFloat("Fov", &parameters_.fov_,1.0f,110.0f);
			ImGui::DragFloat("NearZ", &parameters_.nearZ_, 1.0f,0.1f);
			ImGui::DragFloat("FarZ", &parameters_.farZ_, 1.0f,0.1f);

			transform_.DrawDebugPosAndRotOnly();

			ImGui::InputFloat("Width",&parameters_.width_);
			ImGui::InputFloat("Height",&parameters_.height_);
			ImGui::InputFloat("AspectRatio",&parameters_.aspectRatio_);

			ImGui::EndMenu();
		}
	}
}
