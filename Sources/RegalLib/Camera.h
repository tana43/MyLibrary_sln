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
        //カメラに必要な数値、この中の変数を減らすことは無い
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

        //デバッグ用のカメラ挙動
        void DebugCameraMovement(const float& elapsedTime);

        DirectX::XMMATRIX CalcViewMatrix() const;
        DirectX::XMMATRIX CalcProjectionMatrix() const;

        //シーン用バッファに必要な項目だけゲッター作った。あとで変更するかも
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
