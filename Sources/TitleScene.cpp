#include "TitleScene.h"
#include "GameScene.h"

void TitleScene::CreateResource()
{
	auto& graphics{ Regal::Graphics::Graphics::Instance() };

	framebuffer_ = std::make_unique<Regal::Graphics::Framebuffer>(
		graphics.GetDevice(), graphics.GetScreenWidth(), graphics.GetScreenHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, true);
	bitBlockTransfer_ = std::make_unique < Regal::Graphics::FullscreenQuad>(graphics.GetDevice());


	Regal::Resource::Shader::CreatePSFromCso(graphics.GetDevice(), "./Resources/Shader/LuminanceExtractionPS.cso", LEPixelShader_.GetAddressOf());

	bloomer_ = std::make_unique<Regal::Graphics::Bloom>(graphics.GetDevice(), graphics.GetScreenWidth(), graphics.GetScreenHeight());
	Regal::Resource::Shader::CreatePSFromCso(graphics.GetDevice(), "./Resources/Shader/FinalPassPS.cso", LEPixelShader_.ReleaseAndGetAddressOf());

	sprite_ = std::make_unique<Regal::Resource::Sprite>(graphics.GetDevice(), L"./Resources/Images/Title.png");
}

void TitleScene::Initialize()
{
	sprite_->SetColor(1, 1, 1, 0.01f);
}

void TitleScene::Finalize()
{
}

void TitleScene::Begin()
{
}

void TitleScene::Update(const float& elapsedTime)
{
	if (Regal::Input::Keyboard::GetKeyDown(DirectX::Keyboard::Enter))
	{
		Regal::Scene::SceneManager::Instance().ChangeScene(new GameScene);
	}
}

void TitleScene::End()
{
}

void TitleScene::Render(const float& elapsedTime)
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
		/*sprite->Render(graphics.GetDeviceContext(),0,0,
			graphics.GetScreenWidth(),graphics.GetScreenHeight(),0);*/
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

void TitleScene::DrawDebug()
{
	if (ImGui::BeginMenu("Clear Color"))
	{
		ImGui::ColorEdit4("Color", &clearColor_[0]);

		ImGui::EndMenu();
	}


	sprite_->DrawDebug();
}

void TitleScene::PostEffectDrawDebug()
{
	bloomer_->DrawDebug();
}
