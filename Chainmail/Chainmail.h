#pragma once

#include <utility>
#include "DefConstants.h"
#include "DefTypes.h"


// ���(����Ʈ) - ������(x,y), �ð�
// ����(��ũ, ���Ѱ�(constraint)) - x,y�� ���� ��
// 2D Chainmail
class Chainmail {

public:
	Node node[NUM_MEMSET][ARR_HEIGHT][ARR_WIDTH] = { 0 }; // pingpong scheme
	Constraint link[ARR_HEIGHT][ARR_WIDTH][NUM_NEIGHBOR] = { 0 };

	ubyte texArr[TEX_HEIGHT][TEX_WIDTH][3] = { 0 };

	int memIdx = 0;

public:
	Chainmail() = default;
	~Chainmail() = default;

	void setNode();
	void setLink();
	void setDensity(ubyte**& volume);
	void setTexArr();
	void resetTime();

	void movePosition(const int x, const int y, const float mvX, const float mvY);

	const std::pair<Node, Direction> minTimeNeighborDir(const int x, const int y) const;
	void propagate();

	void relax();
	void relax_spring();
	void relax_sein();
};

