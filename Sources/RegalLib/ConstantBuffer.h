#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <assert.h>

/*
	// framework.h
	struct constants_t
	{
		float params[4];
	};
	std::unique_ptr <constant_buffer<constants_t>> constants;

	// framework::initialize
	constants = std::make_unique<constant_buffer<constants_t>>(device.Get());

	// framework::update/render
	constants->data.params[0] = 0;
	constants->activate(immediate_context.Get(), 0, true, true);
*/

#define USAGE_DYNAMIC


//定数バッファを簡単に作ってくれる汎用的なクラス
//作りたい定数が入っている構造体を使って定義する
template <class T>
struct ConstantBuffer
{
	T data;
	ConstantBuffer(ID3D11Device* device)
	{
		HRESULT hr = S_OK;
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = (sizeof(T) + 0x0f) & ~0x0f;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
#ifdef USAGE_DYNAMIC
		//https://docs.microsoft.com/en-us/windows/desktop/direct3d11/how-to--use-dynamic-resources
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		hr = device->CreateBuffer(&bufferDesc, 0, bufferObject.GetAddressOf());
		assert(SUCCEEDED(hr) && hr_trace(hr));
#else
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA subresource_data;
		subresource_data.pSysMem = &data;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, buffer_object.GetAddressOf());
		assert(SUCCEEDED(hr) && hr_trace(hr));
#endif
	}
	virtual ~ConstantBuffer() = default;
	ConstantBuffer(ConstantBuffer&) = delete;
	ConstantBuffer& operator =(ConstantBuffer&) = delete;

	//引数にどのシェーダーで使うかを入れておく
	void Activate(ID3D11DeviceContext* immediate_context, int slot, bool vs_on = true, bool ps_on = true)
	{
		HRESULT hr = S_OK;
#ifdef USAGE_DYNAMIC
		D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
		D3D11_MAPPED_SUBRESOURCE mapped_buffer;

		hr = immediate_context->Map(bufferObject.Get(), 0, map, 0, &mapped_buffer);
		assert(SUCCEEDED(hr) && hr_trace(hr));
		memcpy_s(mapped_buffer.pData, sizeof(T), &data, sizeof(T));
		immediate_context->Unmap(bufferObject.Get(), 0);
#else
		immediate_context->UpdateSubresource(buffer_object.Get(), 0, 0, &data, 0, 0);
#endif
		if (vs_on)
		{
			immediate_context->VSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());
		}
		if (ps_on)
		{
			immediate_context->PSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());
		}
	}
	void Deactivate(ID3D11DeviceContext* immediate_context)
	{
		// NOP
	}
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> bufferObject;

};
