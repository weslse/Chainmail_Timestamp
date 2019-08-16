#include <iostream>
#include <fstream>
#include <gl/freeglut.h>
#include "Chainmail.h"
using namespace std;

// 노드 개수 조절, 창 크기 조절, 조절 위치 변수는
// DefConstants.h에서 변경할 수 있다.

// gl function들은 세인이형 초기 코드 참고하였음
#define ORTHO_BOUND (ARR_WIDTH * .8)

int dragStartX;
int dragStartY;
bool dragging = false;


// volume data
ubyte** volume = nullptr;

// 체인메일 클래스 변수
Chainmail c;



// function prototypes
void displayFunc();
void reshapeFunc(int width, int height);
void clickFunc(GLint button, GLint state, GLint x, GLint y);
void motionFunc(GLint x, GLint y);
void idleFunc();

// 노드 정보 출력(좌표, timestamp)
void printNode();



// 주요 함수
void setVolume()
{
	volume = new ubyte*[ARR_HEIGHT];
	for (int y = 0; y < ARR_HEIGHT; ++y)
		volume[y] = new ubyte[ARR_WIDTH];

	ubyte* tVolume = new ubyte[VOLUME_SIZE];

	std::ifstream inStream;
	inStream.open("./volume/Bighead.den", std::ios::binary);

	if (!inStream)
		return;

	inStream.seekg(VOLUME_HEIGHT * VOLUME_WIDTH * 80, ios::beg);
	inStream.read(reinterpret_cast<char*>(tVolume), VOLUME_SIZE);

	for (int y = 0; y < ARR_HEIGHT; ++y)
		for (int x = 0; x < ARR_WIDTH; ++x)
			volume[y][x] = tVolume[y * 4 * VOLUME_HEIGHT + x * 4];

	delete tVolume;
}

// init Chainmail
void initChainmail()
{
	c.setNode();
	c.setLink();
	c.setDensity(volume);
	c.setTexArr();
}



int main(int argc, char* argv[])
{
	// init gl
	glutInit(&argc, argv);
	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Chainmail");

	// init Chainmail
	setVolume();
	initChainmail();

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutMouseFunc(clickFunc);
	glutMotionFunc(motionFunc);
	//glutIdleFunc(idleFunc);
	glutMainLoop();
}



// GL 함수 구현부
void displayFunc()
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB,
		GL_UNSIGNED_BYTE, &c.texArr);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f); glVertex3f(-64.f, 64.f, 0.f);
	glTexCoord2f(1.f, 0.f); glVertex3f(64.f, 64.f, 0.f);
	glTexCoord2f(1.f, 1.f); glVertex3f(64.f, -64.f, 0.f);
	glTexCoord2f(0.f, 1.f); glVertex3f(-64.f, -64.f, 0.f);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	int mIdx = c.memIdx;
	float xPos = c.node[mIdx][TARGET_Y][TARGET_X].position.x;
	float yPos = c.node[mIdx][TARGET_Y][TARGET_X].position.y;
	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(xPos - (ARR_WIDTH / 2), (ARR_HEIGHT / 2) - yPos, 0);
	glutSolidTorus(.1, .8, 4, 30);
	glPopMatrix();

	glutSwapBuffers();

	/*
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	int mIdx = c.memIdx;
	for (int i = 0; i < ARR_HEIGHT; i++)
		for (int j = 0; j < ARR_WIDTH; j++) {

			float xPos = c.node[mIdx][i][j].position.x;
			float yPos = c.node[mIdx][i][j].position.y;

			if ((j == TARGET_X) && (i == TARGET_Y))
				glColor3f(1, 0, 0);
			else
				glColor3f(1, 1, 1);

			glPushMatrix();
			glTranslatef(xPos - (ARR_WIDTH / 2), (ARR_HEIGHT / 2) - yPos, 0);
			glutSolidTorus(.1, .8, 4, 30);
			glPopMatrix();
		}

	glutSwapBuffers();
	*/
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
	if (button == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

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
	if (dragging) {

		int deltaX = (x - dragStartX);
		int deltaY = (y - dragStartY);

		c.movePosition(TARGET_X, TARGET_Y, deltaX * .0426f, deltaY * .0426f);

		dragStartX = x;
		dragStartY = y;

		glutPostRedisplay();
	}
}

void idleFunc()
{
	c.relax();
	c.setTexArr();
	displayFunc();
}

void printNode()
{
	int mIdx = c.memIdx;
	// 좌표와 시간 출력
	for (int i = 0; i < ARR_HEIGHT; ++i) {

		for (int j = 0; j < ARR_WIDTH; ++j) {

			printf("node[%d][%d] : %f %f %f\n",
				i, j,
				c.node[mIdx][i][j].position.x,
				c.node[mIdx][i][j].position.y,
				c.node[mIdx][i][j].time);
		}
		printf("\n");
	}
}
