#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

#include <vector>

#define NUMTHREADS_X 16

namespace Regal::Graphics
{
    struct Particle
    {
        DirectX::XMFLOAT4 color_{1, 1, 1, 1};
        DirectX::XMFLOAT3 position_{0, 0, 0};
        DirectX::XMFLOAT3 velocity_{0, 0, 0};
        float age_{};
        int state_{};
    };
    struct Particles
    {
        const size_t maxParticleCount_;
        struct  ParticleConstants
        {
            DirectX::XMFLOAT3 emitterPosition_{};
            float particleSize_{ 0.02f };
            float time_{};
            float deltaTime_{};
            float something_[2];
        };
        ParticleConstants particleData_;

        Microsoft::WRL::ComPtr<ID3D11Buffer> particleBuffer_;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particleBufferUav_;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleBufferSrv_;

        Microsoft::WRL::ComPtr<ID3D11VertexShader> particleVS_;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> particlePS_;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader> particleGS_;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> particleCS_;
        Microsoft::WRL::ComPtr<ID3D11ComputeShader> particleInitializerCS_;
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;

        Particles(ID3D11Device* device, size_t particleCount);
        Particles(const Particles&) = delete;
        Particles& operator=(const Particles&) = delete;
        Particles(Particles&&) noexcept = delete;
        Particles& operator=(Particles&&) noexcept = delete;
        virtual ~Particles() = default;

        //çXêVèàóù
        void Integrate(ID3D11DeviceContext* immediateContext, float deltaTime);

        void Initialize(ID3D11DeviceContext* immediateContext, float deltaTime);
        void Render(ID3D11DeviceContext* immediateContext);
        void DrawDebug();
    };
}
