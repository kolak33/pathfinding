#include "stdafx.h"
#include "CExperimentsHandler.h"


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
		m_StatisticCollectionDijkstra.push_back(m_Dijkstra.GetStatistics());

		m_AStar.FindShortestPath(iStartNode, iGoalNode);
		//m_AStar.DrawMap();
		m_StatisticCollectionAStar.push_back(m_AStar.GetStatistics());

		if(i%10 == 0)
			std::cout << i << " : DONE" << std::endl;
	}

	CalcAvgStats();
}

void CExperimentsHandler::CalcAvgStats()
{
	long long lAvgNodesExpAStar = 0, lAvgNodesExpDijk = 0;
	long long lAvgNodesVisAStar = 0, lAvgNodesVisDijk = 0;
	long long lAvgOpenSetAStar = 0, lAvgOpenSetDijk = 0;
	double lAvgTimeAStar = 0.0, lAvgTimeDijk = 0.0;

	for (int i = 0; i < m_StatisticCollectionDijkstra.size(); ++i)
	{
		lAvgNodesExpAStar += m_StatisticCollectionAStar[i].GetNodesExpanded();
		lAvgNodesVisAStar += m_StatisticCollectionAStar[i].GetNodesVisited();
		lAvgOpenSetAStar += m_StatisticCollectionAStar[i].GetOpenSetMaxSize();
		lAvgTimeAStar += m_StatisticCollectionAStar[i].GetSearchTimeClock();

		lAvgNodesExpDijk += m_StatisticCollectionDijkstra[i].GetNodesExpanded();
		lAvgNodesVisDijk += m_StatisticCollectionDijkstra[i].GetNodesVisited();
		lAvgOpenSetDijk += m_StatisticCollectionDijkstra[i].GetOpenSetMaxSize();
		lAvgTimeDijk += m_StatisticCollectionDijkstra[i].GetSearchTimeClock();
	}

	std::cout << "Dijkstra avg stats:\n";
	std::cout << "nodesExpanded: " << lAvgNodesExpDijk / m_StatisticCollectionDijkstra.size() << std::endl;
	std::cout << "nodesVisited : " << lAvgNodesVisDijk / m_StatisticCollectionDijkstra.size() << std::endl;
	std::cout << "openSetMax   : " << lAvgOpenSetDijk / m_StatisticCollectionDijkstra.size() << std::endl;
	std::cout << "avgTime      : " << lAvgTimeDijk / m_StatisticCollectionDijkstra.size() << std::endl;

	std::cout << "\n\n\n";

	std::cout << "AStar avg stats:\n";
	std::cout << "nodesExpanded: " << lAvgNodesExpAStar / m_StatisticCollectionAStar.size() << std::endl;
	std::cout << "nodesVisited : " << lAvgNodesVisAStar / m_StatisticCollectionAStar.size() << std::endl;
	std::cout << "openSetMax   : " << lAvgOpenSetAStar / m_StatisticCollectionAStar.size() << std::endl;
	std::cout << "avgTime      : " << lAvgTimeAStar / m_StatisticCollectionAStar.size() << std::endl;
}