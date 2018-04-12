#pragma once
#include <vector>
#include "CHPAAbstrLevel.h"
#include "CMapManager.h"
#include "CStatisticsSummary.h"

class CHPAStar
{
public:
	CHPAStar() //TODO popatrzec rozne parametry, bedzie trzeba potestowac mocno
		: m_iFirstLvlClusterHeight(10)
		, m_iFirstLvlClusterWidth(10)
		, m_iMaxSingleEntranceLen(6)
		, m_iMaxLevel(1)
		, m_iClustersInGroup(2)
		, m_iMaxStartLvl(0)
		, m_Statistics(std::string("HPAStar"))
		, m_AStar(std::string("HPAStar"))
		, m_lMemoryUsage(0)
	{
		//m_AStar.GetStatistics().SetAlgName(std::string("HPAStar"));
	}
	~CHPAStar()
	{
	}

	void SetLowLevelGraphPtr(CMapManager *mapManager) //set MapManagerPtr
	{
		m_GraphLowLevelPtr = mapManager;
		m_AStar.SetMapManagerPtr(m_GraphLowLevelPtr);
	}

	void SetHeurFactor(double heurFactor)
	{
		m_AStar.SetHeurFactor(heurFactor);
	}

	void DrawMap()
	{
		m_GraphLowLevelPtr->DrawMap();

		m_AllAbstrLevels[0].DrawClusterBorders();
		m_AllAbstrLevels[0].DrawEntrances();
		//m_AllAbstrLevels[0].DrawGraphConnections();
		
		//m_AllAbstrLevels[3].DrawGraphConnections();	
		//m_AllAbstrLevels[3].DrawStartGoalNodes(m_iStart, m_iGoal);

		
		//CColor color(1.0f, 0.0f, 0.0f);
		//m_GraphLowLevelPtr->ColorSelectedTiles(m_nodesToColor, &color);

		m_GraphLowLevelPtr->DrawShortestPath(m_shortestPath);
	}

	void Preprocessing();
	void FindShortestPath(int iStartId, int iGoalId);
	CStatistics& GetStatistics() { return m_Statistics; }
	CStatistics& GetAddStart() { return m_StatAddStart; }
	CStatistics& GetSearchAbstr() { return m_SearchAtFirstAbstractLvl; }
	CStatistics& GetTimeAlloc() { return m_StatTimeAlloc; }
	CStatistics& GetStatRefineSearch() { return m_StatSimpleRefineSearch; }
	long GetMemoryUsage() { return m_lMemoryUsage; }
protected:
	void BuildFirstLevel();
	void AddNextLevels();
	void AddStartAndGoalNodesToGraph(int iStartId, int iGoalId);
	void RemoveStartAndGoalNodesFromGraph(int iStartId, int iGoalId);
	void RefinePath(CAstar &AStar, std::vector<int> &path, int iLvl, std::vector<int> &resultPath);

private:
	std::vector<CHPAAbstrLevel> m_AllAbstrLevels; //Higher abstraction levels with clusters
	CMapManager *m_GraphLowLevelPtr;

	int m_iClustersInGroup; // grouping together nxn adjacent clusters where n = m_iCLustersInGroup
	int m_iMaxLevel;
	int m_iFirstLvlClusterWidth;
	int m_iFirstLvlClusterHeight;
	int m_iMaxSingleEntranceLen;
	int m_iMaxStartLvl;

	std::vector<int> m_shortestPath;
	std::vector<int> m_pathEntr;
	CStatistics	m_Statistics;
	CStatistics m_StatAddStart;
	CStatistics m_SearchAtFirstAbstractLvl;
	CStatistics m_StatTimeAlloc;
	CStatistics m_StatSimpleRefineSearch;
	CStatisticsSummary m_StatSummarySimpleSearch;
	double m_timeAlloc;
	double m_timeSimpleRefineSearch;
	double m_refineNodesExpanded;


	CAstar m_AStar;
	std::vector<int> m_nodesToColor;
	//TEMP
	int m_iStart, m_iGoal;
	long m_lMemoryUsage;
};

