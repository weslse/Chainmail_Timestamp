#include "Chainmail.h"
#include "Vec3.h"
using namespace std;

// 노드(Point) 초기화
void Chainmail::setNode()
{
	for (int idxM = 0; idxM < NUM_MEMSET; ++idxM) {
		for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY) {
			for (int idxX = 0; idxX < ARR_WIDTH; ++idxX) {

				node[idxM][idxY][idxX].position.x = static_cast<float>(idxX);
				node[idxM][idxY][idxX].position.y = static_cast<float>(idxY);
				node[idxM][idxY][idxX].time = 100000.0f;
			}
		}
	}
}

// 동형질 버전 저장용 함수
void Chainmail::setLink_Homogeneous()
{
	for (int y = 0; y < ARR_HEIGHT; ++y) {
		for (int x = 0; x < ARR_WIDTH; ++x) {

			// 순서 r, l, t, b
			for (int n = 0; n < NUM_NEIGHBOR; ++n) {

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

void Chainmail::setConstranints(const int x, const int y, const Direction n, const ubyte nDensity)
{
	// 공기
	if (nDensity >= 0 && nDensity < 40) {
		link[y][x][n].minDx = 0.5f;
		link[y][x][n].maxDx = 1.5f;
		link[y][x][n].maxHrzDy = 0.5f;

		link[y][x][n].minDy = 0.5f;
		link[y][x][n].maxDy = 1.5f;
		link[y][x][n].maxVrtDx = 0.5f;
	}
	// 피부
	else if (nDensity >= 40 && nDensity < 100) {
		link[y][x][n].minDx = 0.95f;
		link[y][x][n].maxDx = 1.05f;
		link[y][x][n].maxHrzDy = 0.05f;

		link[y][x][n].minDy = 0.95f;
		link[y][x][n].maxDy = 1.05f;
		link[y][x][n].maxVrtDx = 0.05f;
	}
	// 뼈
	else if (nDensity >= 100 && nDensity < 140) {
		link[y][x][n].minDx = 0.9999f;
		link[y][x][n].maxDx = 1.0001f;
		link[y][x][n].maxHrzDy = 0.0001f;

		link[y][x][n].minDy = 0.9999f;
		link[y][x][n].maxDy = 1.0001f;
		link[y][x][n].maxVrtDx = 0.0001f;
	}
	// 이외의 더 밀도 높은 경우
	else {
		link[y][x][n].minDx = 0.99999f;
		link[y][x][n].maxDx = 1.00001f;
		link[y][x][n].maxHrzDy = 0.00001f;

		link[y][x][n].minDy = 0.99999f;
		link[y][x][n].maxDy = 1.00001f;
		link[y][x][n].maxVrtDx = 0.00001f;
	}
}

// 링크(constraint) 초기화
// 이형질(Heterogeneous)한 경우
void Chainmail::setLink()
{
	for (int y = 0; y < ARR_HEIGHT; ++y) {
		for (int x = 0; x < ARR_WIDTH; ++x) {

			// 순서 r, l, t, b
			for (int n = 0; n < NUM_NEIGHBOR; ++n) {

				// 해당 이웃의 밀도
				ubyte nDensity = 0;

				// Density 값에 따라 형질 최대, 최소 값에 변화를 준다.
				// 형질이 다른 경우(heterogeneous)
				// link의 constraint는 모두 임의의 상수로 설정하였다.
				switch (n) {

				case Direction::RIGHT:
					nDensity = (x == ARR_WIDTH - 1) ? NULL : node[memIdx][y][x + 1].density;
					setConstranints(x, y, Direction::RIGHT, nDensity);
					break;

				case Direction::LEFT:
					nDensity = (x == 0) ? NULL : node[memIdx][y][x - 1].density;
					setConstranints(x, y, Direction::LEFT, nDensity);
					break;

				case Direction::TOP:
					nDensity = (y == 0) ? NULL : node[memIdx][y - 1][x].density;
					setConstranints(x, y, Direction::TOP, nDensity);
					break;

				case Direction::BOTTOM:
					nDensity = (y == ARR_HEIGHT - 1) ? NULL : node[memIdx][y + 1][x].density;
					setConstranints(x, y, Direction::BOTTOM, nDensity);

					break;
				}
			}
		}
	}
}

void Chainmail::setDensity(const ubyte volumeSlice[ARR_HEIGHT][ARR_WIDTH])
{
	for (int idxM = 0; idxM < NUM_MEMSET; ++idxM)
		for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
			for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
				node[idxM][idxY][idxX].density = volumeSlice[idxY][idxX];
}

void Chainmail::setTexArr()
{
	for (int y = 0; y < ARR_HEIGHT; ++y) {
		for (int x = 0; x < ARR_WIDTH; ++x) {
			for (int c = 0; c < 3; ++c) {

				int idxX = static_cast<int>(node[memIdx][y][x].position.x);
				int idxY = static_cast<int>(node[memIdx][y][x].position.y);

				if (idxY > 0 && idxY < ARR_HEIGHT)
					if (idxX > 0 && idxX < ARR_WIDTH)
						texArr[idxY][idxX][c] = node[memIdx][y][x].density;

			}
		}
	}
}

void Chainmail::resetTime()
{
	for (int idxM = 0; idxM < NUM_MEMSET; ++idxM)
		for (int idxY = 0; idxY < ARR_HEIGHT; ++idxY)
			for (int idxX = 0; idxX < ARR_WIDTH; ++idxX)
				node[idxM][idxY][idxX].time = 100000.0f;
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

	// 위치가 갱신되었으므로
	// 텍스쳐 배열 또한 갱신한다.
	setTexArr();
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
			for (int x = 0; x < ARR_WIDTH; ++x) {

				// 살짝 수정 필요 (pair 사용하지말기-> CUDA로 넘어갈때 바로 변경하기 힘듬)
				auto pair_NeighborDir = minTimeNeighborDir(x, y);
				Node minTimeNeighbor = pair_NeighborDir.first; // minTimeNeighbor
				Direction mtnDir = pair_NeighborDir.second;    // neighborDir

				// 현재 노드의 시간이 주변 노드의 시간 보다 느릴때 (나.t > 주변.t + 1)
				// 노드의 위치를 갱신한다.
				if (node[memIdx][y][x].time > minTimeNeighbor.time + 1) {

					// 순서 : r l t b
					switch (mtnDir) {

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
// 원하는 함수의 주석을 해제하고
// 다른 함수의 주석을 넣어 실행할 것
void Chainmail::relax()
{
	//relax_spring();
	relax_sein();
}

// 힘을 이용한 방식. 고전적
void Chainmail::relax_spring()
{
	// 임의의 반복 횟수 제한
	// 세인이형 모델에 비해 횟수가 많다.
	// 약 10회 부터 약간의 버벅임이 있음 (본인 노트북 기준)
	for (int i = 0; i < 8; i++) {
		for (int y = 0; y < ARR_HEIGHT; ++y) {
			for (int x = 0; x < ARR_WIDTH; ++x) {

				Node target = node[memIdx][y][x];

				// EPICENTER는 relaxation에서 제외
				if (y == TARGET_Y && x == TARGET_X) {

					node[(memIdx + 1) & 1][y][x].position.x = target.position.x;
					node[(memIdx + 1) & 1][y][x].position.y = target.position.y;
					continue;
				}

				// 각각의 노드 변수
				Node noMeaningNode = target;
				Node nRight = (x == ARR_WIDTH - 1) ? noMeaningNode : node[memIdx][y][x + 1];
				Node nLeft = (x == 0) ? noMeaningNode : node[memIdx][y][x - 1];
				Node nTop = (y == 0) ? noMeaningNode : node[memIdx][y - 1][x];
				Node nBottom = (y == ARR_HEIGHT - 1) ? noMeaningNode : node[memIdx][y + 1][x];

				// 노드들의 위치 벡터
				Vec3 targetPos = target.position;
				Vec3 nRPos = nRight.position;
				Vec3 nLPos = nLeft.position;
				Vec3 nTPos = nTop.position;
				Vec3 nBPos = nBottom.position;

				// 이웃노드들의 k값(탄성계수)
				float kRight = (x == ARR_WIDTH - 1) ? 0.f : 1.f / (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx);
				float kLeft = (x == 0) ? 0.f : 1.f / (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx);
				float kTop = (y == 0) ? 0.f : 1.f / (link[y][x][TOP].maxDy - link[y][x][TOP].minDy);
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : 1.f / (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy);

				// k값에 비례
				// 이웃노드의 k값을 댐핑에 사용할 수 있도록 
				// 아래의 kProp 상수를 곱하여 사용한다.(k값 조절)
				constexpr float kProp = 0.05f;
				const float kDampRight = kProp * kRight;
				const float kDampLeft = kProp * kLeft;
				const float kDampTop = kProp * kTop;
				const float kDampBottom = kProp * kBottom;

				// 오른쪽 방향의 탄성력 + 감쇠력
				Vec3 vRight = nRPos - targetPos;
				const float vRightLen = vRight.getLength();
				Vec3 velRight = ((nRight.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vRight * (1.f / (nRight.time - target.time));
				vRight.normalize();
				Vec3 fRight = vRight * ((ORIGIN_LEN - vRightLen) * kRight - (velRight * vRight) * kDampRight);  // 탄성력 + 감쇠력

				// 왼쪽 방향의 탄성력 + 감쇠력
				Vec3 vLeft = nLPos - targetPos;
				const float vLeftLen = vLeft.getLength();
				Vec3 velLeft = ((nLeft.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vLeft * (1.f / (nLeft.time - target.time));
				vLeft.normalize();
				Vec3 fLeft = vLeft * ((ORIGIN_LEN - vLeftLen) * kLeft - (velLeft * vLeft) * kDampLeft);  // 탄성력 + 감쇠력

				// 위쪽 방향의 탄성력 + 감쇠력
				Vec3 vTop = nTPos - targetPos;
				const float vTopLen = vTop.getLength();
				Vec3 velTop = ((nTop.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vTop * (1.f / (nTop.time - target.time));
				vTop.normalize();
				Vec3 fTop = vTop * ((ORIGIN_LEN - vTopLen) * kTop - (velTop * vTop) * kDampTop);  // 탄성력 + 감쇠력

				// 아래쪽 방향의 탄성력 + 감쇠력
				Vec3 vBottom = nBPos - targetPos;
				const float vBottomLen = vBottom.getLength();
				Vec3 velBottom = ((nBottom.time - target.time) == 0.f) ? Vec3(0.f, 0.f, 0.f) : vBottom * (1.f / (nBottom.time - target.time));
				vBottom.normalize();
				Vec3 fBottom = vBottom * ((ORIGIN_LEN - vBottomLen) * kBottom - (velBottom * vBottom) * kDampBottom);  // 탄성력 + 감쇠력

				// 총합 탄성력은 계산 결과에 반대로 작용
				Vec3 fTotal = -(fRight + fLeft + fTop + fBottom);

				// 크기가 너무 작지 않다면 움직임에 적용한다.
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


// 에너지를 이용한 방식=합력이 0인 에너지 최소점을 구하는 방정식을 푼 방법. 근사적
// 축에 따라 각기 다르게
void Chainmail::relax_sein()
{
	// 이 값을 조절하여 부드러운 정도 설정
	// 동형질의 물질이기 때문에 이렇게 설정가능
	// 이형질인 경우 노드의 property 등으로 설정해야할 것
	// constexpr float ELASTICITY = 0.7f;

	// spring 방법에 비해 반복횟수가 적다.
	for (int i = 0; i < 3; ++i) {
		for (int y = 0; y < ARR_HEIGHT; ++y) {
			for (int x = 0; x < ARR_WIDTH; ++x) {

				Vec3 targetPos = node[memIdx][y][x].position;

				// EPICENTER는 relaxation에서 제외
				if (y == TARGET_Y && x == TARGET_X) {

					node[(memIdx + 1) & 1][y][x].position.x = targetPos.x;
					node[(memIdx + 1) & 1][y][x].position.y = targetPos.y;
					continue;
				}

				// 이웃노드의 방향
				Vec3 noMeaningVec = targetPos;
				Vec3 nRPos = (x == ARR_WIDTH - 1) ? noMeaningVec : node[memIdx][y][x + 1].position;
				Vec3 nLPos = (x == 0) ? noMeaningVec : node[memIdx][y][x - 1].position;
				Vec3 nTPos = (y == 0) ? noMeaningVec : node[memIdx][y - 1][x].position;
				Vec3 nBPos = (y == ARR_HEIGHT - 1) ? noMeaningVec : node[memIdx][y + 1][x].position;

				// 이웃노드에 대한 k값
				float kRight = (x == ARR_WIDTH - 1) ? 0.f : (1.f / ((link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx) * 0.5f + EPS));
				float kLeft = (x == 0) ? 0.f : (1.f / ((link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx) * 0.5f + EPS));
				float kTop = (y == 0) ? 0.f : (1.f / ((link[y][x][TOP].maxDy - link[y][x][TOP].minDy) * 0.5f + EPS));
				float kBottom = (y == ARR_HEIGHT - 1) ? 0.f : (1.f / ((link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy) * 0.5f + EPS));
				float kSumInv = 1.f / (kRight + kLeft + kTop + kBottom);

				// k 값에 대응해야함 현재는 미리 계산하여 적용
				// 원진 누나 코드 참고
				Vec3 plasticity = {
					fminf(link[y][x][RIGHT].maxVrtDx, (link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx) * 0.5f), // plasticity.x
					fminf(link[y][x][BOTTOM].maxHrzDy, (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy) * 0.5f), // plasticity.y
					0.0f // plasticity.z
				};

				// 이형질(heterogeneous)한 경우이므로 elasticity를 정해주어야함
				// 각 방향의 (max-min)값중에 가장 작은 값(가장 딱딱한 물체)의 
				// 부드러운 정도를 따르기로 정함
				float elasticity = fminf(
					fminf((link[y][x][RIGHT].maxDx - link[y][x][RIGHT].minDx), (link[y][x][LEFT].maxDx - link[y][x][LEFT].minDx)),
					fminf((link[y][x][TOP].maxDy - link[y][x][TOP].minDy), (link[y][x][BOTTOM].maxDy - link[y][x][BOTTOM].minDy))
				);

				// link.constraint와 k값은 반비례
				// elasticity와 link.constraint는 비례
				// plasticity는 k값과 반비례해야하므로 (1-elasticity)를 이용
				// 임의의 숫자를 사용하여 적절하게 움직일 수 있도록 한다.(ELASTICITY_EXP = 0.16f)
				// plasticity = plasticity * (1 - (elasticity^0.16f))
				plasticity *= (1.f - powf(elasticity, ELASTICITY_EXP));

				//plasticity *= (1.f - elasticity * 0.0125f);

				//Vec3 goal_pos = (right - 1 - me)*right.k + (left + 1 - me)*left.k + (top - me)*top.k ...; // 이 식을 축별로 근사
				float goal_Fx = ((nRPos.x - 1.f) * kRight) + ((nLPos.x + 1.f) * kLeft) + (nTPos.x * kTop) + (nBPos.x * kBottom);
				float goal_Fy = (nRPos.y * kRight) + (nLPos.y * kLeft) + ((nTPos.y + 1.f) * kTop) + ((nBPos.y - 1.f) * kBottom);
				float goal_Fz = 0.f;

				//// 진동 감쇠
				Vec3 relax = Vec3(goal_Fx, goal_Fy, goal_Fz) * kSumInv;
				Vec3 delta = relax - targetPos;
				Vec3 movF = delta * (kRight + kLeft + kTop + kBottom);
				float movF_size = movF.getLength();
				movF.normalize();
				
				Vec3 originPos = targetPos;

				// if (mov 가 적당하게 커야만 > T)
				// 적당하게란 k값에 대응한다. k값이 너무 크면 딱딱한 물체이므로 T는 0에 가까울 것이다.: 세인의 방법
				if (movF_size > MAX_F * plasticity.x)
					targetPos.x += ((movF.x * (movF_size - MAX_F * plasticity.x)) * kSumInv);
				if (movF_size > MAX_F * plasticity.y)
					targetPos.y += ((movF.y * (movF_size - MAX_F * plasticity.y)) * kSumInv);

				// 수정 필요
				if ((nRPos.x - targetPos.x) < link[y][x][RIGHT].minDx)
					targetPos.x += link[y][x][RIGHT].minDx;
				if ((nRPos.x - targetPos.x) > link[y][x][RIGHT].maxDx)
					targetPos.x -= link[y][x][RIGHT].maxDx;

				if ((targetPos.x - nLPos.x) < link[y][x][LEFT].minDx)
					targetPos.x += link[y][x][LEFT].minDx;
				if ((targetPos.x - nLPos.x) > link[y][x][LEFT].maxDx)
					targetPos.x -= link[y][x][LEFT].maxDx;

				if ((targetPos.y - nTPos.y) < link[y][x][TOP].minDy)
					targetPos.y += link[y][x][TOP].minDy;

				if ((targetPos.y - nTPos.y) < link[y][x][TOP].maxDy)
					targetPos.x -= link[y][x][LEFT].maxDx;

				if ((nBPos.y - targetPos.y) > link[y][x][BOTTOM].minDy)
					targetPos.y += link[y][x][BOTTOM].minDy;

				if ((nBPos.y - targetPos.y) < link[y][x][BOTTOM].maxDy)
					targetPos.x -= link[y][x][BOTTOM].maxDy;

				// pingpong scheme
				node[(memIdx + 1) & 1][y][x].position.x = targetPos.x;
				node[(memIdx + 1) & 1][y][x].position.y = targetPos.y;

			} // end for(x)
		} // end for(y)

		// 0->1, 1->0
		memIdx = (++memIdx) & 1;
	} // end for(i)
}
