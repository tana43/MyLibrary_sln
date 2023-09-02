#pragma once
#include <Keyboard.h>

using UseKey = unsigned int;

namespace Regal::Input
{
    //DirectXTK��Keyboard���g��
    class Keyboard
    {
    private:
        Keyboard();
        ~Keyboard() {}

    public:
        static Keyboard& Instance()
        {
            static Keyboard instance;
            return instance;
        }

        void Update();

        //���ꂼ��̃L�[�̓��͏�Ԃ��������\���̂�Ԃ�(��:if(GetKeyState().Space))
        DirectX::Keyboard::State& GetKeyState() { return state; }

        //�L�[���������u�Ԃ��ǂ�����Ԃ�
        const bool GetKeyUp(const DirectX::Keyboard::Keys key);

        //�L�[�𗣂����u�Ԃ��ǂ�����Ԃ�
        const bool GetKeyDown(const DirectX::Keyboard::Keys key);

    private:
        //DirectX::Keyboard�N���X�𐶐����邽�߂̃|�C���^
        std::unique_ptr<DirectX::Keyboard> keyboard;

        //�{�^�����������u�ԁA�������u�Ԃ̏��������Ă���Ă���
        DirectX::Keyboard::KeyboardStateTracker tracker;

        //�L�[�{�[�h�̓��͏�Ԃ�����Ă���
        DirectX::Keyboard::State state;
    };
}
