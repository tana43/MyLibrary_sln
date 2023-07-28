#include "GltfModel.hlsli"

VS_OUT main(VS_IN vin)
{
    float sigma = vin.tangent.w;
    
    if (skin > -1)
    {
        row_major float4x4 skinMatrix =
            vin.weights.x * jointMatrices[vin.joints.x] +
            vin.weights.y * jointMatrices[vin.joints.y] +
            vin.weights.z * jointMatrices[vin.joints.z] +
            vin.weights.w * jointMatrices[vin.joints.w];
        //vin.position = mul(float4(vin.position.xyz, 1), skinMatrix);
        vin.normal = normalize(mul(float4(vin.normal.xyz, 0), skinMatrix));
        vin.tangent = normalize(mul(float4(vin.tangent.xyz, 0), skinMatrix));
    }
    
    VS_OUT vout;
	
    vin.position.w = 1;
    vout.position = mul(vin.position, mul(world, viewProjection));
    vout.wPosition = mul(vin.position, world);
    
    vin.normal.w = 0;
    vout.wNormal = normalize(mul(vin.normal, world));
    
    //float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    vout.wTangent = normalize(mul(vin.tangent, world));
    vout.wTangent.w = sigma;
	
    vout.texcoord = vin.texcoord;
    
    return vout;
}