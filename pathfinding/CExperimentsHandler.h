#pragma once
#include "CMapManager.h"
#include "CDijkstra.h"
#include "CAstar.h"
#include "CHPAStar.h"
#include "CJPSearch.h"
#include "CFringeSearch.h"
#include "CStatistics.h"
#include "CStatisticsSummary.h"

class CExperimentsHandler
{
public:
	CExperimentsHandler()
	{
		m_Dijkstra.SetMapManagerPtr(&m_MapManager);
		m_AStar.SetMapManagerPtr(&m_MapManager);
		m_HPAStar.SetLowLevelGraphPtr(&m_MapManager);
		m_JPSearch.SetMapManagerPtr(&m_MapManager);
		m_Fringe.SetMapManagerPtr(&m_MapManager);
		//m_Dijkstra.SearchPathTest();
		//int iNumExperiments = 100;
		//DoDijkstraExperiments(iNumExperiments);
		DoTestSuiteExperiments();
	}
	~CExperimentsHandler()
	{
	}

	auto& GetMapManager() { return m_MapManager; }
	auto& GetDijkstra() { return m_Dijkstra; }
	auto& GetAStar() { return m_AStar; }
	auto& GetHPAStar() { return m_HPAStar; }
	auto& GetJPSearch() { return m_JPSearch; }
	auto& GetFringe() { return m_Fringe; }

private:
	//TODO kiedys dziedziczenie po abstr klasie z funkcja searchPath
	CMapManager m_MapManager;
	CDijkstra	m_Dijkstra;
	CAstar		m_AStar;
	CHPAStar	m_HPAStar;
	CJPSearch	m_JPSearch;
	CFringeSearch m_Fringe;
	//std::vector<CStatistics> m_StatisticCollectionDijkstra;
	//std::vector<CStatistics> m_StatisticCollectionAStar;
	//TODO kiedys jakis vector
	CStatisticsSummary m_StatsDijkstra;
	CStatisticsSummary m_StatsAStar;
	CStatisticsSummary m_StatsHPAStar;
	CStatisticsSummary m_StatsJPSearch;
	CStatisticsSummary m_StatsFringe;

	CStatisticsSummary m_StatsAddStart;
	CStatisticsSummary m_SearchAtFirstAbstractLvl;
	CStatisticsSummary m_StatsTimeAlloc;
	CStatisticsSummary m_StatsRefineSearchTime;

	void DoDijkstraExperiments(int iNumExperiments);
	void DoTestSuiteExperiments();
	void PrintStats();
};

