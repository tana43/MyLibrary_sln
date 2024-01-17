#pragma once
#include "MathHelper.h"
#include <string>

namespace Regal::Game
{
    class Transform
    {
    public:
        Transform(const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0, 0, 0),
            const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(1, 1, 1),
            const DirectX::XMFLOAT3& rotation = DirectX::XMFLOAT3(0, 0, 0),
            float scaleFactor = 1.0f,
            int coordinateSystem = 1)
            : position_(position), scale_(scale), rotation_(rotation), scaleFactor_(scaleFactor), coordinateSystem_(coordinateSystem)
        {}
        ~Transform() = default;
        Transform(const Transform&) = delete;
        Transform(const Transform&&) = delete;
        Transform& operator=(const Transform&) = delete;
        Transform& operator=(const Transform&&) = delete;

        virtual void DrawDebug();
        virtual void DrawDebugPosAndRotOnly();//���W�Ɖ�]�݂̂̃f�o�b�O�\��
        void Reset();

        DirectX::XMMATRIX CalcWorldMatrix();

        DirectX::XMFLOAT3 GetPosition() const { return position_; }
        DirectX::XMFLOAT3 GetScale() const { return scale_; }
        DirectX::XMFLOAT3 GetRotation() const { return rotation_; }
        float GetScaleFactor() const { return scaleFactor_; }

        void SetPosition(const DirectX::XMFLOAT3& pos) { position_ = pos; }
        void SetPositionX(const float posX) { position_.x = posX; }
        void SetPositionY(const float posY) { position_.y = posY; }
        void SetPositionZ(const float posZ) { position_.z = posZ; }
        void AddPosition(const DirectX::XMFLOAT3& pos) { position_ = position_ + pos; }

        void SetScale(const DirectX::XMFLOAT3& scl) { scale_ = scl; }

        void SetRotation(const DirectX::XMFLOAT3& rot) { rotation_ = rot; }
        void SetRotationY(const float rotY) { rotation_.y = rotY; }
        void AddRotation(const DirectX::XMFLOAT3& rot) { rotation_ = rotation_ + rot; }
        void AddRotationY(const float rotY) { rotation_.y += rotY; }

        void SetScaleFactor(const float scale) { scaleFactor_ = scale; }
        void AddScaleFactor(const float scale) { scaleFactor_ += scale; }


        //���K���ςݑO�x�N�g����Ԃ�
        virtual DirectX::XMFLOAT3 CalcForward() const;
        //���K���ςݏ�x�N�g����Ԃ�
        virtual DirectX::XMFLOAT3 CalcUp() const;
        //���K���ς݉E�x�N�g����Ԃ�
        virtual DirectX::XMFLOAT3 CalcRight() const;

    protected:
        DirectX::XMFLOAT3 position_;
        DirectX::XMFLOAT3 scale_;
        DirectX::XMFLOAT3 rotation_;
        float scaleFactor_;
        int coordinateSystem_;

        static const int MAX_COORDINATE_SYSTEM = 4;
        const DirectX::XMFLOAT4X4 coordinateSystemTransforms_[MAX_COORDINATE_SYSTEM]{
            { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, // 0:RHS Y-UP
            { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, // 1:LHS Y-UP
            { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 }, // 2:RHS Z-UP
            { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 }, // 3:LHS Z-UP
        };

        const std::string coordinateSystemName_[MAX_COORDINATE_SYSTEM]
        {
            "Right Hand, Y up",
            "Left Hand, Y up",
            "Right Hand, Z up",
            "Left Hand, Z up"
        };

        bool ImGuiCoordinateComboUI();

        virtual DirectX::XMMATRIX MatrixRotation() = 0;
    };

    //�I�C���[�p�ɂ���]������g����Transform
    class TransformEuler final : public Transform
    {
    public:
        TransformEuler(const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0, 0, 0),
            const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(1, 1, 1),
            const DirectX::XMFLOAT3& rotation = DirectX::XMFLOAT3(0, 0, 0),
            float scaleFactor = 1.0f,
            int coordinateSystem = 1) :
            Transform(position, scale, rotation, scaleFactor, coordinateSystem)
        {}
        ~TransformEuler() {}

    private:
        DirectX::XMMATRIX MatrixRotation() override;
    };

    //�N�H�[�^�j�I���ɂ���]������g����Transform
    //todo:�Ȃ񂩏o����񂩂���
    class TransformQuaternion final : public Transform
    {
    public:
        TransformQuaternion(const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0, 0, 0),
            const DirectX::XMFLOAT3& scale = DirectX::XMFLOAT3(1, 1, 1),
            const DirectX::XMFLOAT3& rotation = DirectX::XMFLOAT3(0, 0, 0),
            float scaleFactor = 1.0f,
            int coordinateSystem = 1);
        ~TransformQuaternion() {}

        void DrawDebug() override;

        //���K���ςݑO�x�N�g����Ԃ�
        DirectX::XMFLOAT3 CalcForward()const override;
        //���K���ςݏ�x�N�g����Ԃ�
        DirectX::XMFLOAT3 CalcUp()const override;
        //���K���ς݉E�x�N�g����Ԃ�
        DirectX::XMFLOAT3 CalcRight()const override;

    private:
        DirectX::XMMATRIX MatrixRotation() override;

        DirectX::XMFLOAT4 orientation_;

        float angle_;
    };
}