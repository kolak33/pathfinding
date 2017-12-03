#include "stdafx.h"
#include "CExperimentsHandler.h"
#include <regex>
#include <fstream>
#include <boost\filesystem.hpp>



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
        auto stats = m_Dijkstra.GetStatistics();
        m_StatsDijkstra.AddStats(stats);
		//m_StatisticCollectionDijkstra.push_back(m_Dijkstra.GetStatistics());

		m_AStar.FindShortestPath(iStartNode, iGoalNode);
        stats = m_AStar.GetStatistics();
        m_StatsAStar.AddStats(stats);
		//m_AStar.DrawMap();
		//m_StatisticCollectionAStar.push_back(m_AStar.GetStatistics());

		if(i%10 == 0)
			std::cout << i << " : DONE" << std::endl;
	}

	PrintStats();
}

void CExperimentsHandler::DoTestSuiteExperiments(std::string &strTestSuiteLocationPath)
{
	//std::string strTestMapSuite = strTestsLocationPrefix + strMapName;
	std::string strTestMapSuite = std::regex_replace(strTestSuiteLocationPath, std::regex(".map"), ".txt");

	
	std::ifstream testFileSuite(strTestMapSuite.c_str());
	ATLASSERT(testFileSuite.is_open() == true);
	if (testFileSuite.is_open())
	{
		srand(time(NULL));
		std::string strTemp;
		int iTestCount = 0, iStartNode = 0, iGoalNode = 0;
		testFileSuite >> strTemp; //"testCount:"
		testFileSuite >> iTestCount;
		iTestCount = 200; //TODO

		m_HPAStar.Preprocessing();

		for (int i = 0; i < iTestCount; ++i)
		{
			testFileSuite >> iStartNode;
			testFileSuite >> iGoalNode;
			

			//m_Dijkstra.FindShortestPath(iStartNode, iGoalNode);
			//m_StatsDijkstra.AddStats(m_Dijkstra.GetStatistics());

			m_AStar.FindShortestPath(iStartNode, iGoalNode);
			auto stats = m_AStar.GetStatistics();
			m_StatsAStar.AddStats(stats);

			m_HPAStar.FindShortestPath(iStartNode, iGoalNode);
			stats = m_HPAStar.GetStatistics();
			m_StatsHPAStar.AddStats(stats);

			m_JPSearch.FindShortestPath(iStartNode, iGoalNode);
			stats = m_JPSearch.GetStatistics();
			m_StatsJPSearch.AddStats(stats);

			m_Fringe.FindShortestPath(iStartNode, iGoalNode);
			stats = m_Fringe.GetStatistics();
			m_StatsFringe.AddStats(stats);
			
					

            /*stats = m_HPAStar.GetAddStart();
            m_StatsAddStart.AddStats(stats);
			stats = m_HPAStar.GetSearchAbstr();
			m_SearchAtFirstAbstractLvl.AddStats(stats);
			stats = m_HPAStar.GetTimeAlloc();
			m_StatsTimeAlloc.AddStats(stats);
			stats = m_HPAStar.GetStatRefineSearch();
			m_StatsRefineSearchTime.AddStats(stats);*/

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
	//std::cout << "\n\nASTAR:\n";
	//m_StatsAStar.PrintAvgStats();
	//m_StatsAStar.PrintAllStats();

	//std::cout << "\n\nFRINGE:\n";
	//m_StatsFringe.PrintAvgStats();
	//m_StatsFringe.PrintAllStats();

	//std::cout << "\n\nJPSEARCH:\n";
	//m_StatsJPSearch.PrintAvgStats();

	//std::cout << "\n\nHPA REFINE, ONLY SEARCHES: \n";
	//m_StatsRefineSearchTime.PrintAvgStats();

	//std::cout << "\n\nHPA FIRST LVL ABSTR:\n";
	//m_SearchAtFirstAbstractLvl.PrintAvgStats();

	//std::cout << "\n\nHPA TIME ALLOC IN REFINING PATH:\n";
	//m_StatsTimeAlloc.PrintAvgStats();

	//std::cout << "\n\nHPASTAR:\n";
	////m_StatsHPAStar.PrintAllStats();
	//m_StatsHPAStar.PrintAvgStats();

	//std::cout << "\n\nHPASTAR ADD START:\n";
	////m_StatsAddStart.PrintAllStats();
	//m_StatsAddStart.PrintAvgStats();
}
