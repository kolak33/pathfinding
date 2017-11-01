#pragma once
#include "CMapManager.h"
#include "CPriorityQueueNode.h"
#include "CPriorityQueueEdges.h"
#include "CStatistics.h"
#include <list>
#include <queue>

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <limits>
#include <chrono>

//template< template <typename T, typename = std::allocator<CGridTile*> > class NeighbourContainer >
class CDijkstra
{
public:
	CDijkstra()
	{
		//m_MapManager.InitMapFromFile();
		//InitGraph();
		//SearchPathTest();
	}

	~CDijkstra()
	{
	}

	void DrawMap()
	{
		m_MapManagerPtr->DrawMap();
		m_MapManagerPtr->DrawShortestPath(m_shortestPath);
	}

	void FindShortestPath(int iStartNode,int iGoalNode);
	void SearchPathTest();
	void SetMapManagerPtr(CMapManager *mapManager)
	{
		m_MapManagerPtr = mapManager;
	}
	int GenerateRandomAccessibleNodeId();

	CStatistics GetStatistics() { return m_Statistics; }
protected:

	CMapManager						*m_MapManagerPtr;

	std::vector<int>				 m_previous;
	std::vector<double>				 m_distSoFar;
	std::vector<bool>				 m_closedList;

	std::vector<int>				 m_shortestPath; //TODO to bedzie zwracane

	CStatistics						 m_Statistics;
};

