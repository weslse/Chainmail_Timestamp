#pragma once

// ������, ����, ��, �Ʒ�
enum Direction
{
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
} Node;

typedef struct {
	float minDx;
	float maxDx;
	float maxHrzDy;

	float minDy;
	float maxDy;
	float maxVrtDx;
} Constraint;
