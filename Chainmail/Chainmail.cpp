#include "Chainmail.h"
using namespace std;

// ���(Point) �ʱ�ȭ
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

// ��ũ(constraint) �ʱ�ȭ
void Chainmail::setLink()
{
	for (int y = 0; y < ARR_HEIGHT; ++y)
	{
		for (int x = 0; x < ARR_WIDTH; ++x)
		{
			// ���� r, l, t, b
			for (int n = 0; n < NUM_NEIGHBOR; ++n)
			{
				// ���� ������ ��Ȳ
				// ������ ��� ���� �� (homogeneous)
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
	// Ÿ�ӽ����� ����� ����ϱ� ������
	// propagation�� �ϱ� ���� Ÿ�� �������� �ʱ�ȭ
	resetTime();

	node[y][x].position.x += mvX;
	node[y][x].position.y += mvY;
	node[y][x].time = 0.0f;

	propagate();
	relax();
}

// time�� ���� ���� ���� ������ �����Ѵ�.
const pair<Node, Direction> Chainmail::minTimeNeighborDir(const int x, const int y) const
{
	// ������ ���(���Ѵ� �ð�)
	// ������ �� ���� ��ġ�� ��带 ���
	// ���� ���� ��带 ã���� �ǹ̰� ������ ��
	Node gabageNode;
	gabageNode.time = 100000000.f;

	// �ش� ��尡 �ƴҶ� ó��
	Node leftNode = (x == 0) ? gabageNode : node[y][x - 1];
	Node rightNode = (x == ARR_WIDTH - 1) ? gabageNode : node[y][x + 1];
	Node topNode = (y == 0) ? gabageNode : node[y - 1][x];
	Node bottomNode = (y == ARR_HEIGHT - 1) ? gabageNode : node[y + 1][x];

	// �ð��� ���� ���� ��� ã��(���� ����)
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

// ����(propagation) ����
void Chainmail::propagate()
{
	bool isMoved = true;

	while (true)
	{
		// ��ȭ�� ������ loop �׸�
		if (!isMoved)
			break;

		// ��ġ ��ȭ�� ���ٰ� �����ϰ� ����
		isMoved = false;

		// loop ����
		for (int y = 0; y < ARR_HEIGHT; ++y)
		{
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				// ��¦ ���� �ʿ� (pair �����������)
				auto pair_NeighborDir = minTimeNeighborDir(x, y);
				Node minTimeNeighbor = pair_NeighborDir.first; // minTimeNeighbor
				Direction mtnDir = pair_NeighborDir.second; // neighborDir

				// ���� ����� �ð��� �ֺ� ����� �ð� ���� ������ (��.t > �ֺ�.t + 1)
				// ����� ��ġ�� �����Ѵ�.
				if (node[y][x].time > minTimeNeighbor.time + 1)
				{
					// ���� : r l t b
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

					// ��ġ�� ���������� �ð��� �������ش�.
					node[y][x].time = minTimeNeighbor.time + 1;
					isMoved = true; // �������ٰ� üũ
				}
			}
		}
	}
}

// ��Ȱȭ(relaxation) ����
void Chainmail::relax()
{
	//resetTime();
}