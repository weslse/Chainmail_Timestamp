#pragma once

// 볼륨 사이즈 상수
#define VOLUME_WIDTH 256
#define VOLUME_HEIGHT 256
#define VOLUME_DEPTH 225

#define VOLUME_SIZE (VOLUME_WIDTH * VOLUME_HEIGHT * VOLUME_DEPTH)

// 노드 개수 조절
#define ARR_WIDTH 64
#define ARR_HEIGHT 64
#define NUM_NEIGHBOR 4
#define NUM_MEMSET 2

// 창 크기 조절
#define WINDOW_W 600
#define WINDOW_H 600

// gl function 변수
#define ORTHO_BOUND (ARR_WIDTH * .8)

// 변화할 위치 변수 설정 가능
// (빨간색 고리, 현재 검정색으로 그려짐)
#define TARGET_X 32	
#define TARGET_Y 32

// epsilon 상수 값
#define EPS 0.000001f

// 노드 간의 기본 거리
#define ORIGIN_LEN 1.0f

// 원진 누나 ppt 참고(2차원일때 최대 4, 3차원 : 6)
#define MAX_F 4.f
#define ELASTICITY_EXP 0.16f

// Texture size
#define TEX_WIDTH ARR_WIDTH
#define TEX_HEIGHT ARR_HEIGHT