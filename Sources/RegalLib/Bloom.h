#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>

#include "FrameBuffer.h"
#include "FullscreenQuad.h"

namespace Regal::Graphics
{
    class Bloom
    {
    public:
        Bloom(ID3D11Device* device, uint32_t width, uint32_t height);
        ~Bloom() = default;
        Bloom(const Bloom&) = delete;
        Bloom& operator = (const Bloom&) = delete;
        Bloom(Bloom&&) noexcept = delete;
        Bloom& operator = (Bloom&&) noexcept = delete;

        void Bloom::Make(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView* colorMap);
        ID3D11ShaderResourceView* ShaderResourceView() const
        {
            return glowExtraction_->shaderResourceViews_[0].Get();
        }

        void DrawDebug();

    public:
        float bloomExtractionThreshold_ = 0.85f;
        float bloomIntensity_ = 0.15f;

    private:
        std::unique_ptr<FullscreenQuad> bitBlockTransfer_;
        std::unique_ptr<Framebuffer> glowExtraction_;

        static const size_t downsampledCount_ = 6;
        std::unique_ptr<Framebuffer> gaussianBlur_[downsampledCount_][2];

        Microsoft::WRL::ComPtr<ID3D11PixelShader> glowExtractionPs_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurDownsamplingPs_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurHorizontalPs_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurVerticalPs_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussianBlurUpsamplingPs_;

        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_;
        Microsoft::WRL::ComPtr<ID3D11BlendState> blendState_;

        struct BloomConstants
        {
            float bloomExtractionThreshold_;
            float bloomIntensity_;
            float something_[2];
        };
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;
    };
}

