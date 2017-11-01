#pragma once
#include "CMapManager.h"
#include "CDijkstra.h"
#include "CAstar.h"
#include "CStatistics.h"

class CExperimentsHandler
{
public:
	CExperimentsHandler()
	{
		m_Dijkstra.SetMapManagerPtr(&m_MapManager);
		m_AStar.SetMapManagerPtr(&m_MapManager);
		//m_Dijkstra.SearchPathTest();
		int iNumExperiments = 100;
		DoDijkstraExperiments(iNumExperiments);
	}
	~CExperimentsHandler()
	{
	}

	auto& GetMapManager() { return m_MapManager; }
	auto& GetDijkstra() { return m_Dijkstra; }
	auto& GetAStar() { return m_AStar; }

private:
	CMapManager m_MapManager;
	CDijkstra	m_Dijkstra;
	CAstar		m_AStar;
	std::vector<CStatistics> m_StatisticCollectionDijkstra;
	std::vector<CStatistics> m_StatisticCollectionAStar;


private:
	void DoDijkstraExperiments(int iNumExperiments);
	void CalcAvgStats();
};

