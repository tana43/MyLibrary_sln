#include "Input.h"

namespace Regal::Input
{
    Input::Input()
    {
    }

    void Input::Update()
    {
        Keyboard::Instance().Update();
        Mouse::Instance().Update();
        gamePad_.Update();

        //最後に入力されたデバイスの検出
        {
            auto currPtr = reinterpret_cast<const uint32_t*>(&Keyboard::Instance().GetKeyState());
            for (size_t j = 0; j < (256 / 32); ++j)
            {
                if (*currPtr)
                {
                    useDevice_ = KEYBOARD;
                    break;
                }
                ++currPtr;
            }

            if (gamePad_.GetButton())
            {
                useDevice_ = GAMEPAD;
            }
        }
    }
}
