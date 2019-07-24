#include <iostream>
#include <gl/freeglut.h>
#include "Chainmail.h"
using namespace std;

#define ORTHO_BOUND (ARR_WIDTH * .8)

#define TARGET_X 10
#define TARGET_Y 10

int dragStartX;
int dragStartY;
bool dragging = false;

Chainmail c;


void printNode();

// display function
void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	for (int i = 0; i < ARR_HEIGHT; i++) 
	{
		for (int j = 0; j < ARR_WIDTH; j++)
		{
			float xPos = c.node[i][j].position.x;
			float yPos = c.node[i][j].position.y;

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
