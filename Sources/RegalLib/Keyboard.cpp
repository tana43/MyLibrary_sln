#include "Keyboard.h"

namespace Regal::Input
{
    Keyboard::Keyboard()
    {
        keyboard_ = std::make_unique<DirectX::Keyboard>();
    }

    void Keyboard::Update()
    {
        state_ = keyboard_->GetState();
        tracker_.Update(state_);
    }

    const bool Keyboard::GetKeyDown(const DirectX::Keyboard::Keys key)
    {
        return Instance().tracker_.IsKeyPressed(key);
    }

    const bool Keyboard::GetKeyUp(const DirectX::Keyboard::Keys key)
    {
        return Instance().tracker_.IsKeyReleased(key);
    }

    
}