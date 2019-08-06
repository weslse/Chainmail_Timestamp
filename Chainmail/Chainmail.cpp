#include "Chainmail.h"
#include "Vec3.h"
using namespace std;

// ���(Point) �ʱ�ȭ
void Chainmail::setNode()
{
	for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
		for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
		{
			node[idxY][idxX].position.x = static_cast<float>(idxX);
			node[idxY][idxX].position.y = static_cast<float>(idxY);
			node[idxY][idxX].time = 100000.0f;
		}
}

// ��ũ(constraint) �ʱ�ȭ
void Chainmail::setLink()
{
	for (int y = 0; y < ARR_HEIGHT; ++y)
		for (int x = 0; x < ARR_WIDTH; ++x)
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

void Chainmail::resetTime()
{
	for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
		for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
			node[idxY][idxX].time = nodeCopy[idxY][idxX].time = 100000.0f;
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
	//relax();
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
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				// ��¦ ���� �ʿ� (pair �����������-> CUDA�� �Ѿ�� �ٷ� �����ϱ� ����)
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
							node[y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].minDx; // ������庸�� ���� ���� ��ġ�� ��������
						else if ((minTimeNeighbor.position.x - node[y][x].position.x) > link[y][x][mtnDir].maxDx)
							node[y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxDx; // �ִ� ���̸�ŭ ���� ���Ѵ�. �ƴϸ� ���� Ŀ���� ����

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
							node[y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].minDy; // ���ҳ�庸�� ���� ���� ��ġ�� ��������
						else if ((minTimeNeighbor.position.y - node[y][x].position.y) > link[y][x][mtnDir].maxDy)
							node[y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].maxDy; // �ִ� ���̸�ŭ ���� ���Ѵ�. �ƴϸ� ���� Ŀ���� ����

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

// ��Ȱȭ(relaxation) ����
// �ڵ� ���� �ʿ�
void Chainmail::relax()
{
	relax_spring();
	//relax_sein();
}

// ���� �̿��� ���. ������
void Chainmail::relax_spring()
{
	const float eps = 0.000001f;

	// false�� node�� ���� nodeCopy�� �� ����, cnt����(true�� ����� nodeCopy ������)
	if (!pingPongCnt)
	{
		//for (int z = 0; z < ARR_DEPTH; ++z)
		for (int y = 0; y < ARR_HEIGHT; ++y)
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				Vec3 targetPos = node[y][x].position;
				Vec3 noMeaningVec = targetPos;
				Vec3 nRight = (x == ARR_WIDTH - 1) ? noMeaningVec : node[y][x + 1];
				Vec3 nLeft = (x == 0) ? noMeaningVec : node[y][x - 1];
				Vec3 nTop = (y == 0) ? noMeaningVec : node[y - 1][x];
				Vec3 nBottom = (y == ARR_HEIGHT - 1) ? noMeaningVec : node[y + 1][x];

				float kRight = (x == ARR_WIDTH - 1) ? 0.f : 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx + eps);
				float kLeft = (x == 0) ? 0.f : 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx + eps);
				float kTop = (y == 0) ? 0.f : 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy + eps);
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy + eps);

				// k���� ���
				const float kDampRight = 1.f * kRight;
				const float kDampLeft = 1.f * kLeft;
				const float kDampTop = 1.f * kTop;
				const float kDampBottom = 1.f * kBottom;

				Vec3 rightF = nRight - targetPos;
				rightF.normalize();
				rightF = (rightF * (1 - (nRight - targetPos).dst()) * kRight) //   ź����
					+ rightF * ((nRight - targetPos)*rightF) * kDampRight;    // + �����

				Vec3 leftF = nLeft - targetPos;
				leftF.normalize();
				leftF = leftF * (1 - (nLeft - targetPos).dst()) * kLeft
					+ leftF * ((nLeft - targetPos)*leftF) * kDampLeft;

				Vec3 topF = nTop - targetPos;
				topF.normalize();
				topF = topF * (1 - (nTop - targetPos).dst()) * kTop
					+ topF * ((nTop - targetPos)*topF) * kDampTop;

				Vec3 bottomF = nBottom - targetPos;
				bottomF.normalize();
				bottomF = bottomF * (1 - (nBottom - targetPos).dst()) * kBottom
					+ bottomF * ((nBottom - targetPos)*bottomF) * kDampBottom;

				Vec3 totalF = rightF + leftF + topF + bottomF;

				// ������ �� �ִ� ����?
				//if (fabsf((1.f / (kRight + eps) - 1.f / (kLeft + eps)) >= fabsf(totalF.x) && fabsf(totalF.x) >= 0.f)
				//	&& fabsf((1.f / (kBottom + eps) - 1.f / (kTop + eps)) >= fabsf(totalF.y) && fabsf(totalF.y) >= 0.f))
				//{
					float delta = 0.001f;
					Vec3 mov = totalF * delta;//(/ m * deltat *deltat);
					targetPos += mov;
				//}

				nodeCopy[y][x].position.x = targetPos.x;
				nodeCopy[y][x].position.y = targetPos.y;
				//node[y][x].position.z = targetPos.z;
			}
	}
	// true�� nodeCopy�� ���� node�� �� ����, cnt����(false�� ����� node ������)
	else
	{
		for (int y = 0; y < ARR_HEIGHT; ++y)
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				Vec3 targetPos = nodeCopy[y][x].position;
				Vec3 noMeaningVec = targetPos;
				Vec3 nRight = (x == ARR_WIDTH - 1) ? noMeaningVec : nodeCopy[y][x + 1];
				Vec3 nLeft = (x == 0) ? noMeaningVec : nodeCopy[y][x - 1];
				Vec3 nTop = (y == 0) ? noMeaningVec : nodeCopy[y - 1][x];
				Vec3 nBottom = (y == ARR_HEIGHT - 1) ? noMeaningVec : nodeCopy[y + 1][x];

				float kRight = (x == ARR_WIDTH - 1) ? 0.f : 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx + eps);
				float kLeft = (x == 0) ? 0.f : 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx + eps);
				float kTop = (y == 0) ? 0.f : 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy + eps);
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy + eps);

				// k���� ���
				const float kDampRight = 1.f * kRight;
				const float kDampLeft = 1.f * kLeft;
				const float kDampTop = 1.f * kTop;
				const float kDampBottom = 1.f * kBottom;

				Vec3 rightF = nRight - targetPos;
				rightF.normalize();
				rightF = (rightF * (1 - (nRight - targetPos).dst()) * kRight) //   ź����
					+ rightF * ((nRight - targetPos)*rightF) * kDampRight;    // + �����

				Vec3 leftF = nLeft - targetPos;
				leftF.normalize();
				leftF = leftF * (1 - (nLeft - targetPos).dst()) * kLeft
					+ leftF * ((nLeft - targetPos)*leftF) * kDampLeft;

				Vec3 topF = nTop - targetPos;
				topF.normalize();
				topF = topF * (1 - (nTop - targetPos).dst()) * kTop
					+ topF * ((nTop - targetPos)*topF) * kDampTop;

				Vec3 bottomF = nBottom - targetPos;
				bottomF.normalize();
				bottomF = bottomF * (1 - (nBottom - targetPos).dst()) * kBottom
					+ bottomF * ((nBottom - targetPos)*bottomF) * kDampBottom;

				Vec3 totalF = rightF + leftF + topF + bottomF;

				// ������ �� �ִ� ����?
				//if (fabsf((1.f / (kRight + eps) - 1.f / (kLeft + eps)) >= fabsf(totalF.x) && fabsf(totalF.x) >= 0.f)
				//	&& fabsf((1.f / (kBottom + eps) - 1.f / (kTop + eps)) >= fabsf(totalF.y) && fabsf(totalF.y) >= 0.f))
				//{
				float delta = 0.001f;
				Vec3 mov = totalF * delta;//(/ m * deltat *deltat);
				targetPos += mov;
				//}

				node[y][x].position.x = targetPos.x;
				node[y][x].position.y = targetPos.y;
				//node[y][x].position.z = targetPos.z;
			}
	}

	pingPongCnt = !pingPongCnt;
}


// �������� �̿��� ���=�շ��� 0�� ������ �ּ����� ���ϴ� �������� Ǭ ���. �ٻ���
// �࿡ ���� ���� �ٸ���
void Chainmail::relax_sein()
{
	const float maxF = 4.f;
	const float eps = 0.000001f;

	// false�� node�� ���� nodeCopy�� �� ����, cnt����(true�� ����� nodeCopy ������)
	if (!pingPongCnt)
	{
		for (int y = 0; y < ARR_HEIGHT; ++y)
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				Vec3 targetPos = node[y][x].position;
				Vec3 noMeaningVec = targetPos;
				Vec3 nRight = (x == ARR_WIDTH - 1) ? noMeaningVec : node[y][x + 1];
				Vec3 nLeft = (x == 0) ? noMeaningVec : node[y][x - 1];
				Vec3 nTop = (y == 0) ? noMeaningVec : node[y - 1][x];
				Vec3 nBottom = (y == ARR_HEIGHT - 1) ? noMeaningVec : node[y + 1][x];

				float kRight = (x == ARR_WIDTH - 1) ? 0.f : 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx + eps);
				float kLeft = (x == 0) ? 0.f : 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx + eps);
				float kTop = (y == 0) ? 0.f : 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy + eps);
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy + eps);
				float kSum = kRight + kLeft + kTop + kBottom;
				float kSumInv = 1.f / kSum;

				/*Vec3 plasticity = { fminf(link[y][x][RIGHT].maxVrtDx, (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx) * 0.5f),
					fminf(link[y][x][BOTTOM].maxVrtDx, (link[y][x][BOTTOM].maxDx - link[y][x][BOTTOM].minDx) * 0.5f), 0.0f };
				plasticity *= (1.f - powf(0.7f, 0.16f));*/
				// k ���� �����ؾ���
				// ����� ������ ��
				Vec3 plasticity = { 0.00005f, 0.00005f, 0.f }; 

				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // �� ���� �ະ�� �ٻ�
				float goal_Fx = (nRight.x - 1 - targetPos.x)*kRight + (nLeft.x + 1 - targetPos.x)*kLeft
					+ (nTop.x - targetPos.x)*kTop + (nBottom.x - targetPos.x)*kBottom;
				float goal_Fy = (nRight.y - targetPos.y)*kRight + (nLeft.y - targetPos.y)*kLeft
					+ (nTop.y + 1 - targetPos.y)*kTop + (nBottom.y - 1 - targetPos.y)*kBottom;
				float goal_Fz = 0.f;

				Vec3 goal_F = Vec3(goal_Fx, goal_Fy, goal_Fz);
				Vec3 goal_pos = goal_F * kSumInv;

				//// ���� ����
				Vec3 movF = goal_F;
				float movF_size = movF.getLength();
				movF.normalize();
				float movFx = movF.x;
				float movFy = movF.y;

				//if (mov �� �����ϰ� Ŀ�߸� > T)
				//	//, �����ϰԶ� k���� �����Ѵ�. k���� �ʹ� ũ�� ������ ��ü�̹Ƿ� T�� 0�� ����� ���̴�.: ������ ���

				/*if (movF_size < -plasticity.x)
					targetPos.x -= ((movF.x * (movF_size + plasticity.x)) * kSumInv);*/
				if (movF_size > maxF * plasticity.x)
					targetPos.x += ((movF.x * (movF_size - maxF * plasticity.x)) * kSumInv);

				/*	if (movF_size < -plasticity.y)
						targetPos.y -= ((movF.y * (movF_size + plasticity.y)) * kSumInv);*/
				if (movF_size > maxF * plasticity.y)
					targetPos.y += ((movF.y * (movF_size - maxF * plasticity.y)) * kSumInv);

				nodeCopy[y][x].position.x = targetPos.x;
				nodeCopy[y][x].position.y = targetPos.y;
			}
	}
	// true�� nodeCopy�� ���� node�� �� ����, cnt����(false�� ����� node ������)
	else
	{
		for (int y = 0; y < ARR_HEIGHT; ++y)
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				Vec3 targetPos = nodeCopy[y][x].position;
				Vec3 noMeaningVec = targetPos;
				Vec3 nRight = (x == ARR_WIDTH - 1) ? noMeaningVec : nodeCopy[y][x + 1];
				Vec3 nLeft = (x == 0) ? noMeaningVec : nodeCopy[y][x - 1];
				Vec3 nTop = (y == 0) ? noMeaningVec : nodeCopy[y - 1][x];
				Vec3 nBottom = (y == ARR_HEIGHT - 1) ? noMeaningVec : nodeCopy[y + 1][x];

				float kRight = (x == ARR_WIDTH - 1) ? 0.f : 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx + eps);
				float kLeft = (x == 0) ? 0.f : 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx + eps);
				float kTop = (y == 0) ? 0.f : 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy + eps);
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy + eps);
				float kSum = kRight + kLeft + kTop + kBottom;
				float kSumInv = 1.f / kSum;

				//	Vec3 plasticity = { fminf(link[y][x][RIGHT].maxVrtDx, (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx) * 0.5f),
				//fminf(link[y][x][BOTTOM].maxVrtDx, (link[y][x][BOTTOM].maxDx - link[y][x][BOTTOM].minDx) * 0.5f), 0.0f };
				//	//plasticity *= (1.f - powf(0.7f, 0.16f));
				// k ���� �����ؾ���
				// ����� ������ ��
				Vec3 plasticity = { 0.00005f, 0.00005f, 0.f };


				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // �� ���� �ະ�� �ٻ�
				float goal_Fx = (nRight.x - 1 - targetPos.x)*kRight + (nLeft.x + 1 - targetPos.x)*kLeft
					+ (nTop.x - targetPos.x)*kTop + (nBottom.x - targetPos.x)*kBottom;
				float goal_Fy = (nRight.y - targetPos.y)*kRight + (nLeft.y - targetPos.y)*kLeft
					+ (nTop.y + 1 - targetPos.y)*kTop + (nBottom.y - 1 - targetPos.y)*kBottom;
				float goal_Fz = 0.f;

				Vec3 goal_F = Vec3(goal_Fx, goal_Fy, goal_Fz);
				Vec3 goal_pos = goal_F * kSumInv;

				//// ���� ����
				Vec3 movF = goal_F;
				float movF_size = movF.getLength();
				movF.normalize();
				float movFx = movF.x;
				float movFy = movF.y;

				//if (mov �� �����ϰ� Ŀ�߸� > T)
				//	//, �����ϰԶ� k���� �����Ѵ�. k���� �ʹ� ũ�� ������ ��ü�̹Ƿ� T�� 0�� ����� ���̴�.: ������ ���

				/*if (movF_size < -plasticity.x)
					targetPos.x -= ((movF.x * (movF_size + plasticity.x)) * kSumInv);*/
				if (movF_size > maxF * plasticity.x)
					targetPos.x += ((movF.x * (movF_size - maxF * plasticity.x)) * kSumInv);

				/*	if (movF_size < -plasticity.y)
						targetPos.y -= ((movF.y * (movF_size + plasticity.y)) * kSumInv);*/
				if (movF_size > maxF * plasticity.y)
					targetPos.y += ((movF.y * (movF_size - maxF * plasticity.y)) * kSumInv);

				node[y][x].position.x = targetPos.x;
				node[y][x].position.y = targetPos.y;
			}
	}

	pingPongCnt = !pingPongCnt;
}
