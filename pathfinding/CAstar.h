#pragma once
#include "CDijkstra.h"
#include "CCluster.h"

class CAstar : public CDijkstra
{
public:
	CAstar();
	~CAstar();

	void FindShortestPath(int iStartNode, int iGoalNode);
	void FindShortestPathInCluster(int iStartNode, int iGoalNode, CCluster &cluster);

private:
	bool NeighbourIsInClusterArea(CPriorityQueueEdges &neighbour, CCluster &cluster);
};

