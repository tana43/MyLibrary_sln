#pragma once

#include "../Input/Mouse.h"
#include "../Input/Keyboard.h"

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
        static Input& Instance()
        {
            static Input instance;
            return instance;
        }

        void Update();
    };
}

