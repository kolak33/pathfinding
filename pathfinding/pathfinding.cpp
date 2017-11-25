#include "stdafx.h"
#include <gl\freeglut.h>
#include <vector>
#include "CExperimentsHandler.h"



CExperimentsHandler *expHandlerGlobalPtr;

void DrawMap()
{
	// musi byc ustawione wszystko odpowiednio wczesniej

	//expHandlerGlobalPtr->GetHPAStar().DrawMap();
	expHandlerGlobalPtr->GetJPSearch().DrawMap();
	//expHandlerGlobalPtr->GetAStar().DrawMap();
}

int main(int argc, char** argv)
{
	CExperimentsHandler expHandler;
	expHandlerGlobalPtr = &expHandler;

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE);
    glutInitDisplayMode(GLUT_RGB);

    glutInitWindowSize(650, 650);
    glutInitWindowPosition(300, 100);
    glutCreateWindow("HelloTeapot");

    glutDisplayFunc(DrawMap);

    glutMainLoop();

	return 0;
}
