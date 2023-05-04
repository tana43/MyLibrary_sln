#include "GeometricPrimitive.h"
#include "misc.h"
#include <sstream>

GeometricPrimitive::GeometricPrimitive(ID3D11Device* device)
{
    Vertex  vertices[24]{};
    //�������̂̃R���g���[���|�C���g����8�A
    //�@���̌������Ⴄ���_���R����̂Œ��_���̑�����8x3 = 24�A
    //���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B

    uint32_t indices[36]{};
    //�������̂͂U�ʎ����A�P�̖ʂ͂Q�̂R�p�`�|���S���ō\�������̂ő�����6x2 = 12�A
    //�������̂�`�悷�邽�߂ɂP�Q��̂R�p�`�|���S���`�悪�K�v�A����ĎQ�Ƃ���钸�_����12x3 = 36��A
    //3�p�`�|���S�����Q�Ƃ��钸�_���̃C���f�b�N�X�i���_�ԍ��j��`�揇�ɔz��iindices�j�Ɋi�[����B
    //���v��肪�\�ʂɂȂ�悤�Ɋi�[���邱�ƁB

    CreateComBuffers(device, vertices, 24, indices, 36);

    HRESULT hr{ S_OK };

    D3D11_INPUT_ELEMENT_DESC inputElememtDesc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };
    
    //���_�V�F�[�_�[�I�u�W�F�N�g����
    const char* csoName{ "GeometricPrimitive.cso" };

    FILE* fp{};
    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO file not found");

    fseek(fp, 0, SEEK_END);
    long csoSize{ ftell(fp) };
    fseek(fp, 0, SEEK_SET);

    std::unique_ptr<unsigned char[]>csoData{ std::make_unique<unsigned char[]>(csoSize) };
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g����
    csoName = "SpritePS.cso";

    fopen_s(&fp, csoName, "rb");
    _ASSERT_EXPR_A(fp, "CSO file not found");

    fseek(fp, 0, SEEK_END);
    csoSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    csoData = std::make_unique<unsigned char[]>(csoSize);
    fread(csoData.get(), csoSize, 1, fp);
    fclose(fp);

    hr = device->CreatePixelShader(csoData.get(), csoSize, nullptr, pixelShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    hr = device->CreateVertexShader(csoData.get(), csoSize, nullptr, vertexShader.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    
}

void GeometricPrimitive::CreateComBuffers(ID3D11Device* device, Vertex* vertices, size_t vertexCount, uint32_t* indices, size_t indexCount)
{
}
