#pragma once
#include "CDijkstra.h"

class CAstar : public CDijkstra
{
public:
	CAstar();
	~CAstar();

	void FindShortestPath(int iStartNode, int iGoalNode);
};

