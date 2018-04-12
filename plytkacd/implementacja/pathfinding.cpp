#include "stdafx.h"
#include <gl\freeglut.h>
#include <vector>
#include "CAutomatedExperiments.h"
#include <fstream>

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
	expAutomatedGlobalPtr->DrawMap();
}

int main(int argc, char** argv)
{
	CAutomatedExperiments expAutomated;
	expAutomatedGlobalPtr = &expAutomated;

	//bool bCalcOdchylenie = false;

	//if (bCalcOdchylenie)
	//{
	//	std::ifstream input("..\\wyniki\\ResultsOdchylenie.txt");
	//	std::ofstream output("..\\wyniki\\OdchStCompRes.txt", std::ofstream::app);
	//	double optimal;
	//	double current;
	//	std::string algName;
	//	std::string statName;
	//	double value;
	//	std::map<std::string, double> stats;
	//	std::map<std::string, std::vector<double>> avgTimes;
	//	std::map<std::string, double> srednia;
	//	int mapCount = 0;
	//	if (input.is_open())
	//	{
	//		while (!input.eof())
	//		{
	//			input >> algName;
	//			input >> statName;
	//			input >> value;
	//			++mapCount;

	//			std::string fullName = algName + statName;
	//			stats[fullName] += value;

	//			if (statName.find(std::string("searchTime")) != std::string::npos)
	//				avgTimes[fullName].push_back(value);

	//			//if (avgTimes[fullName].size() == 61)
	//			//	stats[fullName] -= value;
	//		}

	//		for (auto& a : avgTimes)
	//		{
	//			if (a.second.size() == 41)
	//				a.second.pop_back();
	//		}
	//		if (avgTimes["FringeSearchsearchTime"].size() == 41)
	//			avgTimes["FringeSearchsearchTime"].pop_back();

	//		for (auto alg : avgTimes)
	//		{
	//			double avg = 0.0;
	//			for (auto val : alg.second)
	//			{
	//				avg += val;
	//			}
	//			avg /= alg.second.size();

	//			double stdOdchylenie = 0.0;
	//			for (auto val : alg.second)
	//			{
	//				stdOdchylenie += (val - avg) * (val - avg);
	//			}
	//			stdOdchylenie /= alg.second.size() - 1;
	//			stdOdchylenie = std::sqrt(stdOdchylenie);

	//			output << alg.first << " ST ODCH. " << stdOdchylenie << "\n";
	//		}

	//		for (auto st : stats)
	//		{
	//			output << st.first << " " << st.second / 40 << "\n";
	//		}

	//		input.close();
	//		output.close();
	//	}
	//	else
	//		std::cout << "fail";
	//}




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
