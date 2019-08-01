#pragma once

#include <utility>
#include "DefConstants.h"
#include "DefTypes.h"


// 노드(포인트) - 포지션(x,y), 시간
// 엣지(링크, 제한값(constraint)) - x,y에 대한 값
// 2D Chainmail
class Chainmail
{

public:
	// to display
	Node node[ARR_HEIGHT][ARR_WIDTH];
	Constraint link[ARR_HEIGHT][ARR_WIDTH][NUM_NEIGHBOR];

	bool pingPongCnt = false;

	// to relaxation
	Node nodeCopy[ARR_HEIGHT][ARR_WIDTH];

public:
	Chainmail() = default;
	~Chainmail() = default;

	void setNode();
	void setLink();
	void resetTime();

	void movePosition(const int x, const int y, const float mvX, const float mvY);

	const std::pair<Node, Direction> minTimeNeighborDir(const int x, const int y) const;

	void propagate();
	void relax();
	void relax_spring();
	void relax_sein();
};

