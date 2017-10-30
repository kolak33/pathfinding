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
		m_MapManager.InitMapFromFile();
		InitGraph();
		SearchPathTest();
	}

	~CDijkstra()
	{
	}

	void DrawMap()
	{
		m_MapManager.DrawMap();
		m_MapManager.DrawShortestPath(m_shortestPath);
	}

	void InitGraph();
	void FindShortestPath(int iStartNode,int iGoalNode);
	void SearchPathTest();

protected:
	//typedef NeighbourContainer NC;
	CMapManager m_MapManager;
	std::vector< CPriorityQueueEdges > m_Graph;

	std::vector<int>				 m_previous;
	std::vector<double>				 m_distSoFar;
	std::vector<bool>				 m_closedList;

	std::vector<int>				 m_shortestPath;

	//std::vector< NeighbourContainer<T> > m_Graph;
private:
	void AddNeighbourEdgesToGraph(int iHeight,int iWidth);
	int GenerateRandomAccessibleNodeId();

};

