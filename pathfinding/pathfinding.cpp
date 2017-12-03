#include "stdafx.h"
#include <gl\freeglut.h>
#include <vector>
#include "CAutomatedExperiments.h"



//CExperimentsHandler *expHandlerGlobalPtr;
CAutomatedExperiments *expAutomatedGlobalPtr;

void DrawMap()
{
	// musi byc ustawione wszystko odpowiednio wczesniej

	//expHandlerGlobalPtr->GetHPAStar().DrawMap();
	//expHandlerGlobalPtr->GetJPSearch().DrawMap();

	//expHandlerGlobalPtr->GetMapManager().DrawMap();
	//expHandlerGlobalPtr->GetAStar().DrawMap();
	//expHandlerGlobalPtr->GetFringe().DrawMap();
	
}

int main(int argc, char** argv)
{
	//CExperimentsHandler expHandler;
	//expHandlerGlobalPtr = &expHandler;
	CAutomatedExperiments expAutomated;
	expAutomatedGlobalPtr = &expAutomated;

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_SINGLE);
    glutInitDisplayMode(GLUT_RGB);

    glutInitWindowSize(850, 850);
    glutInitWindowPosition(300, 10);
    glutCreateWindow("HelloTeapot");

    glutDisplayFunc(DrawMap);

    glutMainLoop();

	return 0;
}
