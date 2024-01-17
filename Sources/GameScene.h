#pragma once

#include "./RegalLib/Regal.h"

class GameScene final : public Regal::Scene::BaseScene
{
public:
    GameScene() {}
    ~GameScene()override {}

    void CreateResource()override;
    void Initialize() override;
    void Finalize() override;
    void Begin() override;
    void Update(const float& elapsedTime)override;
    void End() override;
    void Render(const float& elapsedTime)override;

    void DrawDebug()override;
    void PostEffectDrawDebug()override;

private:
    float clearColor_[4] = { 0.1f,0.1f,0.1f,1.0f };
    std::unique_ptr<Regal::Graphics::Framebuffer> framebuffer_;
    std::unique_ptr<Regal::Graphics::Bloom> bloomer_;
    std::unique_ptr<Regal::Graphics::FullscreenQuad> bitBlockTransfer_;

    //輝度成分抽出用シェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> LEPixelShader_;

    std::unique_ptr<Regal::Resource::Sprite> sprite_;
};

