#include "SkyBox.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 E = normalize(pin.worldPosition.xyz - cameraPosition.xyz);
	
	//�@���x�N�g���𐳋��~���}�@�ɑ�����UV���W�֕ϊ�����
    float2 texcoord;
	{
        texcoord = float2(1.0f - atan2(E.z, E.x) / 2.0f, -atan2(E.y, length(E.xz)));
        texcoord = texcoord / PI + 0.5f;
    }
	
    return texture0.Sample(sampler0,texcoord);
}