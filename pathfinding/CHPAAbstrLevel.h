#pragma once
#include <vector>
//#include "CPriorityQueueEdges.h"
#include "CHierarchicalEdge.h"
#include "CCluster.h"
#include "CMapManager.h"

class CHPAAbstrLevel
{
public:
	CHPAAbstrLevel()
		: m_iClustersRowCount(0)
		, m_iClustersColCount(0)
	{
	}

	CHPAAbstrLevel(int iCurrLevel, int iClustersLen, CMapManager *lowLevelGraph)
		: m_iCurrLevel(iCurrLevel)
		, m_GraphLowLevelPtr(lowLevelGraph)
		, m_iClustersRowCount(0)
		, m_iClustersColCount(0)
		, m_iClustersLen(iClustersLen)
	{

	}

	~CHPAAbstrLevel()
	{
	}

	void BuildClusters(int iClusterWidth, int iClusterHeight);
	void BuildClustersNextLevel(CHPAAbstrLevel *lowerAbstrLevelPtr);
	void BuildHorizontalEntrancesNextLevel(CHPAAbstrLevel *lowerAbstrLevelPtr, int iFirstLvlClusterWidth, int iFirstLvlClusterHeight);
	void BuildVerticalEntrances(int iClusterWidth, int iClusterHeight, int iMaxEntranceLength);
	void BuildHorizontalEntrances(int iClusterWidth, int iClusterHeight, int iMaxEntranceLength);
	void BuildIntraEdges();
	void DrawEntrances();
	void DrawClusterBorders();

	auto& GetGraph() { return m_Graph; }
	auto& GetClusters() { return m_Clusters; }
	int GetClustersRowCount() { return m_iClustersRowCount; }
	int GetClustersColCount() { return m_iClustersColCount; }
protected:
	bool AddEntranceToClusterAndGraph(CPriorityQueueEdges &entrance, int iClusterId);
	void ConnectNodes(CPriorityQueueEdges &firstE, int iFirstClId, CPriorityQueueEdges &secondE, int iSecondClId);

private:
	int CalcPosInGraph(CPriorityQueueEdges &firstE, int iFirstClId);
	int CalcNewClusterId(CGridTile &tile,int iFirstLvlClusterWidth, int iFirstLvlClusterHeight);

	std::vector<CHierarchicalEdge> m_Graph; // graph on each level of abstraction
	std::vector<CCluster> m_Clusters;

	CMapManager *m_GraphLowLevelPtr;
	int	m_iCurrLevel;
	int m_iClustersRowCount;
	int m_iClustersColCount;
	int m_iClustersLen;
};

