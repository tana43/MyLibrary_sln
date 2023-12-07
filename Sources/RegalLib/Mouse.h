#pragma once

#include <Mouse.h>
#include <memory>

using MouseButton = char;

namespace Regal::Input
{
    class Mouse
    {
    private:
        Mouse();
        ~Mouse() {}

    public:
        static const MouseButton BTN_LEFT = 0;
        static const MouseButton BTN_MIDDLE = 1;
        static const MouseButton BTN_RIGHT = 2;

        static Mouse& Instance()
        {
            static Mouse instance;
            return instance;
        }

        void Update();

        //���ꂼ��̃}�E�X�̓��͏�Ԃ��������\���̂�Ԃ�(��:if(GetButtonState().Space))
        static DirectX::Mouse::State& GetButtonState() { return Instance().state; }

        //�L�[���������u�Ԃ��ǂ�����Ԃ�
        static const bool GetButtonDown(const MouseButton button);

        //�L�[�𗣂����u�Ԃ��ǂ�����Ԃ�
        static const bool GetButtonUp(const MouseButton button);

        const int GetPosX() const { return state.x; }
        const int GetPosY() const { return state.y; }

        //TODO:��őO��]�A���]�̂ǂ��炪���̒l��Ԃ����m�F����
        //�}�E�X�z�C�[���̉�]�l��Ԃ�
        const int GetScrollWheelValue() const { return state.scrollWheelValue; }

    private:
        //DirectX::Keyboard�N���X�𐶐����邽�߂̃|�C���^
        std::unique_ptr<DirectX::Mouse> mouse;

        //�{�^�����������u�ԁA�������u�Ԃ̔��菈�������Ă���Ă���
        DirectX::Mouse::ButtonStateTracker tracker;

        //�}�E�X�{�^���̓��͏�Ԃ�����Ă���
        DirectX::Mouse::State state;
    };
}

