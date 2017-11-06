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
	{
	}

	CHPAAbstrLevel(int iCurrLevel, CMapManager *lowLevelGraph)
		: m_iCurrLevel(iCurrLevel)
		, m_GraphLowLevelPtr(lowLevelGraph)
	{

	}

	~CHPAAbstrLevel()
	{
	}

	void BuildClusters(int iClusterWidth, int iClusterHeight);
	void BuildEntrances(int iClusterWidth, int iClusterHeight, int iMaxEntranceLength);

private:
	std::vector<CHierarchicalEdge> m_Graph; // graph on each level of abstraction
	std::vector<CCluster> m_Clusters;

	CMapManager *m_GraphLowLevelPtr;
	int	m_iCurrLevel;
};

