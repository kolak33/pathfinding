#pragma once
#include <vector>
//#include "CPriorityQueueEdges.h"
#include "CHierarchicalEdge.h"
#include "CCluster.h"
#include "CMapManager.h"
#include "CAstar.h"

class CHPAAbstrLevel
{
public:
	CHPAAbstrLevel()
		: m_iClustersRowCount(0)
		, m_iClustersColCount(0)
	{
	}

	CHPAAbstrLevel(int iCurrLevel, int iClustersLen, CMapManager *lowLevelGraph, int iFirstLvlClusterWidth, int iFirstLvlClusterHeight)
		: m_iCurrLevel(iCurrLevel)
		, m_GraphLowLevelPtr(lowLevelGraph)
		, m_iClustersRowCount(0)
		, m_iClustersColCount(0)
		, m_iClustersLen(iClustersLen)
		, m_iFirstLvlClusterWidth(iFirstLvlClusterWidth)
		, m_iFirstLvlClusterHeight(iFirstLvlClusterHeight)
		, m_bGluedHeightCluster(false)
		, m_bGluedWidthCluster(false)
		, m_AddedEdges(0)
	{

	}

	~CHPAAbstrLevel()
	{
	}
	//preprocessing
	void BuildClusters();
	void BuildClustersNextLevel(CHPAAbstrLevel *lowerAbstrLevelPtr);
	void BuildEntrancesNextLevel(CHPAAbstrLevel *lowerAbstrLevelPtr);
	void BuildVerticalEntrances(int iMaxEntranceLength);
	void BuildHorizontalEntrances(int iMaxEntranceLength);
	void BuildIntraEdges(std::vector<CHierarchicalEdge> &graphLowerLevel);
	void ClusterByIdPrepocessing();

	//drawing
	void DrawEntrances();
	void DrawStartGoalNodes(int iStart, int iGoal);
	void DrawClusterBorders();
	void DrawGraphConnections();

	//returns false if start&goal node are in the same cluster - they weren't added to the graph
	bool AddStartAndGoalNodesToGraph(CAstar &AStar, int iStartId, int iGoalId, std::vector<CHierarchicalEdge> &graphLowerLevel);
	void DeleteStartAndGoalNodesFromGraph(int iStartId, int iGoalId);
	int CalcClusterId(int iNodeId); //returns clusterId to which node belongs

	

	auto& GetGraph() { return m_Graph; }
	auto& GetClusters() { return m_Clusters; }
	int GetClustersRowCount() { return m_iClustersRowCount; }
	int GetClustersColCount() { return m_iClustersColCount; }
	auto& GetEdgePositions() { return m_EdgePositions; }
	void SetEdgePosLowerLvl(std::vector<int> *edgePosLowerLvlPtr)
	{
		m_EdgePosLowerLvlPtr = edgePosLowerLvlPtr;
	}
protected:
	bool AddEntranceToClusterAndGraph(CPriorityQueueEdges &entrance, int iClusterId);
	void ConnectNodes(CPriorityQueueEdges &firstE, int iFirstClId, CPriorityQueueEdges &secondE, int iSecondClId);

private:
	int CalcPosInGraph(CPriorityQueueEdges &firstE, int iFirstClId);
	int CalcNewClusterId(CGridTile &tile); //returns clusterId for a tile
	int FindEdgePositionInGraph(std::vector<CHierarchicalEdge> &graph, int iTileId, bool bAddStart, bool bFindInCurrentGraph);
	void CreateIntraEdges(CAstar &AStar, CCluster &cluster, std::vector<CHierarchicalEdge> &graphLowerLevel,
		int iFirstNodeId, int iSecondNodeId, bool bAddStart);
	void AddIntraEdgesToGraph(double dShortestPathLength, int iClusterId, int iFirstNodeId, int iSecondNodeId, int iLowerLvlFirstNodeId, int iLowerLvlSecondNodeId, bool bAddStart);
	bool NeighbourAlreadyAdded(CHierarchicalEdge &edge, int neighbourId);
	void AddNodeToGraph(CAstar &AStar, int iNodeId, std::vector<CHierarchicalEdge> &graphLowerLevel);
	bool IsEntrance(int iNodeId);
	void DeleteStartOrGoal(int iNodeId);

	std::vector<CHierarchicalEdge> m_Graph; // graph on each level of abstraction
	std::vector<CCluster> m_Clusters;

	std::vector<int> m_EdgePositions;
	std::vector<int> *m_EdgePosLowerLvlPtr;

	CMapManager *m_GraphLowLevelPtr;
	int	m_iCurrLevel;
	int m_iClustersRowCount;
	int m_iClustersColCount;
	int m_iClustersLen;
	int m_iFirstLvlClusterWidth;
	int m_iFirstLvlClusterHeight;

	//TEMP
	bool m_bGluedWidthCluster;
	bool m_bGluedHeightCluster;

	std::vector<int> m_ClusterIdByTile;
	int m_AddedEdges;
};

