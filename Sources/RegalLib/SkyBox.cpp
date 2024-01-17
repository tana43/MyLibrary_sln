#include "SkyBox.h"
#include "Shader.h"
#include "Misc.h"

using namespace Regal::Resource;

namespace Regal::Graphics
{
	SkyBox::SkyBox(ID3D11Device* device, Sprite* sprite) : sprite_(sprite)
	{
		//入力レイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		Shader::CreateVSFromCso(device, "./Resources/Shader/SkyBoxVS.cso", vertexShader_.GetAddressOf(),
			inputLayout_.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
		Shader::CreatePSFromCso(device, "./Resources/Shader/SkyBoxPS.cso", pixelShader_.GetAddressOf());

		//定数バッファ
		D3D11_BUFFER_DESC desc;
		::memset(&desc, 0, sizeof(desc));//memset関数はメモリに指定バイト数分の値をセットすることができる。
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbScene);
		desc.StructureByteStride = 0;

		HRESULT hr{ S_OK };
		hr = device->CreateBuffer(&desc, 0, sceneConstantBuffer_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//各種ステートの設定
		// とりあえず今は授業課題と同じように作ってみる
		// フレームワークで設定しているものをそのまま使うのが理想
		// 
		// ブレンドステート
		{
			D3D11_BLEND_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			hr = device->CreateBlendState(&desc, blendState_.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// 深度ステンシルステート
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

			hr = device->CreateDepthStencilState(&desc, depthStencilState_.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// ラスタライザーステート
		{
			D3D11_RASTERIZER_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			desc.FrontCounterClockwise = false;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0;
			desc.SlopeScaledDepthBias = 0;
			desc.DepthClipEnable = true;
			desc.ScissorEnable = false;
			desc.MultisampleEnable = true;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.CullMode = D3D11_CULL_NONE;
			desc.AntialiasedLineEnable = false;

			hr = device->CreateRasterizerState(&desc, rasterizerState_.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// サンプラステート
		{
			D3D11_SAMPLER_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = -FLT_MAX;
			desc.MaxLOD = FLT_MAX;
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			hr = device->CreateSamplerState(&desc, samplerState_.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}

	void SkyBox::Render(ID3D11DeviceContext* immediateContext, DirectX::XMMATRIX V, DirectX::XMMATRIX P)
	{
		immediateContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
		immediateContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
		immediateContext->IASetInputLayout(inputLayout_.Get());

		immediateContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		const float blendFactor[4] = { 1.0f,1.0f,1.0f,1.0f };
		immediateContext->OMSetBlendState(blendState_.Get(), blendFactor, 0xFFFFFFFF);
		immediateContext->OMSetDepthStencilState(depthStencilState_.Get(), 0);
		immediateContext->RSSetState(rasterizerState_.Get());
		immediateContext->PSSetSamplers(0, 1, samplerState_.GetAddressOf());

		//シーン用定数バッファ更新
		CbScene cbScene;

		DirectX::XMStoreFloat4x4(&cbScene.inverseViewProjection_, DirectX::XMMatrixInverse(nullptr, V * P));
		DirectX::XMStoreFloat4(&cbScene.viewPosition_, V.r[3]);

		//frameworkでやってる
		immediateContext->UpdateSubresource(sceneConstantBuffer_.Get(), 0, 0, &cbScene, 0, 0);
		immediateContext->VSSetConstantBuffers(1, 1, sceneConstantBuffer_.GetAddressOf());

		UINT stride{ sizeof(Sprite::Vertex) };
		UINT offset{ 0 };
		immediateContext->IASetVertexBuffers(0, 1, sprite_->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		immediateContext->PSSetShaderResources(0, 1, sprite_->GetShaderResourceView().GetAddressOf());
		immediateContext->Draw(4, 0);
	}
}