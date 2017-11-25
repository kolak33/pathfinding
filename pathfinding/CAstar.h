#pragma once
#include "CDijkstra.h"
#include "CCluster.h"
#include "CHierarchicalEdge.h"
#include <unordered_map>

class CAstar : public CDijkstra
{
public:
	CAstar();
	~CAstar();

	void FindShortestPath(int iStartNode, int iGoalNode, bool bSimpleSearch = false, CCluster *clusterPtr = nullptr,
		int iFirstLvlClusterWidth = -1, int iFirstLvlClusterHeight = -1, int iClustersRowCount = -1);
	void FindShortestPathInCluster(int iStartNode, int iGoalNode, CCluster &cluster);
	void FindShortestPathInClusterAbstract(int iStartNode, int iGoalNode, CCluster &cluster, std::vector<CHierarchicalEdge> &abstrGraph, int iStartOwnPos, int iGoalOwnPos);
	void FindShortestPathAbstract(int iStartNode, int iGoalNode, std::vector<CHierarchicalEdge> &abstrGraph, int iStartOwnPos, int iGoalOwnPos);

	double m_timeAllocation;

protected:
	double GetOctileHeuristic(int iStartNode, int iGoalNode);

private:
	bool NeighbourIsInClusterArea(/*CPriorityQueueEdges &neighbour*/CGridTile &tile, CCluster &cluster);

	/*template <class T>
	void InitHashMapValues(std::unordered_map<int, int> &hashMap, CCluster &cluster, T value);*/
	template <class T>
	void InitHashMapValues(std::unordered_map<int, T> &hashMap, CCluster &cluster, T value)
	{
		for (int iHeight = cluster.GetYPos(); iHeight < cluster.GetYPos() + cluster.GetClusterHeight(); ++iHeight)
		{
			for (int iWidth = cluster.GetXPos(); iWidth < cluster.GetXPos() + cluster.GetClusterWidth(); ++iWidth)
			{
				hashMap[m_MapManagerPtr->GetGraphNodeByPosition(iWidth, iHeight).GetGridTileInfo().GetGridId()] = value;
			}
		}
	}

	template <class T>
	void InitValues(std::vector<T> &vec, CCluster *clusterPtr, T value)
	{
		for (int iHeight = clusterPtr->GetYPos(); iHeight < clusterPtr->GetYPos() + clusterPtr->GetClusterHeight(); ++iHeight)
		{
			for (int iWidth = clusterPtr->GetXPos(); iWidth < clusterPtr->GetXPos() + clusterPtr->GetClusterWidth(); ++iWidth)
			{
				int tileId = m_MapManagerPtr->GetGraphNodeByPosition(iWidth, iHeight).GetGridTileInfo().GetGridId();
				vec[tileId] = value;
			}
		}
	}

	int CalcClusterIdByTile(CGridTile &tile, int iFirstLvlClusterWidth, int iFirstLvlClusterHeight, int iClustersRowCount);

	std::unordered_map<int, bool> m_closedListHash;
	std::unordered_map<int, double> m_distSoFarHash;

	bool m_bPreallocatedHash;

};

