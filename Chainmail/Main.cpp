#include <iostream>
#include <gl/freeglut.h>
#include "Chainmail.h"
using namespace std;

// 노드 개수 조절, 창 크기 조절은
// DefConstants.h에서 변경할 수 있다.

// 변화할 위치 변수 설정 가능 (빨간색 고리)
#define TARGET_X 8
#define TARGET_Y 8


// gl function들은 세인이형 초기 코드 참고하였음
#define ORTHO_BOUND (ARR_WIDTH * .8)

int dragStartX;
int dragStartY;
bool dragging = false;

// 노드 정보 출력(좌표, timestamp)
void printNode();


// 체인메일 클래스 변수
Chainmail c;


// display function
void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	for (int i = 0; i < ARR_HEIGHT; i++)
	{
		for (int j = 0; j < ARR_WIDTH; j++)
		{
			float xPos = 0.f;
			float yPos = 0.f;
			// true면 nodeCopy를 렌더링
			if (c.pingPongCnt) {
				xPos = c.nodeCopy[i][j].position.x;
				yPos = c.nodeCopy[i][j].position.y;
			}
			// false면 node를 렌더링
			else {
				xPos = c.node[i][j].position.x;
				yPos = c.node[i][j].position.y;
			}

			if ((j == TARGET_X) && (i == TARGET_Y))
				glColor3f(1, 0, 0);
			else
				glColor3f(1, 1, 1);

			glPushMatrix();
			glTranslatef(xPos - (ARR_WIDTH / 2), (ARR_HEIGHT / 2) - yPos, 0);
			glutSolidTorus(.1, .8, 4, 30);
			glPopMatrix();
		}
	}

	glutSwapBuffers();
}

void reshapeFunc(int width, int height)
{
	glViewport(0, 0, width, height);
	GLfloat WidthFactor = (GLfloat)width / (GLfloat)WINDOW_W;
	GLfloat HeightFactor = (GLfloat)height / (GLfloat)WINDOW_H;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(
		-(ORTHO_BOUND * WidthFactor), (ORTHO_BOUND * WidthFactor),
		-(ORTHO_BOUND * HeightFactor), (ORTHO_BOUND * HeightFactor),
		-1.0, 1.0);
}

void clickFunc(GLint button, GLint state, GLint x, GLint y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			dragging = true;
			dragStartX = x;
			dragStartY = y;
		}
		else if (state == GLUT_UP)
			dragging = false;
	}
}

void motionFunc(GLint x, GLint y)
{
	if (dragging)
	{
		int deltaX = (x - dragStartX);
		int deltaY = (y - dragStartY);

		c.movePosition(TARGET_X, TARGET_Y, deltaX * .0426f, deltaY * .0426f);

		dragStartX = x;
		dragStartY = y;

		//printNode();

		glutPostRedisplay();
	}
}

void idleFunc() {
	c.relax();
	displayFunc();
}

// init Chainmail
void InitChainmail()
{
	c.setNode();
	c.setLink();
}

int main(int argc, char* argv[])
{
	// init gl
	glutInit(&argc, argv);
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Chainmail");

	// init Chainmail
	InitChainmail();

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutMouseFunc(clickFunc);
	glutMotionFunc(motionFunc);
	glutIdleFunc(idleFunc);
	glutMainLoop();
}


void printNode()
{
	// 좌표와 시간 출력
	for (int i = 0; i < ARR_HEIGHT; ++i)
	{
		for (int j = 0; j < ARR_WIDTH; ++j)
		{
			printf("node[%d][%d] : %f %f %f\n",
				i, j,
				c.node[i][j].position.x,
				c.node[i][j].position.y,
				c.node[i][j].time);
		}
		printf("\n");
	}
}
