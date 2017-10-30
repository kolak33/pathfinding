#pragma once
#include "CMapManager.h"
#include "CPriorityQueueNode.h"
#include "CPriorityQueueEdges.h"
#include <list>
#include <queue>

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

protected:

	CMapManager						*m_MapManagerPtr;

	std::vector<int>				 m_previous;
	std::vector<double>				 m_distSoFar;
	std::vector<bool>				 m_closedList;

	std::vector<int>				 m_shortestPath; //TODO to bedzie zwracane

private:
	void AddNeighbourEdgesToGraph(int iHeight,int iWidth);
	int GenerateRandomAccessibleNodeId();

};

