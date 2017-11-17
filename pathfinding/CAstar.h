#pragma once
#include "CDijkstra.h"
#include "CCluster.h"
#include "CHierarchicalEdge.h"

class CAstar : public CDijkstra
{
public:
	CAstar();
	~CAstar();

	void FindShortestPath(int iStartNode, int iGoalNode);
	void FindShortestPathInCluster(int iStartNode, int iGoalNode, CCluster &cluster);
	void FindShortestPathInClusterAbstract(int iStartNode, int iGoalNode, CCluster &cluster, std::vector<CHierarchicalEdge> &abstrGraph);
	void FindShortestPathAbstract(int iStartNode, int iGoalNode, std::vector<CHierarchicalEdge> &abstrGraph);

private:
	bool NeighbourIsInClusterArea(/*CPriorityQueueEdges &neighbour*/CGridTile &tile, CCluster &cluster);
};

