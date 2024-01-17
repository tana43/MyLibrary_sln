#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Sprite.h"

namespace Regal::Graphics
{
    class SkyBox
    {
    public:
        SkyBox(ID3D11Device* device, Regal::Resource::Sprite* sprite);
        ~SkyBox() {}

        struct  Vertex
        {
            DirectX::XMFLOAT3 position_;
        };

        struct  CbScene
        {
            DirectX::XMFLOAT4X4 inverseViewProjection_;
            DirectX::XMFLOAT4 viewPosition_;
        };

        void Render(ID3D11DeviceContext* immediateContext, DirectX::XMMATRIX V/*view*/, DirectX::XMMATRIX P/*projection*/);

    private:

        Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer_;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;

        //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

        Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState_;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState_;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState_;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState_;

        Regal::Resource::Sprite* sprite_;
    };
}
