#include "GeometricPrimitive.h"

GeometricPrimitive::GeometricPrimitive(ID3D11Device* device)
{
    Vertex  vertices[24]{};
    //�������̂̃R���g���[���|�C���g����8�A
    //�@���̌������Ⴄ���_���R����̂Œ��_���̑�����8x3 = 24�A
    //���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B

    uint32_t indices[36]{};
    //�������̂͂U�ʎ����A�P�̖ʂ͂Q�̂R�p�`�|���S���̑�����6x2 = 12�A
    //�������̂�`�悷�邽�߂ɂP�Q��̂R�p�`�|���S���`�悪�K�v�A����ĎQ�Ƃ���钸�_����12x3 = 36��A
    //3�p�`�|���S�����Q�Ƃ��钸�_���̃C���f�b�N�X�i���_�ԍ��j��`�揇�ɔz��iindices�j�Ɋi�[����B
    //���v���́����\�ʂɂȂ�悤�Ɋi�[���邱�ƁB
}
