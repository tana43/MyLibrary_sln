#include "Particles.h"

#include "Shader.h"
#include "Misc.h"

#include "../../External/imgui/imgui.h"

using namespace Regal::Resource;

namespace Regal::Graphics
{
    Particles::Particles(ID3D11Device* device, size_t particleCount) : maxParticleCount_(particleCount)
    {
        HRESULT hr{ S_OK };
        D3D11_BUFFER_DESC bufferDesc{};
        bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Particle) * particleCount);
        bufferDesc.StructureByteStride = sizeof(Particle);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        hr = device->CreateBuffer(&bufferDesc, NULL, particleBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        shaderResourceViewDesc.Buffer.ElementOffset = 0;
        shaderResourceViewDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
        hr = device->CreateShaderResourceView(particleBuffer_.Get(), &shaderResourceViewDesc, particleBufferSrv_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
        unorderedAccessViewDesc.Format = DXGI_FORMAT_UNKNOWN;
        unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        unorderedAccessViewDesc.Buffer.FirstElement = 0;
        unorderedAccessViewDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
        unorderedAccessViewDesc.Buffer.Flags = 0;
        hr = device->CreateUnorderedAccessView(particleBuffer_.Get(), &unorderedAccessViewDesc, particleBufferUav_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        bufferDesc.ByteWidth = sizeof(ParticleConstants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer_.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        Shader::CreateVSFromCso(device, "./Resources/Shader/ParticleVS.cso", particleVS_.ReleaseAndGetAddressOf(), NULL, NULL, 0);
        Shader::CreatePSFromCso(device, "./Resources/Shader/ParticlePS.cso", particlePS_.ReleaseAndGetAddressOf());
        Shader::CreateGSFromCso(device, "./Resources/Shader/ParticleGS.cso", particleGS_.ReleaseAndGetAddressOf());
        Shader::CreateCSFromCso(device, "./Resources/Shader/ParticleCS.cso", particleCS_.ReleaseAndGetAddressOf());
        Shader::CreateCSFromCso(device, "./Resources/Shader/ParticleInitializerCS.cso", particleInitializerCS_.ReleaseAndGetAddressOf());
    }

    UINT Align(UINT num, UINT alignment)
    {
        return (num + (alignment - 1)) & ~(alignment - 1);
    }

    void Particles::Integrate(ID3D11DeviceContext* immediateContext, float deltaTime)
    {
        immediateContext->CSSetUnorderedAccessViews(0, 1, particleBufferUav_.GetAddressOf(), NULL);

        particleData_.time_ += deltaTime;
        particleData_.deltaTime_ = deltaTime;
        immediateContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &particleData_, 0, 0);
        immediateContext->CSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

        immediateContext->CSSetShader(particleCS_.Get(), NULL, 0);

        const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
        immediateContext->Dispatch(threadGroupCountX, 1, 1);

        ID3D11UnorderedAccessView* nullUnorederedAccessView{};
        immediateContext->CSSetUnorderedAccessViews(0, 1, &nullUnorederedAccessView, NULL);
    }

    void Particles::Initialize(ID3D11DeviceContext* immediateContext, float deltaTime)
    {
        immediateContext->CSSetUnorderedAccessViews(0, 1, particleBufferUav_.GetAddressOf(), NULL);

        particleData_.time_ += deltaTime;
        particleData_.deltaTime_ = deltaTime;
        immediateContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &particleData_, 0, 0);
        immediateContext->CSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

        immediateContext->CSSetShader(particleInitializerCS_.Get(), NULL, 0);

        const UINT threadGroupCountX = Align(static_cast<UINT>(maxParticleCount_), NUMTHREADS_X) / NUMTHREADS_X;
        immediateContext->Dispatch(threadGroupCountX, 1, 1);

        ID3D11UnorderedAccessView* nullUnorderedAccessView{};
        immediateContext->CSSetUnorderedAccessViews(0, 1, &nullUnorderedAccessView, NULL);
    }

    void Particles::Render(ID3D11DeviceContext* immediateContext)
    {
        immediateContext->VSSetShader(particleVS_.Get(), NULL, 0);
        immediateContext->PSSetShader(particlePS_.Get(), NULL, 0);
        immediateContext->GSSetShader(particleGS_.Get(), NULL, 0);
        immediateContext->GSSetShaderResources(9, 1, particleBufferSrv_.GetAddressOf());

        immediateContext->UpdateSubresource(constantBuffer_.Get(), 0, 0, &particleData_, 0, 0);
        immediateContext->VSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());
        immediateContext->PSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());
        immediateContext->GSSetConstantBuffers(9, 1, constantBuffer_.GetAddressOf());

        immediateContext->IASetInputLayout(NULL);
        immediateContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
        immediateContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
        immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        immediateContext->Draw(static_cast<UINT>(maxParticleCount_), 0);

        ID3D11ShaderResourceView* nullShaderResourceView{};
        immediateContext->GSSetShaderResources(9, 1, &nullShaderResourceView);
        immediateContext->VSSetShader(NULL, NULL, 0);
        immediateContext->PSSetShader(NULL, NULL, 0);
        immediateContext->GSSetShader(NULL, NULL, 0);
    }

    void Particles::DrawDebug()
    {
        if (ImGui::BeginMenu("Particle System"))
        {
            ImGui::SliderFloat("Particle Size", &particleData_.particleSize_, 0.0f, 0.1f);
            ImGui::DragFloat3("Emitter Position", &particleData_.emitterPosition_.x, 0.1f);
            //ImGui::SliderFloat("Effect Time", &particleData.time,0.0f, 20.0f);

            ImGui::EndMenu();
        }
    }
}
