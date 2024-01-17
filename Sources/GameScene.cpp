#include "GameScene.h"
#include "TitleScene.h"

void GameScene::CreateResource()
{
	auto& graphics{ Regal::Graphics::Graphics::Instance() };

	framebuffer_ = std::make_unique<Regal::Graphics::Framebuffer>(
		graphics.GetDevice(), graphics.GetScreenWidth(), graphics.GetScreenHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, true);
	bitBlockTransfer_ = std::make_unique < Regal::Graphics::FullscreenQuad>(graphics.GetDevice());


	Regal::Resource::Shader::CreatePSFromCso(graphics.GetDevice(), "./Resources/Shader/LuminanceExtractionPS.cso", LEPixelShader_.GetAddressOf());

	bloomer_ = std::make_unique<Regal::Graphics::Bloom>(graphics.GetDevice(), graphics.GetScreenWidth(), graphics.GetScreenHeight());
	Regal::Resource::Shader::CreatePSFromCso(graphics.GetDevice(), "./Resources/Shader/FinalPassPS.cso", LEPixelShader_.ReleaseAndGetAddressOf());

	sprite_ = std::make_unique<Regal::Resource::Sprite>(graphics.GetDevice(), L"./Resources/Images/Game.png");
}

void GameScene::Initialize()
{
	sprite_->SetColor(1, 1, 1, 0.01f);
}

void GameScene::Finalize()
{
}

void GameScene::Begin()
{
}

void GameScene::Update(const float& elapsedTime)
{
	if (Regal::Input::Keyboard::GetKeyDown(DirectX::Keyboard::Back))
	{
		Regal::Scene::SceneManager::Instance().ChangeScene(new TitleScene);
	}
}

void GameScene::End()
{
}

void GameScene::Render(const float& elapsedTime)
{
	using namespace Regal;

	auto& graphics{ Graphics::Graphics::Instance() };
	auto* immediateContext{ graphics.GetDeviceContext() };

#ifndef DISABLE_OFFSCREENRENDERING
	framebuffer_->Clear(immediateContext, clearColor_[0], clearColor_[1], clearColor_[2], clearColor_[3]);
	framebuffer_->Activate(immediateContext);
#endif // !ENABLE_OFFSCREENRENDERING

	//2D
	{
		graphics.Set2DStates();

		sprite_->Render();
		/*sprite->Render(graphics.GetDeviceContext(), 0, 0,
			graphics.GetScreenWidth(), graphics.GetScreenHeight(), 0);*/
	}

	//パーティクル
	graphics.SetStates(Graphics::ZT_ON_ZW_ON, Graphics::CULL_NONE, Graphics::ALPHA);
	immediateContext->GSSetConstantBuffers(1, 1, graphics.GetShader()->GetSceneConstanceBuffer().GetAddressOf());

	//3D
	{
		graphics.Set3DStates();

	}
#ifndef DISABLE_OFFSCREENRENDERING
	framebuffer_->Deactivate(immediateContext);
#endif // !DISABLE_OFFSCREENRENDERING

	//ブルーム
	{
		bloomer_->Make(immediateContext, framebuffer_->shaderResourceViews_[0].Get());
		graphics.SetStates(Graphics::ZT_OFF_ZW_OFF, Graphics::CULL_NONE, Graphics::ALPHA);
		ID3D11ShaderResourceView* shaderResourceViews[] =
		{
			framebuffer_->shaderResourceViews_[0].Get(),
			bloomer_->ShaderResourceView(),
		};
		bitBlockTransfer_->Bilt(immediateContext, shaderResourceViews, 0, 2, LEPixelShader_.Get());
	}
}

void GameScene::DrawDebug()
{
	if (ImGui::BeginMenu("Clear Color"))
	{
		ImGui::ColorEdit4("Color", &clearColor_[0]);

		ImGui::EndMenu();
	}


	sprite_->DrawDebug();
}

void GameScene::PostEffectDrawDebug()
{
	bloomer_->DrawDebug();
}
