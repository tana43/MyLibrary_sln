#pragma once

#include "Mouse.h"
#include "Keyboard.h"
#include "GamePad.h"

using Device = unsigned int;

namespace Regal::Input
{
    //���̃N���X����e����͂̃A�b�v�f�[�g������
    //���͔�����e�A�N�V�������ƂɌ��߂����Ƃ��������Ŋ֐���`����
    class Input
    {
    private:
        Input();
        ~Input() {}

    public:
        static const Device KEYBOARD = 0;
        static const Device GAMEPAD  = 1;

    public:

        static Input& Instance()
        {
            static Input instance_;
            return instance_;
        }

        void Update();

        static GamePad& GetGamePad() { return Instance().gamePad_; }

    private:
        GamePad gamePad_;

        Device useDevice_;
    };
}

