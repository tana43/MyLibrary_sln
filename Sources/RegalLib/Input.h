#pragma once

#include "Mouse.h"
#include "Keyboard.h"
#include "GamePad.h"

using Device = unsigned int;

namespace Regal::Input
{
    //このクラスから各種入力のアップデートをする
    //入力判定を各アクションごとに決めたいときもここで関数定義する
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

