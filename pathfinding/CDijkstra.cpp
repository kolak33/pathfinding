#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <time.h> 
#include "CDijkstra.h"
#include <limits>


void CDijkstra::AddNeighbourEdgesToGraph(int iHeight, int iWidth)
{


}

void CDijkstra::InitGraph()
{
	m_Graph.resize(m_MapManager.GetMapHeight() * m_MapManager.GetMapWidth());
	auto gridMap = m_MapManager.GetGridMap();

	//TODO zliczanie i rezerwowanie potrzebnej ilosci miejsca wczesniej, aby nie bylo zadnych realokacji
	for (int iHeight = 0, iNodeId = 0; iHeight < m_MapManager.GetMapHeight(); ++iHeight)
	{
		for (int iWidth = 0; iWidth < m_MapManager.GetMapWidth(); ++iWidth, ++iNodeId)
		{
			//1. ADD UPPER NEIGHBOUR
			if (iHeight > 0 && gridMap[iHeight - 1][iWidth].GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(gridMap[iHeight - 1][iWidth].GetGridId(), STRAIGHT_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//2. ADD BOTTOM NEIGHBOUR
			if (iHeight + 1 < m_MapManager.GetMapHeight() && gridMap[iHeight + 1][iWidth].GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(gridMap[iHeight + 1][iWidth].GetGridId(), STRAIGHT_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}
			//3. ADD LEFT NEIGHBOUR
			if (iWidth > 0 && gridMap[iHeight][iWidth - 1].GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(gridMap[iHeight][iWidth - 1].GetGridId(), STRAIGHT_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//4. ADD RIGHT NEIGHBOUR
			if (iWidth + 1 < m_MapManager.GetMapWidth() && gridMap[iHeight][iWidth + 1].GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(gridMap[iHeight][iWidth + 1].GetGridId(), STRAIGHT_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//5. ADD UP-LEFT DIAGONAL
			if (iHeight > 0 && iWidth > 0
				&& gridMap[iHeight - 1][iWidth].GetGridTypeEnum() == GridTypePassable
				&& gridMap[iHeight][iWidth - 1].GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(gridMap[iHeight - 1][iWidth - 1].GetGridId(), DIAGONAL_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//6. ADD UP-RIGHT DIAGONAL
			if (iHeight > 0
				&& iWidth + 1 < m_MapManager.GetMapWidth()
				&& gridMap[iHeight - 1][iWidth].GetGridTypeEnum() == GridTypePassable
				&& gridMap[iHeight][iWidth + 1].GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(gridMap[iHeight - 1][iWidth + 1].GetGridId(), DIAGONAL_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//7. ADD DOWN-LEFT DIAGONAL
			if (iHeight + 1 < m_MapManager.GetMapHeight()
				&& iWidth > 0
				&& gridMap[iHeight][iWidth - 1].GetGridTypeEnum() == GridTypePassable
				&& gridMap[iHeight + 1][iWidth].GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(gridMap[iHeight + 1][iWidth - 1].GetGridId(), DIAGONAL_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//8. ADD DOWN-RIGHT DIAGONAL
			if (iHeight + 1 < m_MapManager.GetMapHeight()
				&& iWidth + 1 < m_MapManager.GetMapWidth()
				&& gridMap[iHeight][iWidth + 1].GetGridTypeEnum() == GridTypePassable
				&& gridMap[iHeight + 1][iWidth].GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(gridMap[iHeight + 1][iWidth + 1].GetGridId(), DIAGONAL_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			m_Graph[iNodeId].SetNeighbourIterInit();
		}
	}
}

//
//bool operator <(const CPriorityQueueNode &lhs, const CPriorityQueueNode &rhs)
//{
//	return (lhs.GetF() < rhs.GetF());
//}

struct ComparePriorityQueueNode
{
	bool operator()(const CPriorityQueueNode &lhs, const CPriorityQueueNode &rhs)
	{
		return (lhs.GetF() > rhs.GetF());
	}
};


void CDijkstra::FindShortestPath(int iStartNode, int iGoalNode)
{
	//TODO wywalic to gdzies indziej
	int iNodesCount = m_MapManager.GetMapNodesCount();
	m_previous.resize(iNodesCount);
	m_distSoFar.resize(iNodesCount);
	double maxDistance = (std::numeric_limits<double>::max)();
	m_distSoFar.assign(iNodesCount, maxDistance);
	m_closedList.assign(iNodesCount, false);

	std::priority_queue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;

	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;
	//m_closedList[iStartNode] = true;
	CPriorityQueueNode firstNode(iStartNode, 0.0);

	dijQueueNode.push(firstNode);
	long lVisitedNodes = 0;
	while (!dijQueueNode.empty())
	{
		auto currentNode = dijQueueNode.top();
		auto current = currentNode.GetNodeId();
		dijQueueNode.pop();

		if (m_closedList[current] == true) // juz rozpatrzylismy ten wierzcholek
			continue;

		m_closedList[current] = true;
		++lVisitedNodes;
		
		if (current == iGoalNode) break;

		auto nextNeighbour = m_Graph[current].GetNextNeighbour();
		while (nextNeighbour != m_Graph[current].GetIterEnd())
		{
			double new_cost = m_distSoFar[current] + nextNeighbour->GetWeight(); //m_distSoFar chyba niepotrzebne skoro przechowuje to w node'ach
			if (m_closedList[nextNeighbour->GetNodeId()] != true
				&& m_distSoFar[nextNeighbour->GetNodeId()] > new_cost)
			{
				m_distSoFar[nextNeighbour->GetNodeId()] = new_cost;
				m_previous[nextNeighbour->GetNodeId()] = current;
				nextNeighbour->SetF(new_cost);

				dijQueueNode.push(*nextNeighbour);
			}
			nextNeighbour = m_Graph[current].GetNextNeighbour();
		}
	}

	long lVisN = lVisitedNodes;
	int iNodeId = iGoalNode;
	while (m_previous[iNodeId] != -1)
	{
		//std::cout << iNodeId << "  " << std::endl;
		m_shortestPath.push_back(iNodeId);
		iNodeId = m_previous[iNodeId];
	}
}

int CDijkstra::GenerateRandomAccessibleNodeId()
{
	int iNodesCount = m_MapManager.GetMapNodesCount();
	int iHeight = m_MapManager.GetMapHeight(), iWidth = m_MapManager.GetMapWidth();

	int iRow = rand() % iHeight;
	int iCol = rand() % iWidth;
	auto gridMap = m_MapManager.GetGridMap();

	while (gridMap[iRow][iCol].GetGridTypeEnum() != GridTypePassable)
	{
		iRow = rand() % iHeight;
		iCol = rand() % iWidth;
	}
	
	return gridMap[iRow][iCol].GetGridId();
}

void CDijkstra::SearchPathTest()
{
	srand(time(NULL));

	int iStartNode = GenerateRandomAccessibleNodeId();
	int iGoalNode  = GenerateRandomAccessibleNodeId();
	
	FindShortestPath(iStartNode, iGoalNode);
}
