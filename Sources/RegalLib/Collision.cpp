#include "Collision.h"
#include "MathHelper.h"


// ���Ƌ��̏Փ˔���(�����o����̈ʒu�v�Z����)
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

// ���Ƌ��̌�������
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

    //�����̕����Ƌt�����ɋ����������ꍇ�͏������Ȃ�
    if (Dot(Normalize(vec), rayDirection) < 0)return false;

    //�����̒����������܂ł̋��������������ꍇ�������Ȃ�
    if (Length(vec) > length)return false;

    //�������狅�܂ł̍ŒZ����
    float projectionLength = Dot(vec, rayDirection);

    const DirectX::XMFLOAT3 p1 = rayStart + rayDirection * projectionLength;
    float nearestLength = Length(p1 - spherePos);

    //���̋��������̔��a�����Z���Ȃ�������Ă���
    if (nearestLength <= radius)
    {
        return true;
    }

    return false;
}
