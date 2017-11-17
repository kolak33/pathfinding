#pragma once
#include <vector>
#include "CHPAAbstrLevel.h"
#include "CMapManager.h"

class CHPAStar
{
public:
	CHPAStar() //TODO popatrzec rozne parametry, bedzie trzeba potestowac mocno
		: m_iFirstLvlClusterHeight(10)
		, m_iFirstLvlClusterWidth(10)
		, m_iMaxSingleEntranceLen(6)
		, m_iMaxLevel(1)
		, m_iClustersInGroup(2)
		, m_iMaxLvlStartGoalNodes(0)
	{

	}
	~CHPAStar()
	{
	}

	void SetLowLevelGraphPtr(CMapManager *mapManager) //set MapManagerPtr
	{
		m_GraphLowLevelPtr = mapManager;
		m_AStar.SetMapManagerPtr(m_GraphLowLevelPtr);
	}

	void DrawMap()
	{
		m_GraphLowLevelPtr->DrawMap();

		m_AllAbstrLevels[0].DrawEntrances();
		//m_AllAbstrLevels[3].DrawGraphConnections();	
		//m_AllAbstrLevels[3].DrawStartGoalNodes(m_iStart, m_iGoal);

		m_GraphLowLevelPtr->DrawShortestPath(m_shortestPath);
	}

	void Preprocessing();
	void FindShortestPath(int iStartId, int iGoalId);
	CStatistics GetStatistics() { return m_Statistics; }

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
	int m_iMaxLvlStartGoalNodes;

	std::vector<int> m_shortestPath;
	CStatistics	m_Statistics;
	CAstar m_AStar;

	//TEMP
	int m_iStart, m_iGoal;
};

