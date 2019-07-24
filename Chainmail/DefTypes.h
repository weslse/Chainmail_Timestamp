#pragma once

typedef struct {
	struct Position {
		float x;
		float y;
	} position;

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
