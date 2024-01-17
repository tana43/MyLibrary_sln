#include "Collision.h"
#include "MathHelper.h"


// 球と球の衝突判定(押し出し後の位置計算あり)
const bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA, const float radiusA,
    const DirectX::XMFLOAT3& positionB, const float radiusB,
    DirectX::XMFLOAT3* outPosition)
{
    const DirectX::XMFLOAT3 vec      = positionB - positionA;
    const float    lengthSq = LengthSq(vec);
    const float    range    = radiusA + radiusB;

    if (lengthSq > (range * range)) return false;

    const DirectX::XMFLOAT3 vecN = Normalize(vec);
    (*outPosition) = (positionA + vecN * range);

    return true;
}

// 球と球の交差判定
const bool Collision::IntersectSphereVsSphere(
    const DirectX::XMFLOAT3& positionA, const float radiusA,
    const DirectX::XMFLOAT3& positionB, const float radiusB)
{
    const DirectX::XMFLOAT3 vec      = positionB - positionA;
    const float    lengthSq = LengthSq(vec);
    const float    range    = radiusA + radiusB;

    if (lengthSq > (range * range)) return false;

    return true;
}

const bool Collision::IntersectSphereVsRay(const DirectX::XMFLOAT3& spherePos, const float radius, const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, const float length)
{
    DirectX::XMFLOAT3 vec = spherePos - rayStart;

    //光線の方向と逆方向に球があった場合は処理しない
    if (Dot(Normalize(vec), rayDirection) < 0)return false;

    //光線の長さよりも球までの距離が長かった場合処理しない
    if (Length(vec) > length)return false;

    //光線から球までの最短距離
    float projectionLength = Dot(vec, rayDirection);

    const DirectX::XMFLOAT3 p1 = rayStart + rayDirection * projectionLength;
    float nearestLength = Length(p1 - spherePos);

    //この距離が球の半径よりも短いなら交差している
    if (nearestLength <= radius)
    {
        return true;
    }

    return false;
}
