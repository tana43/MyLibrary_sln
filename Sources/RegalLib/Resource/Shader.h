#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

namespace Regal::Resource
{
    //�Q�[�v���Ɠ������ɂ��悤�Ǝv�������ǎ��Ԃ��Ȃ��̂Ńp�X
    class Shader
    {
    public:
        static void CreateVSFromCso(ID3D11Device* device, const char* csoName, ID3D11VertexShader** vertexShader,
            ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, SIZE_T BytecodeLength);

        static void CreatePSFromCso(ID3D11Device* device, const char* csoName, ID3D11PixelShader** pixelShader);

        static void CreateGSFromCso(ID3D11Device* device, const char* csoName, ID3D11GeometryShader** geometryShader);

        static void CreateCSFromCso(ID3D11Device* device, const char* csoName, ID3D11ComputeShader** computeShader);

    public:
        //Shader() {}
        //virtual ~Shader() {}

        ////�`��J�n
        //virtual void Begin(ID3D11DeviceContext* immediateContext) = 0;

        ////�`��
        //virtual void Draw(ID3D11DeviceContext* immediateContext, const Model* model) = 0;

        ////�`��I��
        //virtual void End(ID3D11DeviceContext* immediateContext) = 0;

        struct  SceneConstants
        {
            DirectX::XMFLOAT4X4 viewProjection;	//�r���[�v���W�F�N�V���������s��
            DirectX::XMFLOAT4 lightDirection;	//���C�g�̌���
            DirectX::XMFLOAT4 cameraPosition;
            //DirectX::XMFLOAT4X4 inverseViewProjection;//�r���[�v���W�F�N�V�����t�s��
        };

        Shader(ID3D11Device* device);
        ~Shader() {}

        //�`��J�n(�V�[���p�̒萔�o�b�t�@���Z�b�g�X�V���Ă��邾��)
        void UpdateSceneConstants(ID3D11DeviceContext* immediateContext);

        void SceneConstantsDrawDebug();

    private:
        Microsoft::WRL::ComPtr<ID3D11Buffer> sceneConstantBuffer;

        DirectX::XMFLOAT3 directionalLightAngle{DirectX::XMFLOAT3(0,0,0)};
    };
}
