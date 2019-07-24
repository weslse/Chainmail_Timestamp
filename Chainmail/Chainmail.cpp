#include "Chainmail.h"
using namespace std;

// 노드(Point) 초기화
void Chainmail::setNode()
{
	for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
	{
		for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
		{
			node[idxY][idxX].position.x = static_cast<float>(idxX);
			node[idxY][idxX].position.y = static_cast<float>(idxY);
			node[idxY][idxX].time = 100000.0f;
		}
	}
}

// 링크(constraint) 초기화
void Chainmail::setLink()
{
	for (int y = 0; y < ARR_HEIGHT; ++y)
	{
		for (int x = 0; x < ARR_WIDTH; ++x)
		{
			// 순서 r, l, t, b
			for (int n = 0; n < NUM_NEIGHBOR; ++n)
			{
				// 가장 간단한 상황
				// 형질이 모두 같을 때 (homogeneous)
				link[y][x][n].minDx = 0.3f;
				link[y][x][n].maxDx = 1.25f;
				link[y][x][n].maxHrzDy = 0.5f;

				link[y][x][n].minDy = 0.3f;
				link[y][x][n].maxDy = 1.25f;
				link[y][x][n].maxVrtDx = 0.5f;
			}
		}
	}
}

void Chainmail::resetTime()
{
	for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
		for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
			node[idxY][idxX].time = 100000.0f;
}

// (x', y') = (x + mvX, y + mvY)
void Chainmail::movePosition(const int x, const int y, const float mvX, const float mvY)
{
	// 타임스탬프 방법을 사용하기 때문에
	// propagation을 하기 전에 타임 스탬프를 초기화
	resetTime();

	node[y][x].position.x += mvX;
	node[y][x].position.y += mvY;
	node[y][x].time = 0.0f;

	propagate();
	relax();
}

// time이 가장 빠른 노드와 방향을 리턴한다.
const pair<Node, Direction> Chainmail::minTimeNeighborDir(const int x, const int y) const
{
	// 쓰레기 노드(무한대 시간)
	// 존재할 수 없는 위치의 노드를 대신
	// 가장 빠른 노드를 찾을때 의미가 없도록 함
	Node gabageNode;
	gabageNode.time = 100000000.f;

	// 극단 노드가 아닐때 처리
	Node leftNode = (x == 0) ? gabageNode : node[y][x - 1];
	Node rightNode = (x == ARR_WIDTH - 1) ? gabageNode : node[y][x + 1];
	Node topNode = (y == 0) ? gabageNode : node[y - 1][x];
	Node bottomNode = (y == ARR_HEIGHT - 1) ? gabageNode : node[y + 1][x];

	// 시간이 가장 빠른 노드 찾기(값이 작은)
	Node minTimeNeighbor = rightNode;
	Direction mtnDir = Direction::RIGHT;

	if (minTimeNeighbor.time > leftNode.time) {
		minTimeNeighbor = leftNode;
		mtnDir = Direction::LEFT;
	}

	if (minTimeNeighbor.time > topNode.time) {
		minTimeNeighbor = topNode;
		mtnDir = Direction::TOP;
	}

	if (minTimeNeighbor.time > bottomNode.time) {
		minTimeNeighbor = bottomNode;
		mtnDir = Direction::BOTTOM;
	}

	return make_pair(minTimeNeighbor, mtnDir);
}

// 전파(propagation) 과정
void Chainmail::propagate()
{
	bool isMoved = true;

	while (true)
	{
		// 변화가 없으면 loop 그만
		if (!isMoved)
			break;

		// 위치 변화가 없다고 가정하고 시작
		isMoved = false;

		// loop 시작
		for (int y = 0; y < ARR_HEIGHT; ++y)
		{
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				// 살짝 수정 필요 (pair 사용하지말기)
				auto pair_NeighborDir = minTimeNeighborDir(x, y);
				Node minTimeNeighbor = pair_NeighborDir.first; // minTimeNeighbor
				Direction mtnDir = pair_NeighborDir.second; // neighborDir

				// 현재 노드의 시간이 주변 노드의 시간 보다 느릴때 (나.t > 주변.t + 1)
				// 노드의 위치를 갱신한다.
				if (node[y][x].time > minTimeNeighbor.time + 1)
				{
					// 순서 : r l t b
					switch (mtnDir)
					{
					case RIGHT:
						if ((minTimeNeighbor.position.x - node[y][x].position.x) < link[y][x][mtnDir].minDx)
							node[y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].minDx;
						else if ((minTimeNeighbor.position.x - node[y][x].position.x) > link[y][x][mtnDir].maxDx)
							node[y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].maxDx;

						if ((node[y][x].position.y - minTimeNeighbor.position.y) < -link[y][x][mtnDir].maxHrzDy)
							node[y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].maxHrzDy;
						else if ((node[y][x].position.y - minTimeNeighbor.position.y) > link[y][x][mtnDir].maxHrzDy)
							node[y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].maxHrzDy;
						
						break;

					case LEFT:
						if ((node[y][x].position.x - minTimeNeighbor.position.x) < link[y][x][mtnDir].minDx)
							node[y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].minDx;
						else if ((node[y][x].position.x - minTimeNeighbor.position.x) > link[y][x][mtnDir].maxDx)
							node[y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].maxDx;

						if ((node[y][x].position.y - minTimeNeighbor.position.y) < -link[y][x][mtnDir].maxHrzDy)
							node[y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].maxHrzDy;
						else if ((node[y][x].position.y - minTimeNeighbor.position.y) > link[y][x][mtnDir].maxHrzDy)
							node[y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].maxHrzDy;
						
						break;

					case TOP:
						if ((node[y][x].position.y - minTimeNeighbor.position.y) < link[y][x][mtnDir].minDy)
							node[y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].minDy;
						else if ((node[y][x].position.y - minTimeNeighbor.position.y) > link[y][x][mtnDir].maxDy)
							node[y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].maxDy;

						if ((node[y][x].position.x - minTimeNeighbor.position.x) < -link[y][x][mtnDir].maxVrtDx)
							node[y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxVrtDx;
						else if ((node[y][x].position.x - minTimeNeighbor.position.x) > link[y][x][mtnDir].maxVrtDx)
							node[y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].maxVrtDx;
						
						break;

					case BOTTOM:
						if ((minTimeNeighbor.position.y - node[y][x].position.y) < link[y][x][mtnDir].minDy)
							node[y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].minDy;
						else if ((minTimeNeighbor.position.y - node[y][x].position.y) > link[y][x][mtnDir].maxDy)
							node[y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].maxDy;

						if ((node[y][x].position.x - minTimeNeighbor.position.x) < -link[y][x][mtnDir].maxVrtDx)
							node[y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxVrtDx;
						else if ((node[y][x].position.x - minTimeNeighbor.position.x) > link[y][x][mtnDir].maxVrtDx)
							node[y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].maxVrtDx;

						break;
					}

					// 위치를 갱신했으니 시간도 갱신해준다.
					node[y][x].time = minTimeNeighbor.time + 1;
					isMoved = true; // 움직였다고 체크
				}
			}
		}
	}
}

// 평활화(relaxation) 과정
void Chainmail::relax()
{
	//resetTime();
}