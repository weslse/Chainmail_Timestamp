#pragma once

using ubyte = unsigned char;

// ������, ����, ��, �Ʒ�
enum Direction {
	RIGHT = 0, LEFT, TOP, BOTTOM
};

typedef struct {
	float x;
	float y;
	float z;
} Position;

typedef struct {
	Position position;
	float time;
	float density;
} Node;

typedef struct {
	float minDx;
	float maxDx;
	float maxHrzDy;

	float minDy;
	float maxDy;
	float maxVrtDx;
} Constraint;
