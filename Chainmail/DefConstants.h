#pragma once

// ���� ������ ���
#define VOLUME_WIDTH 256
#define VOLUME_HEIGHT 256
#define VOLUME_DEPTH 225

#define VOLUME_SIZE (VOLUME_WIDTH * VOLUME_HEIGHT * VOLUME_DEPTH)

// ��� ���� ����
#define ARR_WIDTH 64
#define ARR_HEIGHT 64
#define NUM_NEIGHBOR 4
#define NUM_MEMSET 2

// â ũ�� ����
#define WINDOW_W 600
#define WINDOW_H 600

// gl function ����
#define ORTHO_BOUND (ARR_WIDTH * .8)

// ��ȭ�� ��ġ ���� ���� ����
// (������ ��, ���� ���������� �׷���)
#define TARGET_X 32	
#define TARGET_Y 32

// epsilon ��� ��
#define EPS 0.000001f

// ��� ���� �⺻ �Ÿ�
#define ORIGIN_LEN 1.0f

// ���� ���� ppt ����(2�����϶� �ִ� 4, 3���� : 6)
#define MAX_F 4.f
#define ELASTICITY_EXP 0.16f

// Texture size
#define TEX_WIDTH ARR_WIDTH
#define TEX_HEIGHT ARR_HEIGHT