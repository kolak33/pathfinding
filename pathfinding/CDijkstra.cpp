#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <time.h> 
#include "CDijkstra.h"
#include <limits>


void CDijkstra::AddNeighbourEdgesToGraph(int iHeight, int iWidth)
{


}

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
	int iNodesCount = m_MapManagerPtr->GetMapNodesCount();
	m_previous.resize(iNodesCount);
	m_distSoFar.resize(iNodesCount);
	double maxDistance = (std::numeric_limits<double>::max)();
	m_distSoFar.assign(iNodesCount, maxDistance);
	m_closedList.assign(iNodesCount, false);

	std::priority_queue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;
	auto graph = m_MapManagerPtr->GetGraph();

	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;
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

		auto nextNeighbour = graph[current].GetNeighbours().begin();
		while (nextNeighbour != graph[current].GetIterEnd())
		{
			double new_cost = m_distSoFar[current] + nextNeighbour->GetWeight();
			if (m_closedList[nextNeighbour->GetNodeId()] != true
				&& m_distSoFar[nextNeighbour->GetNodeId()] > new_cost)
			{
				m_distSoFar[nextNeighbour->GetNodeId()] = new_cost;
				m_previous[nextNeighbour->GetNodeId()] = current;
				nextNeighbour->SetF(new_cost);

				dijQueueNode.push(*nextNeighbour);
			}
			nextNeighbour++;
		}
	}

	long lVisN = lVisitedNodes;
	int iNodeId = iGoalNode;
	while (m_previous[iNodeId] != -1)
	{
		m_shortestPath.push_back(iNodeId);
		iNodeId = m_previous[iNodeId];
	}
}

int CDijkstra::GenerateRandomAccessibleNodeId()
{
	int iNodesCount = m_MapManagerPtr->GetMapNodesCount();

	int iRand = rand() % iNodesCount;
	while (m_MapManagerPtr->GetGraph()[iRand].GetGridTileInfo().GetGridTypeEnum() != GridTypePassable)
		iRand = rand() % iNodesCount;

	return iRand;
}

void CDijkstra::SearchPathTest()
{
	srand(time(NULL));

	int iStartNode = GenerateRandomAccessibleNodeId();
	int iGoalNode  = GenerateRandomAccessibleNodeId();
	
	FindShortestPath(iStartNode, iGoalNode);
}
