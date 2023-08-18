#include "Particle.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
SamplerState samplerStates[3] : register(s0);
Texture2D textureMap : register(t0);

float4 main(GS_OUT pin) : SV_TARGET
{
	return pin.color;
}