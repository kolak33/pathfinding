#include "stdafx.h"
#include <gl\freeglut.h>
#include <vector>
#include "CDijkstra.h"


const int SCREENSIZE = 600;
const int GRIDSIZE	 = 30;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	//double offset[3];
	//offset[0] = 0.1; offset[1] = 0.3; offset[2] = 0.5;
	//glutSolidSierpinskiSponge(5, offset, 0.5);
	//glutSolidTeapot(.6);

	int quadCount = 4;
	float quadSize = 2.0f / static_cast<float>(3 * quadCount);

	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.2f, 0.8f);

	for (int x = 0; x < quadCount; ++x)
	{
		float xPos = -0.5f + x * 2 * quadSize;

		for (int y = 0; y < quadCount; ++y)
		{
			float yPos = -0.5f + y * 2 * quadSize;

			glVertex2f(xPos, yPos);
			glVertex2f(xPos + quadSize, yPos);
			glVertex2f(xPos + quadSize, yPos + quadSize);
			glVertex2f(xPos, yPos + quadSize);			
		}
	}
	
	glEnd();
	glFlush();

	/*
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.2f, 0.8f);

	glVertex2f(-0.2, -0.2);
	glVertex2f(0.2 , -0.2);
	glVertex2f(0.2, 0.2);
	glVertex2f(-0.2, 0.2);

	glEnd();
	*/
	//glFlush();
}

//CMapManager *mapManGlobalPtr;
CDijkstra *dijkstraGlobalPtr;

void DrawMap()
{
	//mapManGlobalPtr->DrawMap();
	dijkstraGlobalPtr->DrawMap();
}

int main(int argc, char** argv)
{
	//CMapManager mapMan;
	//mapMan.InitMapFromFile();
	//mapManGlobalPtr = &mapMan;
	CDijkstra dijkstra;
	dijkstraGlobalPtr = &dijkstra;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE);
	glutInitDisplayMode(GLUT_RGB);

	glutInitWindowSize(650, 650);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("HelloTeapot");

	glutDisplayFunc(DrawMap);

	glutMainLoop();

	


	return 0;
}