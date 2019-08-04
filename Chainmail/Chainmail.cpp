#include "Chainmail.h"
#include "Vec3.h"
using namespace std;

// 노드(Point) 초기화
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
	for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
		for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
			node[idxY][idxX].time = nodeCopy[idxY][idxX].time = 100000.0f;
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
			for (int x = 0; x < ARR_WIDTH; ++x)
			{
				// 살짝 수정 필요 (pair 사용하지말기-> CUDA로 넘어갈때 바로 변경하기 힘듬)
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
							node[y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].minDx; // 우측노드보다 값이 작은 위치로 보내야함
						else if ((minTimeNeighbor.position.x - node[y][x].position.x) > link[y][x][mtnDir].maxDx)
							node[y][x].position.x = minTimeNeighbor.position.x - link[y][x][mtnDir].maxDx; // 최대 차이만큼 빼서 더한다. 아니면 값이 커지는 오류

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
							node[y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].minDy; // 바텀노드보다 값이 작은 위치로 보내야함
						else if ((minTimeNeighbor.position.y - node[y][x].position.y) > link[y][x][mtnDir].maxDy)
							node[y][x].position.y = minTimeNeighbor.position.y - link[y][x][mtnDir].maxDy; // 최대 차이만큼 빼서 더한다. 아니면 값이 커지는 오류

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

// 평활화(relaxation) 과정
void Chainmail::relax()
{
	relax_spring();
	//relax_sein();
}

// 힘을 이용한 방식. 고전적
void Chainmail::relax_spring()
{
	const float eps = 0.000001f;

	// false면 node를 보고 nodeCopy에 값 저장, cnt증가(true로 만들어 nodeCopy 렌더링)
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

				Vec3 totalF = (nRight - targetPos)*((nRight - targetPos).dst() - 1) * kRight;
				totalF += (nLeft - targetPos)*((nLeft - targetPos).dst() - 1) * kLeft;
				totalF += (nTop - targetPos)*((nTop - targetPos).dst() - 1) * kTop;
				totalF += (nBottom - targetPos)*((nBottom - targetPos).dst() - 1) * kBottom;

				float delta = 0.01f;
				Vec3 mov = totalF * delta;//(/ m * deltat *deltat);
				targetPos += mov;

				nodeCopy[y][x].position.x = targetPos.x;
				nodeCopy[y][x].position.y = targetPos.y;
				//node[y][x].position.z = targetPos.z;
			}
	}
	// true면 nodeCopy를 보고 node에 값 저장, cnt증가(false로 만들어 node 렌더링)
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

				Vec3 totalF = (nRight - targetPos)*((nRight - targetPos).dst() - 1) * kRight;
				totalF += (nLeft - targetPos)*((nLeft - targetPos).dst() - 1) * kLeft;
				totalF += (nTop - targetPos)*((nTop - targetPos).dst() - 1) * kTop;
				totalF += (nBottom - targetPos)*((nBottom - targetPos).dst() - 1) * kBottom;

				float delta = 0.01f;
				Vec3 mov = totalF * delta;//(/ m * deltat *deltat);
				targetPos += mov;

				node[y][x].position.x = targetPos.x;
				node[y][x].position.y = targetPos.y;
				//node[y][x].position.z = targetPos.z;
			}
	}

	pingPongCnt = !pingPongCnt;
}


// 에너지를 이용한 방식=합력이 0인 에너지 최소점을 구하는 방정식을 푼 방법. 근사적
// 축에 따라 각기 다르게
void Chainmail::relax_sein()
{
	const float maxF = 4.f;
	const float eps = 0.000001f;

	// false면 node를 보고 nodeCopy에 값 저장, cnt증가(true로 만들어 nodeCopy 렌더링)
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
				Vec3 plasticity = { 0.005f, 0.005f, 0.f };

				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // 이 식을 축별로 근사
				float goal_Fx = (nRight.x - 1 - targetPos.x)*kRight + (nLeft.x + 1 - targetPos.x)*kLeft
					+ (nTop.x - targetPos.x)*kTop + (nBottom.x - targetPos.x)*kBottom;
				float goal_Fy = (nRight.y - targetPos.y)*kRight + (nLeft.y - targetPos.y)*kLeft
					+ (nTop.y + 1 - targetPos.y)*kTop + (nBottom.y - 1 - targetPos.y)*kBottom;
				float goal_Fz = 0.f;

				Vec3 goal_F = Vec3(goal_Fx, goal_Fy, goal_Fz);
				Vec3 goal_pos = goal_F * kSumInv;

				//// 진동 감쇠
				Vec3 movF = goal_F;
				float movF_size = movF.getLength();
				movF.normalize();
				float movFx = movF.x;
				float movFy = movF.y;

				//if (mov 가 적당하게 커야만 > T)
				//	//, 적당하게란 k값에 대응한다. k값이 너무 크면 딱딱한 물체이므로 T는 0에 가까울 것이다.: 세인의 방법

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
	// true면 nodeCopy를 보고 node에 값 저장, cnt증가(false로 만들어 node 렌더링)
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
				Vec3 plasticity = { 0.005f, 0.005f, 0.f };
				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // 이 식을 축별로 근사
				float goal_Fx = (nRight.x - 1 - targetPos.x)*kRight + (nLeft.x + 1 - targetPos.x)*kLeft
					+ (nTop.x - targetPos.x)*kTop + (nBottom.x - targetPos.x)*kBottom;
				float goal_Fy = (nRight.y - targetPos.y)*kRight + (nLeft.y - targetPos.y)*kLeft
					+ (nTop.y + 1 - targetPos.y)*kTop + (nBottom.y - 1 - targetPos.y)*kBottom;
				float goal_Fz = 0.f;

				Vec3 goal_F = Vec3(goal_Fx, goal_Fy, goal_Fz);
				Vec3 goal_pos = goal_F * kSumInv;

				//// 진동 감쇠
				Vec3 movF = goal_F;
				float movF_size = movF.getLength();
				movF.normalize();
				float movFx = movF.x;
				float movFy = movF.y;

				//if (mov 가 적당하게 커야만 > T)
				//	//, 적당하게란 k값에 대응한다. k값이 너무 크면 딱딱한 물체이므로 T는 0에 가까울 것이다.: 세인의 방법

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

//// 에너지를 이용한 방식=합력이 0인 에너지 최소점을 구하는 방정식을 푼 방법. 근사적
//// 축에 따라 각기 다르게
//void Chainmail::relax_sein()
//{
//	float maxF = 4.f;
//	const float eps = 0.000001f;
//	// false면 node를 보고 nodeCopy에 값 저장, cnt증가(true로 만들어 nodeCopy 렌더링)
//	if (!pingPongCnt)
//	{
//		for (int y = 0; y < ARR_HEIGHT; ++y)
//			for (int x = 0; x < ARR_WIDTH; ++x)
//			{
//				Vec3 targetPos = node[y][x].position;
//				Vec3 noMeaningVec = targetPos;
//				Vec3 nRight = (x == ARR_WIDTH - 1) ? noMeaningVec : node[y][x + 1];
//				Vec3 nLeft = (x == 0) ? noMeaningVec : node[y][x - 1];
//				Vec3 nTop = (y == 0) ? noMeaningVec : node[y - 1][x];
//				Vec3 nBottom = (y == ARR_HEIGHT - 1) ? noMeaningVec : node[y + 1][x];
//
//				float kRight = 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx + eps);
//				float kLeft = 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx + eps);
//				float kTop = 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy + eps);
//				float kBottom = 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy + eps);
//				float kSum = kRight + kLeft + kTop + kBottom;
//				float kSumInv = 1.f / kSum;
//				//Vec3 plasticity = Vec3(fminf((link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx)/2.f, link[y][x][RIGHT].maxVrtDx),
//				//	fminf((link[y][x][TOP].maxDy - link[y][x][TOP].minDy) / 2.f, link[y][x][RIGHT].maxHrzDy), 0.0f);
//				Vec3 plasticity = Vec3(0.425f, 0.425f, 0.0f);
//				plasticity *= (1.f - powf(0.7f, 0.16f));
//
//				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // 이 식을 축별로 근사
//				float goal_Fx = (nRight.x - 1 - targetPos.x)*kRight + (nLeft.x + 1 - targetPos.x)*kLeft
//					+ (nTop.x - targetPos.x)*kTop + (nBottom.x - targetPos.x)*kBottom;
//				float goal_Fy = (nRight.y - targetPos.y)*kRight + (nLeft.y - targetPos.y)*kLeft
//					+ (nTop.y + 1 - targetPos.y)*kTop + (nBottom.y - 1 - targetPos.y)*kBottom;
//				float goal_Fz = 0.f;
//
//				Vec3 goal_F = Vec3(goal_Fx, goal_Fy, goal_Fz);
//				Vec3 goal_pos = goal_F * kSumInv;
//
//				//// 진동 감쇠
//				Vec3 mov_pos = goal_pos - targetPos;
//				//if (mov 가 적당하게 커야만 > T)
//				//	//, 적당하게란 k값에 대응한다. k값이 너무 크면 딱딱한 물체이므로 T는 0에 가까울 것이다.: 세인의 방법
//				if (mov_pos.x < -plasticity.x) {// elasticity) {
//					//	me = goal_pos;
//					targetPos.x -= plasticity.x;
//				}
//				else if (mov_pos.x > plasticity.x) {// elasticity) {
//				   //	me = goal_pos;
//					targetPos.x += plasticity.x;
//				}
//
//				if (mov_pos.y < -plasticity.y) {// elasticity) {
//					//	me = goal_pos;
//					targetPos.y -= plasticity.y;
//				}
//				else if (mov_pos.y > plasticity.y) {// elasticity) {
//				   //	me = goal_pos;
//					targetPos.y += plasticity.y;
//				}
//
//				//if (mov_pos.z < -plasticity.z) {// elasticity) {
//				//	//	me = goal_pos;
//				//	targetPos.z -= plasticity.z;
//				//}
//				//else if (mov_pos.z > plasticity.z) {// elasticity) {
//				//   //	me = goal_pos;
//				//	targetPos.z += plasticity.z;
//				//}
//
//				nodeCopy[y][x].position.x = targetPos.x;
//				nodeCopy[y][x].position.y = targetPos.y;
//			}
//	}
//	// true면 nodeCopy를 보고 node에 값 저장, cnt증가(false로 만들어 node 렌더링)
//	else
//	{
//		for (int y = 0; y < ARR_HEIGHT; ++y)
//			for (int x = 0; x < ARR_WIDTH; ++x)
//			{
//				Vec3 targetPos = nodeCopy[y][x].position;
//				Vec3 noMeaningVec = targetPos;
//				Vec3 nRight = (x == ARR_WIDTH - 1) ? noMeaningVec : nodeCopy[y][x + 1];
//				Vec3 nLeft = (x == 0) ? noMeaningVec : nodeCopy[y][x - 1];
//				Vec3 nTop = (y == 0) ? noMeaningVec : nodeCopy[y - 1][x];
//				Vec3 nBottom = (y == ARR_HEIGHT - 1) ? noMeaningVec : nodeCopy[y + 1][x];
//
//				float kRight = 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx);
//				float kLeft = 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx);
//				float kTop = 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy);
//				float kBottom = 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy);
//				float kSum = kRight + kLeft + kTop + kBottom;
//				float kSumInv = 1.f / (kRight + kLeft + kTop + kBottom);
//				Vec3 plasticity = Vec3(0.425f, 0.425f, 0.0f);
//				plasticity *= (1.f - powf(0.7f, 0.16f));
//
//				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // 이 식을 축별로 근사
//				float goal_Fx = (nRight.x - 1 - targetPos.x)*kRight + (nLeft.x + 1 - targetPos.x)*kLeft
//					+ (nTop.x - targetPos.x)*kTop + (nBottom.x - targetPos.x)*kBottom;
//				float goal_Fy = (nRight.y - targetPos.y)*kRight + (nLeft.y - targetPos.y)*kLeft
//					+ (nTop.y + 1 - targetPos.y)*kTop + (nBottom.y - 1 - targetPos.y)*kBottom;
//				float goal_Fz = 0.f;
//
//				Vec3 goal_F = Vec3(goal_Fx, goal_Fy, goal_Fz);
//				Vec3 goal_pos = goal_F * kSumInv;
//
//				//// 진동 감쇠
//				Vec3 mov_pos = goal_pos - targetPos;
//				//if (mov 가 적당하게 커야만 > T)
//				//	//, 적당하게란 k값에 대응한다. k값이 너무 크면 딱딱한 물체이므로 T는 0에 가까울 것이다.: 세인의 방법
//				if (mov_pos.x < -plasticity.x) {// elasticity) {
//					//	me = goal_pos;
//					targetPos.x -= plasticity.x;
//				}
//				else if (mov_pos.x > plasticity.x) {// elasticity) {
//				   //	me = goal_pos;
//					targetPos.x += plasticity.x;
//				}
//
//				if (mov_pos.y < -plasticity.y) {// elasticity) {
//					//	me = goal_pos;
//					targetPos.y -= plasticity.y;
//				}
//				else if (mov_pos.y > plasticity.y) {// elasticity) {
//				   //	me = goal_pos;
//					targetPos.y += plasticity.y;
//				}
//
//				//if (mov_pos.z < -plasticity.z) {// elasticity) {
//				//	//	me = goal_pos;
//				//	targetPos.z -= plasticity.z;
//				//}
//				//else if (mov_pos.z > plasticity.z) {// elasticity) {
//				//   //	me = goal_pos;
//				//	targetPos.z += plasticity.z;
//				//}
//
//				node[y][x].position.x = targetPos.x;
//				node[y][x].position.y = targetPos.y;
//			}
//	}
//
//	pingPongCnt = !pingPongCnt;
//}
