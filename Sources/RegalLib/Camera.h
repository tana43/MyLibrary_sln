#pragma once
#include <DirectXMath.h>
#include "Transform.h"

namespace Regal::Game
{
    class Camera
    {
    private:
        Camera() {}
        ~Camera() {}

    public:
        //�J�����ɕK�v�Ȑ��l�A���̒��̕ϐ������炷���Ƃ͖���
        struct Parameters
        {
            float width_;
            float height_;
            float nearZ_ = 0.1f;
            float farZ_  = 500;
            float fov_   = 60;
            float aspectRatio_;
        };

        static Camera& Instance()
        {
            static Camera instance_;
            return instance_;
        }

        void Initialize();

        void Update(const float& elapsedTime);
        void UpdateViewProjectionMatrix();

        //�f�o�b�O�p�̃J��������
        void DebugCameraMovement(const float& elapsedTime);

        DirectX::XMMATRIX CalcViewMatrix() const;
        DirectX::XMMATRIX CalcProjectionMatrix() const;

        //�V�[���p�o�b�t�@�ɕK�v�ȍ��ڂ����Q�b�^�[������B���ƂŕύX���邩��
        const DirectX::XMMATRIX GetViewProjectionMatrix() const { return ViewProjection_; }
        const DirectX::XMFLOAT4 GetPosition() const 
        {
            return DirectX::XMFLOAT4(transform_.GetPosition().x, transform_.GetPosition().y, transform_.GetPosition().z, 1.0f); 
        }

        void DrawDebug();

    private:
        DirectX::XMMATRIX ViewProjection_{};

        TransformEuler transform_;
        Parameters parameters_;

        float debugCameraRollSpeed_{0.2f};
        float debugCameraMoveSpeed_{ 1.0f };
    };
}
