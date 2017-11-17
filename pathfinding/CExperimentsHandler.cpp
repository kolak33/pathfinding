#include "stdafx.h"
#include "CExperimentsHandler.h"
#include <regex>
#include <fstream>


//CExperimentsHandler::CExperimentsHandler()
//{
//}
//
//
//CExperimentsHandler::~CExperimentsHandler()
//{
//}


void CExperimentsHandler::DoDijkstraExperiments(int iNumExperiments)
{
	srand(time(NULL));
	for (int i = 0; i < iNumExperiments; ++i)
	{
		int iStartNode = m_Dijkstra.GenerateRandomAccessibleNodeId();
		int iGoalNode = m_Dijkstra.GenerateRandomAccessibleNodeId();

		//iStartNode = m_MapManager.GetMapHeight() / 2 * m_MapManager.GetMapWidth() + m_MapManager.GetMapWidth() / 5;
		//iGoalNode = m_MapManager.GetMapHeight() / 2 * m_MapManager.GetMapWidth() + 4 * m_MapManager.GetMapWidth() / 5;

		m_Dijkstra.FindShortestPath(iStartNode, iGoalNode);
		m_StatsDijkstra.AddStats(m_Dijkstra.GetStatistics());
		//m_StatisticCollectionDijkstra.push_back(m_Dijkstra.GetStatistics());

		m_AStar.FindShortestPath(iStartNode, iGoalNode);
		m_StatsAStar.AddStats(m_AStar.GetStatistics());
		//m_AStar.DrawMap();
		//m_StatisticCollectionAStar.push_back(m_AStar.GetStatistics());

		if(i%10 == 0)
			std::cout << i << " : DONE" << std::endl;
	}

	PrintStats();
}

void CExperimentsHandler::DoTestSuiteExperiments()
{
	std::string strTestMapSuite = strTestsLocationPrefix + strMapName;
	strTestMapSuite = std::regex_replace(strTestMapSuite, std::regex(".map"), ".txt");
	
	std::ifstream testFileSuite(strTestMapSuite.c_str());
	ATLASSERT(testFileSuite.is_open() == true);
	if (testFileSuite.is_open())
	{
		srand(time(NULL));
		std::string strTemp;
		int iTestCount = 0, iStartNode = 0, iGoalNode = 0;
		testFileSuite >> strTemp; //"testCount:"
		testFileSuite >> iTestCount;
		iTestCount = 10; //TODO

		m_HPAStar.Preprocessing();

		for (int i = 0; i < iTestCount; ++i)
		{
			testFileSuite >> iStartNode;
			testFileSuite >> iGoalNode;

			//m_Dijkstra.FindShortestPath(iStartNode, iGoalNode);
			//m_StatsDijkstra.AddStats(m_Dijkstra.GetStatistics());

			m_AStar.FindShortestPath(iStartNode, iGoalNode);
			m_StatsAStar.AddStats(m_AStar.GetStatistics());

			m_HPAStar.FindShortestPath(iStartNode, iGoalNode);
			m_StatsHPAStar.AddStats(m_HPAStar.GetStatistics());

			if (i % 100 == 0)
				std::cout << i << " : DONE" << std::endl;
		}

		testFileSuite.close();
		PrintStats();	
	}
}

void CExperimentsHandler::PrintStats()
{
	//std::cout << "DIJKSTRA:\n";
	//m_StatsDijkstra.PrintAllStats();
	//m_StatsDijkstra.PrintAvgStats();
	std::cout << "\n\nASTAR:\n";
	//m_StatsAStar.PrintAllStats();
	m_StatsAStar.PrintAvgStats();

	std::cout << "\n\nHPASTAR:\n";
	//m_StatsAStar.PrintAllStats();
	m_StatsHPAStar.PrintAvgStats();
}