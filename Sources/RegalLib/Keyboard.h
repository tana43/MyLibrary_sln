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
        static DirectX::Keyboard::State& GetKeyState() { return Instance().state_; }

        //�L�[���������u�Ԃ��ǂ�����Ԃ�
        static const bool GetKeyDown(const DirectX::Keyboard::Keys key);

        //�L�[�𗣂����u�Ԃ��ǂ�����Ԃ�
        static const bool GetKeyUp(const DirectX::Keyboard::Keys key);

    private:
        //DirectX::Keyboard�N���X�𐶐����邽�߂̃|�C���^
        std::unique_ptr<DirectX::Keyboard> keyboard_;

        //�{�^�����������u�ԁA�������u�Ԃ̏��������Ă���Ă���
        DirectX::Keyboard::KeyboardStateTracker tracker_;

        //�L�[�{�[�h�̓��͏�Ԃ�����Ă���
        DirectX::Keyboard::State state_;
    };
}
