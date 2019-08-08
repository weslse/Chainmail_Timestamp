#include "Chainmail.h"
#include "Vec3.h"
using namespace std;

// 노드(Point) 초기화
void Chainmail::setNode()
{
	for (int idxP = 0; idxP < NUM_MEMSET; ++idxP)
		for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
			for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
			{
				node[idxP][idxY][idxX].position.x = static_cast<float>(idxX);
				node[idxP][idxY][idxX].position.y = static_cast<float>(idxY);
				node[idxP][idxY][idxX].time = 100000.0f;
			}
}

// 링크(constraint) 초기화
void Chainmail::setLink()
{
	for (int y = 0; y < ARR_HEIGHT; ++y)
		for (int x = 0; x < ARR_WIDTH; ++x)
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
	// 타임스탬프 방법을 사용하기 때문에
	// propagation을 하기 전에 타임 스탬프를 초기화
	resetTime();

	node[memIdx][y][x].position.x += mvX;
	node[memIdx][y][x].position.y += mvY;
	node[memIdx][y][x].time = 0.0f;

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
	Node leftNode = (x == 0) ? gabageNode : node[memIdx][y][x - 1];
	Node rightNode = (x == ARR_WIDTH - 1) ? gabageNode : node[memIdx][y][x + 1];
	Node topNode = (y == 0) ? gabageNode : node[memIdx][y - 1][x];
	Node bottomNode = (y == ARR_HEIGHT - 1) ? gabageNode : node[memIdx][y + 1][x];

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
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				// 살짝 수정 필요 (pair 사용하지말기-> CUDA로 넘어갈때 바로 변경하기 힘듬)
				auto pair_NeighborDir = minTimeNeighborDir(x, y);
				Node minTimeNeighbor = pair_NeighborDir.first; // minTimeNeighbor
				Direction mtnDir = pair_NeighborDir.second; // neighborDir

				// 현재 노드의 시간이 주변 노드의 시간 보다 느릴때 (나.t > 주변.t + 1)
				// 노드의 위치를 갱신한다.
				if (node[memIdx][y][x].time > minTimeNeighbor.time + 1)
				{
					// 순서 : r l t b
					switch (mtnDir)
					{
					case RIGHT:
						if ((minTimeNeighbor.position.x - node[memIdx][y][x].position.x) < link[y][x][mtnDir].minDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].minDx; // 우측노드보다 값이 작은 위치로 보내야함
						else if ((minTimeNeighbor.position.x - node[memIdx][y][x].position.x) > link[y][x][mtnDir].maxDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxDx; // 최대 차이만큼 빼서 더한다. 아니면 값이 커지는 오류

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
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].minDy; // 바텀노드보다 값이 작은 위치로 보내야함
						else if ((minTimeNeighbor.position.y - node[memIdx][y][x].position.y) > link[y][x][mtnDir].maxDy)
							node[memIdx][y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].maxDy; // 최대 차이만큼 빼서 더한다. 아니면 값이 커지는 오류

						if ((node[memIdx][y][x].position.x - minTimeNeighbor.position.x) < -link[y][x][mtnDir].maxVrtDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxVrtDx;
						else if ((node[memIdx][y][x].position.x - minTimeNeighbor.position.x) > link[y][x][mtnDir].maxVrtDx)
							node[memIdx][y][x].position.x = minTimeNeighbor.position.x + link[y][x][mtnDir].maxVrtDx;

						break;
					}

					// 위치를 갱신했으니 시간도 갱신해준다.
					node[memIdx][y][x].time = minTimeNeighbor.time + 1;
					isMoved = true; // 움직였다고 체크
				}
			}
	}
}

// 평활화(relaxation) 과정
void Chainmail::relax()
{
	relax_spring();
	//relax_sein();
}

// 힘을 이용한 방식. 고전적
// springF를 자료 구조에 추가해야함
void Chainmail::relax_spring()
{
	for (int i = 0; i < 8; i++) // 임의의 반복 횟수 제한
	{
		for (int y = 0; y < ARR_HEIGHT; ++y)
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				Node target = node[memIdx][y][x];

				// EPICENTER는 relaxation에서 제외
				if (y == ARR_HEIGHT && x == ARR_WIDTH)
				{
					node[(memIdx + 1) & 1][y][x].position.x = target.position.x;
					node[(memIdx + 1) & 1][y][x].position.y = target.position.y;
					continue;
				}

				Node noMeaningNode = target;
				Node nRight = (x == ARR_WIDTH - 1) ? noMeaningNode : node[memIdx][y][x + 1];
				Node nLeft = (x == 0) ? noMeaningNode : node[memIdx][y][x - 1];
				Node nTop = (y == 0) ? noMeaningNode : node[memIdx][y - 1][x];
				Node nBottom = (y == ARR_HEIGHT - 1) ? noMeaningNode : node[memIdx][y + 1][x];

				Vec3 targetPos = target.position;
				Vec3 nRPos = nRight.position;
				Vec3 nLPos = nLeft.position;
				Vec3 nTPos = nTop.position;
				Vec3 nBPos = nBottom.position;

				float kRight = (x == ARR_WIDTH - 1) ? 0.f : 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx);
				float kLeft = (x == 0) ? 0.f : 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx);
				float kTop = (y == 0) ? 0.f : 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy);
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy);

				// k값에 비례
				const float kDampRight = .1f * kRight;
				const float kDampLeft = .1f *kLeft;
				const float kDampTop = .1f *kTop;
				const float kDampBottom = .1f *kBottom;

				Vec3 vRight = nRPos - targetPos;
				const float vRightLen = vRight.getLength();
				Vec3 velRight = ((nRight.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vRight * (1.f / (nRight.time - target.time));
				vRight.normalize();
				Vec3 fRight = vRight * ((ORIGIN_LEN - vRightLen) * kRight - (velRight * vRight) * kDampRight);  // 탄성력 + 감쇠력

				Vec3 vLeft = nLPos - targetPos;
				const float vLeftLen = vLeft.getLength();
				Vec3 velLeft = ((nLeft.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vLeft * (1.f / (nLeft.time - target.time));
				vLeft.normalize();
				Vec3 fLeft = vLeft * ((ORIGIN_LEN - vLeftLen) * kLeft - (velLeft * vLeft) * kDampLeft);  // 탄성력 + 감쇠력

				Vec3 vTop = nTPos - targetPos;
				const float vTopLen = vTop.getLength();
				Vec3 velTop = ((nTop.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vTop * (1.f / (nTop.time - target.time));
				vTop.normalize();
				Vec3 fTop = vTop * ((ORIGIN_LEN - vTopLen) * kTop - (velTop * vTop) * kDampTop);  // 탄성력 + 감쇠력

				Vec3 vBottom = nBPos - targetPos;
				const float vBottomLen = vBottom.getLength();
				Vec3 velBottom = ((nBottom.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vBottom * (1.f / (nBottom.time - target.time));
				vBottom.normalize();
				Vec3 fBottom = vBottom * ((ORIGIN_LEN - vBottomLen) * kBottom - (velBottom * vBottom) * kDampBottom);  // 탄성력 + 감쇠력

				Vec3 fTotal = -(fRight + fLeft + fTop + fBottom);

				if (fTotal.dst() > 0.001f)
				{
					float delta = 0.001f;
					Vec3 mov = fTotal * delta;//(/ m * deltat *deltat);
					targetPos += mov;
				}

				node[(memIdx + 1) & 1][y][x].position.x = targetPos.x;
				node[(memIdx + 1) & 1][y][x].position.y = targetPos.y;
			}

		memIdx = (++memIdx) & 1;
	}
}


// 에너지를 이용한 방식=합력이 0인 에너지 최소점을 구하는 방정식을 푼 방법. 근사적
// 축에 따라 각기 다르게
void Chainmail::relax_sein()
{
	const float maxF = 4.f; // 원진 누나 ppt 참고(2차원일때 최대 4, 3차원 : 6)

	for (int i = 0; i < 3; ++i)
	{
		for (int y = 0; y < ARR_HEIGHT; ++y)
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				Vec3 targetPos = node[memIdx][y][x].position;

				// EPICENTER는 relaxation에서 제외
				if (y == ARR_HEIGHT && x == ARR_WIDTH)
				{
					node[(memIdx + 1) & 1][y][x].position.x = targetPos.x;
					node[(memIdx + 1) & 1][y][x].position.y = targetPos.y;
					continue;
				}

				Vec3 noMeaningVec = targetPos;
				Vec3 nRPos = (x == ARR_WIDTH - 1) ? noMeaningVec : node[memIdx][y][x + 1].position;
				Vec3 nLPos = (x == 0) ? noMeaningVec : node[memIdx][y][x - 1].position;
				Vec3 nTPos = (y == 0) ? noMeaningVec : node[memIdx][y - 1][x].position;
				Vec3 nBPos = (y == ARR_HEIGHT - 1) ? noMeaningVec : node[memIdx][y + 1][x].position;

				float kRight = (x == ARR_WIDTH - 1) ? 0.f : 1.f / ((link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx)*0.5f + EPS);
				float kLeft = (x == 0) ? 0.f : 1.f / ((link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx)*0.5f + EPS);
				float kTop = (y == 0) ? 0.f : 1.f / ((link[y][x][TOP].maxDx - link[y][x][TOP].minDx)*0.5f + EPS);
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : 1.f / ((link[y][x][BOTTOM].maxDx - link[y][x][BOTTOM].minDx)*0.5f + EPS);
				float kSumInv = 1.f / (kRight + kLeft + kTop + kBottom);

				// k 값에 대응해야함
				// 현재는 미리 계산하여 적용
				/*Vec3 plasticity = { fminf(link[y][x][RIGHT].maxVrtDx, (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx) * 0.5f),
					fminf(link[y][x][BOTTOM].maxHrzDy, (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy) * 0.5f), 0.0f };*/
				Vec3 plasticity = { .425f, .425f, 0.0f };
				plasticity *= (1.f - powf(0.01f, 0.16f));

				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // 이 식을 축별로 근사
				float goal_Fx = (nRPos.x - 1.f)*kRight + (nLPos.x + 1.f)*kLeft + nTPos.x*kTop + nBPos.x * kBottom;
				float goal_Fy = nRPos.y*kRight + nLPos.y*kLeft + (nTPos.y + 1.f)*kTop + (nBPos.y - 1.f) * kBottom;
				float goal_Fz = 0.f;

				//// 진동 감쇠
				Vec3 relax = Vec3(goal_Fx, goal_Fy, goal_Fz) * kSumInv;
				Vec3 delta = relax - targetPos;
				Vec3 movF = delta * (kRight + kLeft + kTop + kBottom);
				float movF_size = movF.getLength();
				movF.normalize();

				//if (mov 가 적당하게 커야만 > T)
				//적당하게란 k값에 대응한다. k값이 너무 크면 딱딱한 물체이므로 T는 0에 가까울 것이다.: 세인의 방법
				if (movF_size > maxF * plasticity.x)
					targetPos.x += ((movF.x * (movF_size - maxF * plasticity.x)) * kSumInv);
				if (movF_size > maxF * plasticity.y)
					targetPos.y += ((movF.y * (movF_size - maxF * plasticity.y)) * kSumInv);

				node[(memIdx + 1) & 1][y][x].position.x = targetPos.x;
				node[(memIdx + 1) & 1][y][x].position.y = targetPos.y;
			}

		memIdx = (++memIdx) & 1;
	}
}
