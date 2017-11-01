#pragma once
#include "CMapManager.h"
#include "CDijkstra.h"
#include "CStatistics.h"

class CExperimentsHandler
{
public:
	CExperimentsHandler()
	{
		m_Dijkstra.SetMapManagerPtr(&m_MapManager);
		//m_Dijkstra.SearchPathTest();
		int iNumExperiments = 10;
		DoDijkstraExperiments(iNumExperiments);
	}
	~CExperimentsHandler()
	{
	}

	auto& GetMapManager() { return m_MapManager; }
	auto& GetDijkstra() { return m_Dijkstra; }

private:
	CMapManager m_MapManager;
	CDijkstra	m_Dijkstra;
	std::vector<CStatistics> m_StatisticCollection;


private:
	void DoDijkstraExperiments(int iNumExperiments);
};

