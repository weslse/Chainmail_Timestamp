#include "Chainmail.h"
#include "Vec3.h"
using namespace std;

// ���(Point) �ʱ�ȭ
void Chainmail::setNode()
{
	for (int idxP = 0; idxP < NUM_MEMSET; ++idxP)
		for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
			for (int idxX = 0; idxX < ARR_WIDTH; ++idxX) {
				node[idxP][idxY][idxX].position.x = static_cast<float>(idxX);
				node[idxP][idxY][idxX].position.y = static_cast<float>(idxY);
				node[idxP][idxY][idxX].time = 100000.0f;
			}
}

// ������ ���� ����� �Լ�
void Chainmail::setLink_Homogeneous()
{
	for (int y = 0; y < ARR_HEIGHT; ++y)
		for (int x = 0; x < ARR_WIDTH; ++x)
			// ���� r, l, t, b
			for (int n = 0; n < NUM_NEIGHBOR; ++n) {

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

// ��ũ(constraint) �ʱ�ȭ
void Chainmail::setLink()
{
	for (int y = 0; y < ARR_HEIGHT; ++y)
		for (int x = 0; x < ARR_WIDTH; ++x)
			// ���� r, l, t, b
			for (int n = 0; n < NUM_NEIGHBOR; ++n) {

				// density ���� ���� ���� �ִ� �ּ� ���� ��ȭ�� �ش�.
				// ������ �ٸ� ���(heterogeneous)
				// ���� ���⿡ ���� �̿��� �е� ��
				// link�� constraint�� ��� ������ ����� �����Ͽ���.
				ubyte rNeighborDensity, lNeighborDensity, tNeighborDensity, bNeighborDensity;
				switch (n)
				{
				case Direction::RIGHT:
					rNeighborDensity = (x == ARR_WIDTH - 1) ? NULL : node[memIdx][y][x + 1].density;
					if (rNeighborDensity >= 0 && rNeighborDensity < 40) {
						link[y][x][n].minDx = 0.5f;
						link[y][x][n].maxDx = 1.5f;
						link[y][x][n].maxHrzDy = 0.5f;

						link[y][x][n].minDy = 0.5f;
						link[y][x][n].maxDy = 1.5f;
						link[y][x][n].maxVrtDx = 0.5f;
					}
					else if (rNeighborDensity >= 40 && rNeighborDensity < 100) {
						link[y][x][n].minDx = 0.95f;
						link[y][x][n].maxDx = 1.05f;
						link[y][x][n].maxHrzDy = 0.05f;

						link[y][x][n].minDy = 0.95f;
						link[y][x][n].maxDy = 1.05f;
						link[y][x][n].maxVrtDx = 0.05f;
					}
					else if (rNeighborDensity >= 100 && rNeighborDensity < 140) {
						link[y][x][n].minDx = 0.9999f;
						link[y][x][n].maxDx = 1.0001f;
						link[y][x][n].maxHrzDy = 0.0005f;

						link[y][x][n].minDy = 0.9999f;
						link[y][x][n].maxDy = 1.0001f;
						link[y][x][n].maxVrtDx = 0.0005f;
					}
					else {
						link[y][x][n].minDx = 0.99999f;
						link[y][x][n].maxDx = 1.00001f;
						link[y][x][n].maxHrzDy = 0.00001f;

						link[y][x][n].minDy = 0.99999f;
						link[y][x][n].maxDy = 1.00001f;
						link[y][x][n].maxVrtDx = 0.00001f;
					}
					break;

				case Direction::LEFT:
					lNeighborDensity = (x == 0) ? NULL : node[memIdx][y][x - 1].density;
					if (lNeighborDensity >= 0 && lNeighborDensity < 40) {
						link[y][x][n].minDx = 0.5f;
						link[y][x][n].maxDx = 1.5f;
						link[y][x][n].maxHrzDy = 0.5f;

						link[y][x][n].minDy = 0.5f;
						link[y][x][n].maxDy = 1.5f;
						link[y][x][n].maxVrtDx = 0.5f;
					}
					else if (lNeighborDensity >= 40 && lNeighborDensity < 100) {
						link[y][x][n].minDx = 0.95f;
						link[y][x][n].maxDx = 1.05f;
						link[y][x][n].maxHrzDy = 0.05f;

						link[y][x][n].minDy = 0.95f;
						link[y][x][n].maxDy = 1.05f;
						link[y][x][n].maxVrtDx = 0.05f;
					}
					else if (lNeighborDensity >= 100 && lNeighborDensity < 140) {
						link[y][x][n].minDx = 0.9999f;
						link[y][x][n].maxDx = 1.0001f;
						link[y][x][n].maxHrzDy = 0.0005f;

						link[y][x][n].minDy = 0.9999f;
						link[y][x][n].maxDy = 1.0001f;
						link[y][x][n].maxVrtDx = 0.0005f;
					}
					else {
						link[y][x][n].minDx = 0.99999f;
						link[y][x][n].maxDx = 1.00001f;
						link[y][x][n].maxHrzDy = 0.00001f;

						link[y][x][n].minDy = 0.99999f;
						link[y][x][n].maxDy = 1.00001f;
						link[y][x][n].maxVrtDx = 0.00001f;
					}
					break;

				case Direction::TOP:
					tNeighborDensity = (y == 0) ? NULL : node[memIdx][y - 1][x].density;
					if (tNeighborDensity >= 0 && tNeighborDensity < 40) {
						link[y][x][n].minDx = 0.5f;
						link[y][x][n].maxDx = 1.5f;
						link[y][x][n].maxHrzDy = 0.5f;

						link[y][x][n].minDy = 0.5f;
						link[y][x][n].maxDy = 1.5f;
						link[y][x][n].maxVrtDx = 0.5f;
					}
					else if (tNeighborDensity >= 40 && tNeighborDensity < 100) {
						link[y][x][n].minDx = 0.95f;
						link[y][x][n].maxDx = 1.05f;
						link[y][x][n].maxHrzDy = 0.05f;

						link[y][x][n].minDy = 0.95f;
						link[y][x][n].maxDy = 1.05f;
						link[y][x][n].maxVrtDx = 0.05f;
					}
					else if (tNeighborDensity >= 100 && tNeighborDensity < 140) {
						link[y][x][n].minDx = 0.9999f;
						link[y][x][n].maxDx = 1.0001f;
						link[y][x][n].maxHrzDy = 0.0005f;

						link[y][x][n].minDy = 0.9999f;
						link[y][x][n].maxDy = 1.0001f;
						link[y][x][n].maxVrtDx = 0.0005f;
					}
					else {
						link[y][x][n].minDx = 0.99999f;
						link[y][x][n].maxDx = 1.00001f;
						link[y][x][n].maxHrzDy = 0.00001f;

						link[y][x][n].minDy = 0.99999f;
						link[y][x][n].maxDy = 1.00001f;
						link[y][x][n].maxVrtDx = 0.00001f;
					}
					break;

				case Direction::BOTTOM:
					bNeighborDensity = (y == ARR_HEIGHT - 1) ? NULL : node[memIdx][y + 1][x].density;
					if (bNeighborDensity >= 0 && bNeighborDensity < 40) {
						link[y][x][n].minDx = 0.5f;
						link[y][x][n].maxDx = 1.5f;
						link[y][x][n].maxHrzDy = 0.5f;

						link[y][x][n].minDy = 0.5f;
						link[y][x][n].maxDy = 1.5f;
						link[y][x][n].maxVrtDx = 0.5f;
					}
					else if (bNeighborDensity >= 40 && bNeighborDensity < 100) {
						link[y][x][n].minDx = 0.95f;
						link[y][x][n].maxDx = 1.05f;
						link[y][x][n].maxHrzDy = 0.05f;

						link[y][x][n].minDy = 0.95f;
						link[y][x][n].maxDy = 1.05f;
						link[y][x][n].maxVrtDx = 0.05f;
					}
					else if (bNeighborDensity >= 100 && bNeighborDensity < 140) {
						link[y][x][n].minDx = 0.9999f;
						link[y][x][n].maxDx = 1.0001f;
						link[y][x][n].maxHrzDy = 0.0005f;

						link[y][x][n].minDy = 0.9999f;
						link[y][x][n].maxDy = 1.0001f;
						link[y][x][n].maxVrtDx = 0.0005f;
					}
					else {
						link[y][x][n].minDx = 0.99999f;
						link[y][x][n].maxDx = 1.00001f;
						link[y][x][n].maxHrzDy = 0.00001f;

						link[y][x][n].minDy = 0.99999f;
						link[y][x][n].maxDy = 1.00001f;
						link[y][x][n].maxVrtDx = 0.00001f;
					}
					break;
				}
			}
}

void Chainmail::setDensity(ubyte**& volume)
{
	for (int idxP = 0; idxP < NUM_MEMSET; ++idxP)
		for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
			for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
				node[idxP][idxY][idxX].density = volume[idxY][idxX];
}

void Chainmail::setTexArr()
{
	for (int y = 0; y < ARR_HEIGHT; ++y)
		for (int x = 0; x < ARR_WIDTH; ++x)
			for (int c = 0; c < 3; ++c) {
				auto idxX = static_cast<int>(node[memIdx][y][x].position.x);
				auto idxY = static_cast<int>(node[memIdx][y][x].position.y);
				if (idxY > 0.f && idxY < ARR_HEIGHT)
					if (idxX > 0.f && idxX < ARR_WIDTH)
						texArr[idxY][idxX][c] = node[memIdx][y][x].density;
			}
}

void Chainmail::resetTime()
{
	for (int idxP = 0; idxP < NUM_MEMSET; ++idxP)
		for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
			for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
				node[idxP][idxY][idxX].time = 100000.0f;
}

// (x', y') = (x + mvX, y + mvY)
void Chainmail::movePosition(const int x, const int y, const float mvX, const float mvY)
{
	// Ÿ�ӽ����� ����� ����ϱ� ������
	// propagation�� �ϱ� ���� Ÿ�� �������� �ʱ�ȭ
	resetTime();

	node[memIdx][y][x].position.x += mvX;
	node[memIdx][y][x].position.y += mvY;
	node[memIdx][y][x].time = 0.0f;

	propagate();
	relax();
	
	// ���ŵ� ��ġ�� �ؽ��� �迭�� �ٸ���.
	setTexArr(); 
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
	Node leftNode = (x == 0) ? gabageNode : node[memIdx][y][x - 1];
	Node rightNode = (x == ARR_WIDTH - 1) ? gabageNode : node[memIdx][y][x + 1];
	Node topNode = (y == 0) ? gabageNode : node[memIdx][y - 1][x];
	Node bottomNode = (y == ARR_HEIGHT - 1) ? gabageNode : node[memIdx][y + 1][x];

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
			for (int x = 0; x < ARR_WIDTH; ++x) {

				// ��¦ ���� �ʿ� (pair �����������-> CUDA�� �Ѿ�� �ٷ� �����ϱ� ����)
				auto pair_NeighborDir = minTimeNeighborDir(x, y);
				Node minTimeNeighbor = pair_NeighborDir.first; // minTimeNeighbor
				Direction mtnDir = pair_NeighborDir.second;    // neighborDir

				// ���� ����� �ð��� �ֺ� ����� �ð� ���� ������ (��.t > �ֺ�.t + 1)
				// ����� ��ġ�� �����Ѵ�.
				if (node[memIdx][y][x].time > minTimeNeighbor.time + 1) {

					// ���� : r l t b
					switch (mtnDir) {

					case RIGHT:
						if ((minTimeNeighbor.position.x - node[memIdx][y][x].position.x) < link[y][x][mtnDir].minDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].minDx; // ������庸�� ���� ���� ��ġ�� ��������
						else if ((minTimeNeighbor.position.x - node[memIdx][y][x].position.x) > link[y][x][mtnDir].maxDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxDx; // �ִ� ���̸�ŭ ���� ���Ѵ�. �ƴϸ� ���� Ŀ���� ����

						if ((node[memIdx][y][x].position.y - minTimeNeighbor.position.y) < -link[y][x][mtnDir].maxHrzDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].maxHrzDy;
						else if ((node[memIdx][y][x].position.y - minTimeNeighbor.position.y) > link[y][x][mtnDir].maxHrzDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].maxHrzDy;

						break;

					case LEFT:
						if ((node[memIdx][y][x].position.x - minTimeNeighbor.position.x) < link[y][x][mtnDir].minDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].minDx;
						else if ((node[memIdx][y][x].position.x - minTimeNeighbor.position.x) > link[y][x][mtnDir].maxDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].maxDx;

						if ((node[memIdx][y][x].position.y - minTimeNeighbor.position.y) < -link[y][x][mtnDir].maxHrzDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].maxHrzDy;
						else if ((node[memIdx][y][x].position.y - minTimeNeighbor.position.y) > link[y][x][mtnDir].maxHrzDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].maxHrzDy;

						break;

					case TOP:
						if ((node[memIdx][y][x].position.y - minTimeNeighbor.position.y) < link[y][x][mtnDir].minDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].minDy;
						else if ((node[memIdx][y][x].position.y - minTimeNeighbor.position.y) > link[y][x][mtnDir].maxDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y + link[y][x][mtnDir].maxDy;

						if ((node[memIdx][y][x].position.x - minTimeNeighbor.position.x) < -link[y][x][mtnDir].maxVrtDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxVrtDx;
						else if ((node[memIdx][y][x].position.x - minTimeNeighbor.position.x) > link[y][x][mtnDir].maxVrtDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].maxVrtDx;

						break;

					case BOTTOM:
						if ((minTimeNeighbor.position.y - node[memIdx][y][x].position.y) < link[y][x][mtnDir].minDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].minDy; // ���ҳ�庸�� ���� ���� ��ġ�� ��������
						else if ((minTimeNeighbor.position.y - node[memIdx][y][x].position.y) > link[y][x][mtnDir].maxDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].maxDy; // �ִ� ���̸�ŭ ���� ���Ѵ�. �ƴϸ� ���� Ŀ���� ����

						if ((node[memIdx][y][x].position.x - minTimeNeighbor.position.x) < -link[y][x][mtnDir].maxVrtDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxVrtDx;
						else if ((node[memIdx][y][x].position.x - minTimeNeighbor.position.x) > link[y][x][mtnDir].maxVrtDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].maxVrtDx;

						break;
					}

					// ��ġ�� ���������� �ð��� �������ش�.
					node[memIdx][y][x].time = minTimeNeighbor.time + 1;
					isMoved = true; // �������ٰ� üũ
				}
			}
	}
}

// ��Ȱȭ(relaxation) ����
// ���ϴ� �Լ��� �ּ��� �����ϰ�
// �ٸ� �Լ��� �ּ��� �־� ������ ��
void Chainmail::relax()
{
	//relax_spring();
	relax_sein();
}

// ���� �̿��� ���. ������
void Chainmail::relax_spring()
{
	// ������ �ݺ� Ƚ�� ����
	// �������� �𵨿� ���� Ƚ���� ����.
	// �� 10ȸ ���� �ణ�� �������� ���� (���� ��Ʈ�� ����)
	for (int i = 0; i < 8; i++) {
		for (int y = 0; y < ARR_HEIGHT; ++y) {
			for (int x = 0; x < ARR_WIDTH; ++x) {

				Node target = node[memIdx][y][x];

				// EPICENTER�� relaxation���� ����
				if (y == TARGET_Y && x == TARGET_X) {

					node[(memIdx + 1) & 1][y][x].position.x = target.position.x;
					node[(memIdx + 1) & 1][y][x].position.y = target.position.y;
					continue;
				}

				// ������ ��� ����
				Node noMeaningNode = target;
				Node nRight = (x == ARR_WIDTH - 1) ? noMeaningNode : node[memIdx][y][x + 1];
				Node nLeft = (x == 0) ? noMeaningNode : node[memIdx][y][x - 1];
				Node nTop = (y == 0) ? noMeaningNode : node[memIdx][y - 1][x];
				Node nBottom = (y == ARR_HEIGHT - 1) ? noMeaningNode : node[memIdx][y + 1][x];

				// ������ ��ġ ����
				Vec3 targetPos = target.position;
				Vec3 nRPos = nRight.position;
				Vec3 nLPos = nLeft.position;
				Vec3 nTPos = nTop.position;
				Vec3 nBPos = nBottom.position;

				// �̿������� k��(ź�����)
				float kRight = (x == ARR_WIDTH - 1) ? 0.f : 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx);
				float kLeft = (x == 0) ? 0.f : 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx);
				float kTop = (y == 0) ? 0.f : 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy);
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy);

				// k���� ���
				// �̿������ k���� ���ο� ����� �� �ֵ��� 
				// �Ʒ��� kProp ����� ���Ͽ� ����Ѵ�.(k�� ����)
				constexpr float kProp = 0.05f;
				const float kDampRight = kProp * kRight;
				const float kDampLeft = kProp * kLeft;
				const float kDampTop = kProp * kTop;
				const float kDampBottom = kProp * kBottom;

				// ������ ������ ź���� + �����
				Vec3 vRight = nRPos - targetPos;
				const float vRightLen = vRight.getLength();
				Vec3 velRight = ((nRight.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vRight * (1.f / (nRight.time - target.time));
				vRight.normalize();
				Vec3 fRight = vRight * ((ORIGIN_LEN - vRightLen) * kRight - (velRight * vRight) * kDampRight);  // ź���� + �����

				// ���� ������ ź���� + �����
				Vec3 vLeft = nLPos - targetPos;
				const float vLeftLen = vLeft.getLength();
				Vec3 velLeft = ((nLeft.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vLeft * (1.f / (nLeft.time - target.time));
				vLeft.normalize();
				Vec3 fLeft = vLeft * ((ORIGIN_LEN - vLeftLen) * kLeft - (velLeft * vLeft) * kDampLeft);  // ź���� + �����

				// ���� ������ ź���� + �����
				Vec3 vTop = nTPos - targetPos;
				const float vTopLen = vTop.getLength();
				Vec3 velTop = ((nTop.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vTop * (1.f / (nTop.time - target.time));
				vTop.normalize();
				Vec3 fTop = vTop * ((ORIGIN_LEN - vTopLen) * kTop - (velTop * vTop) * kDampTop);  // ź���� + �����

				// �Ʒ��� ������ ź���� + �����
				Vec3 vBottom = nBPos - targetPos;
				const float vBottomLen = vBottom.getLength();
				Vec3 velBottom = ((nBottom.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vBottom * (1.f / (nBottom.time - target.time));
				vBottom.normalize();
				Vec3 fBottom = vBottom * ((ORIGIN_LEN - vBottomLen) * kBottom - (velBottom * vBottom) * kDampBottom);  // ź���� + �����

				// ���� ź������ ��� ����� �ݴ�� �ۿ�
				Vec3 fTotal = -(fRight + fLeft + fTop + fBottom);

				// ũ�Ⱑ �ʹ� ���� �ʴٸ� �����ӿ� �����Ѵ�.
				if (fTotal.getMagnitude() > 0.001f) {

					float delta = 0.001f; // delta  = (1.f/ m * deltat *deltat);
					Vec3 mov = fTotal * delta;
					targetPos += mov;
				}

				// pingpong scheme
				node[(memIdx + 1) & 1][y][x].position.x = targetPos.x;
				node[(memIdx + 1) & 1][y][x].position.y = targetPos.y;
			} // end for(i)
		} // end for(y)

		// 0->1, 1->0
		memIdx = (++memIdx) & 1;
	} // end for(i)
}


// �������� �̿��� ���=�շ��� 0�� ������ �ּ����� ���ϴ� �������� Ǭ ���. �ٻ���
// �࿡ ���� ���� �ٸ���
void Chainmail::relax_sein()
{
	// �� ���� �����Ͽ� �ε巯�� ���� ����
	// �������� �����̱� ������ �̷��� ��������
	// �������� ��� ����� property ������ �����ؾ��� ��
	//constexpr float ELASTICITY = 0.7f;

	// ���� ���� ppt ����(2�����϶� �ִ� 4, 3���� : 6)
	constexpr float MAX_F = 4.f;

	// spring ����� ���� �ݺ�Ƚ���� ����.
	for (int i = 0; i < 3; ++i) {
		for (int y = 0; y < ARR_HEIGHT; ++y) {
			for (int x = 0; x < ARR_WIDTH; ++x) {

				Vec3 targetPos = node[memIdx][y][x].position;

				// EPICENTER�� relaxation���� ����
				if (y == TARGET_Y && x == TARGET_X) {

					node[(memIdx + 1) & 1][y][x].position.x = targetPos.x;
					node[(memIdx + 1) & 1][y][x].position.y = targetPos.y;
					continue;
				}

				// �̿������ ����
				Vec3 noMeaningVec = targetPos;
				Vec3 nRPos = (x == ARR_WIDTH - 1) ? noMeaningVec : node[memIdx][y][x + 1].position;
				Vec3 nLPos = (x == 0) ? noMeaningVec : node[memIdx][y][x - 1].position;
				Vec3 nTPos = (y == 0) ? noMeaningVec : node[memIdx][y - 1][x].position;
				Vec3 nBPos = (y == ARR_HEIGHT - 1) ? noMeaningVec : node[memIdx][y + 1][x].position;

				// �̿���忡 ���� k��
				float kRight = (x == ARR_WIDTH - 1) ? 0.f : (1.f / ((link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx) * 0.5f + EPS));
				float kLeft = (x == 0) ? 0.f : (1.f / ((link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx) * 0.5f + EPS));
				float kTop = (y == 0) ? 0.f : (1.f / ((link[y][x][TOP].maxDy - link[y][x][TOP].minDy) * 0.5f + EPS));
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : (1.f / ((link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy) * 0.5f + EPS));
				float kSumInv = 1.f / (kRight + kLeft + kTop + kBottom);

				// k ���� �����ؾ��� ����� �̸� ����Ͽ� ����
				// ���� ���� �ڵ� ����
				Vec3 plasticity = { fminf(link[y][x][RIGHT].maxVrtDx, (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx) * 0.5f),
					fminf(link[y][x][BOTTOM].maxHrzDy, (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy) * 0.5f), 0.0f };

				// ������(heterogeneous)�� ����̹Ƿ� elasticity�� �����־����
				// �� ������ max-min���߿� ���� ���� ��(���� ������ ��ü)�� 
				// �ε巯�������� ������� ����
				float elasticity = fminf(
					fminf((link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx), (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx)),
					fminf((link[y][x][TOP].maxDy - link[y][x][TOP].minDy), (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy))
				);

				plasticity *= (1.f - powf(elasticity, 0.16f));

				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // �� ���� �ະ�� �ٻ�
				float goal_Fx = ((nRPos.x - 1.f) * kRight) + ((nLPos.x + 1.f) * kLeft) + (nTPos.x * kTop) + (nBPos.x * kBottom);
				float goal_Fy = (nRPos.y * kRight) + (nLPos.y * kLeft) + ((nTPos.y + 1.f) * kTop) + ((nBPos.y - 1.f) * kBottom);
				float goal_Fz = 0.f;

				//// ���� ����
				Vec3 relax = Vec3(goal_Fx, goal_Fy, goal_Fz) * kSumInv;
				Vec3 delta = relax - targetPos;
				Vec3 movF = delta * (kRight + kLeft + kTop + kBottom);
				float movF_size = movF.getLength();
				movF.normalize();

				//if (mov �� �����ϰ� Ŀ�߸� > T)
				//�����ϰԶ� k���� �����Ѵ�. k���� �ʹ� ũ�� ������ ��ü�̹Ƿ� T�� 0�� ����� ���̴�.: ������ ���
				if (movF_size > MAX_F * plasticity.x)
					targetPos.x += ((movF.x * (movF_size - MAX_F * plasticity.x)) * kSumInv);
				if (movF_size > MAX_F * plasticity.y)
					targetPos.y += ((movF.y * (movF_size - MAX_F * plasticity.y)) * kSumInv);

				// pingpong scheme
				node[(memIdx + 1) & 1][y][x].position.x = targetPos.x;
				node[(memIdx + 1) & 1][y][x].position.y = targetPos.y;
			} // end for(x)
		} // end for(y)

		// 0->1, 1->0
		memIdx = (++memIdx) & 1;
	} //end for(i)
}
