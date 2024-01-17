#include "Collision.h"
#include "MathHelper.h"


// ‹…‚Æ‹…‚ÌÕ“Ë”»’è(‰Ÿ‚µo‚µŒã‚ÌˆÊ’uŒvZ‚ ‚è)
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

// ‹…‚Æ‹…‚ÌŒğ·”»’è
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

    //Œõü‚Ì•ûŒü‚Æ‹t•ûŒü‚É‹…‚ª‚ ‚Á‚½ê‡‚Íˆ—‚µ‚È‚¢
    if (Dot(Normalize(vec), rayDirection) < 0)return false;

    //Œõü‚Ì’·‚³‚æ‚è‚à‹…‚Ü‚Å‚Ì‹——£‚ª’·‚©‚Á‚½ê‡ˆ—‚µ‚È‚¢
    if (Length(vec) > length)return false;

    //Œõü‚©‚ç‹…‚Ü‚Å‚ÌÅ’Z‹——£
    float projectionLength = Dot(vec, rayDirection);

    const DirectX::XMFLOAT3 p1 = rayStart + rayDirection * projectionLength;
    float nearestLength = Length(p1 - spherePos);

    //‚±‚Ì‹——£‚ª‹…‚Ì”¼Œa‚æ‚è‚à’Z‚¢‚È‚çŒğ·‚µ‚Ä‚¢‚é
    if (nearestLength <= radius)
    {
        return true;
    }

    return false;
}
