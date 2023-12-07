#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <sstream>

namespace Regal::Resource
{
    class Sprite
    {
        static int num;//生成回数 imguiの名前被りの防止

    public:
        Sprite(ID3D11Device* device, const wchar_t* filename,
            std::string name = "Sprite");
        ~Sprite();

    public:
        struct SpriteTransform
        {
        private:
            DirectX::XMFLOAT2 pos = {};
            DirectX::XMFLOAT2 scale = { 1,1 };
            float angle = 0.0f;
            DirectX::XMFLOAT2 texPos = {};
            DirectX::XMFLOAT2 texSize = { 100,100 };

        public:
            void DrawDebug();

            void SetPos(const DirectX::XMFLOAT2 p) { pos = p; }
            void SetPosX(const float posX) { pos.x = posX; }
            void SetPosY(const float posY) { pos.y = posY; }
            // 引数の位置を画像の中心として、画像の位置を設定する
            void SetSpriteCenterPos(DirectX::XMFLOAT2 p)
            {
                p.x -= scale.x / 2;
                p.y -= scale.y / 2;
                pos = p;
            }
            // 第二引数の位置を中心位置として、画像の位置を設定する
            void SetSpriteAtAnyCenterPos(DirectX::XMFLOAT2 p, DirectX::XMFLOAT2 center)
            {
                p.x -= center.x;
                p.y -= center.y;
                pos = p;
            }
            void SetScale(const DirectX::XMFLOAT2 s) { scale = s; }
            void SetScaleX(const float x) { scale.x = x; }
            void SetScaleY(const float y) { scale.y = y; }
            void SetScaleXY(const float s)
            {
                scale.x = s;
                scale.y = s;
            }

            void SetAngle(const float a) { angle = a; }
            void SetTexPos(const DirectX::XMFLOAT2 texP) { texPos = texP; }
            void SetTexPosX(const float x) { texPos.x = x; }
            void SetTexPosY(const float y) { texPos.y = y; }
            void SetTexSize(const DirectX::XMFLOAT2 texS) { texSize = texS; }
            void SetTexSizeX(const float x) { texSize.x = x; }
            void SetTexSizeY(const float y) { texSize.y = y; }

            void AddPosX(const float posX) { pos.x += posX; }
            void AddPosY(const float posY) { pos.y += posY; }

            const DirectX::XMFLOAT2 GetPos() const { return pos; }
            const float GetPosX() const { return pos.x; }
            const float GetPosY() const { return pos.y; }
            const DirectX::XMFLOAT2 GetSize() const { return scale; }
            const float GetScaleX() const { return scale.x; }
            const float GetScaleY() const { return scale.y; }
   
            const float GetAngle() const { return angle; }
            const DirectX::XMFLOAT2 GetTexPos() const { return texPos; }
            const float GetTexPosX() const { return texPos.x; }
            const float GetTexPosY() const { return texPos.y; }
            const DirectX::XMFLOAT2 GetTexSize() const { return texSize; }
            const float GetTexSizeX() const { return texSize.x; }
            const float GetTexSizeY() const { return texSize.y; }
        };
    public:

        void DrawDebug();

        void Render();
        void Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size);
        void Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color);
        void Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color, float angle/*degree*/);
        void Render(ID3D11DeviceContext* deviceContext, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color, float angle/*degree*/, DirectX::XMFLOAT2 texPos, DirectX::XMFLOAT2 texSize); // 本体

        //改良する
       /* void Textout(ID3D11DeviceContext* immediateContext,
            std::string s,
            float x, float y, float w, float h,
            float r, float g, float b, float a
        );*/

        /// <summary>
    /// アニメーション更新関数
    /// </summary>
    /// <param name="elapsedTime">フレーム経過時間</param>
    /// <param name="frameSpeed">アニメーションする速さ</param>
    /// <param name="totalAnimationFrame">アニメーションフレーム数</param>
    /// <param name="animationVertical">縦アニメーションの場合true</param>
        void PlayAnimation(
            const float elapsedTime,
            const float frameSpeed,
            const float totalAnimationFrame,
            const bool animationVertical
        );

        //ワールド座標からスクリーン座標に変更後描画
        static DirectX::XMFLOAT2 ConvertToScreenPos(const DirectX::XMFLOAT3 worldPos);

    public:


        void SetColor(const DirectX::XMFLOAT4 c) { color = c; }
        void SetColor(const float r, const float g, const float b, const float a) { color = DirectX::XMFLOAT4(r, g, b, a); }
        void SetColorR(const float r) { color.x = r; }
        void SetColorG(const float g) { color.y = g; }
        void SetColorB(const float b) { color.z = b; }
        void SetColorA(const float a) { color.w = a; }
        void SetAlpha(const float a) { color.w = a; }

        SpriteTransform& GetSpriteTransform() { return spriteTransform; }
        DirectX::XMFLOAT4 GetColor() { return color; }
        const float GetColorR() const { return color.x; }
        const float GetColorG() const { return color.y; }
        const float GetColorB() const { return color.z; }
        const float GetAlpha() const{ return color.w; }

    public:
        //シェーダーリソースビュー取得
        const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() const { return shaderResourceView; }

        //頂点バッファ取得
        const Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer() const { return vertexBuffer; }

        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 color;
            DirectX::XMFLOAT2 texcoord;
        };

    private:
        DirectX::XMFLOAT4 color = { 1,1,1,1 };

        Microsoft::WRL::ComPtr<ID3D11VertexShader>  vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>   pixelShader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>   inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>        vertexBuffer;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
        D3D11_TEXTURE2D_DESC texture2dDesc;

        SpriteTransform spriteTransform;

        // Animation
        float animationTime = 0.0f;
        float animationFrame = 0.0f;

        std::string name;
        int myNum;
    };
};
