#include "FullscreenQuad.h"
#include "Shader.h"
#include "misc.h"

using namespace Regal::Resource;

namespace Regal::Graphics
{
    FullscreenQuad::FullscreenQuad(ID3D11Device* device)
    {
        Shader::CreateVSFromCso(device, "./Resources/Shader/FullscreenQuadVS.cso", embeddedVertexShader_.ReleaseAndGetAddressOf(),
            nullptr, nullptr, 0);
        Shader::CreatePSFromCso(device, "./Resources/Shader/FullscreenQuadPS.cso", embeddedPixelShader_.ReleaseAndGetAddressOf());
    }

    void FullscreenQuad::Bilt(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView** shaderResourceView, uint32_t startSlot, uint32_t numViews, ID3D11PixelShader* replacedPixelShader)
    {
        immediateContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
        immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        immediateContext->IASetInputLayout(nullptr);

        immediateContext->VSSetShader(embeddedVertexShader_.Get(), 0, 0);
        replacedPixelShader ? immediateContext->PSSetShader(replacedPixelShader, 0, 0) :
            immediateContext->PSSetShader(embeddedPixelShader_.Get(), 0, 0);


        immediateContext->PSSetShaderResources(startSlot, numViews, shaderResourceView);

        immediateContext->Draw(4, 0);
    }
}
