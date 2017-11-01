#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <time.h> 
#include "CDijkstra.h"
#include <limits>
#include <chrono>


struct ComparePriorityQueueNode
{
	bool operator()(const CPriorityQueueNode &lhs, const CPriorityQueueNode &rhs)
	{
		return (lhs.GetF() > rhs.GetF());
	}
};


void CDijkstra::FindShortestPath(int iStartNode, int iGoalNode)
{
	//TODO preinicjalizacja pamieci i ponowne jej uzywanie gdzies indziej
	int iNodesCount = m_MapManagerPtr->GetMapNodesCount();
	m_previous.resize(iNodesCount);
	m_distSoFar.resize(iNodesCount);
	double maxDistance = (std::numeric_limits<double>::max)();
	m_distSoFar.assign(iNodesCount, maxDistance);
	m_closedList.assign(iNodesCount, false);

	clock_t startClockTime = clock();
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); //TODO check high resolution clock vs standard one
	std::chrono::high_resolution_clock::time_point beginHighRes = std::chrono::high_resolution_clock::now();

	auto graph = m_MapManagerPtr->GetGraph();
	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;

	std::priority_queue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;
	CPriorityQueueNode firstNode(iStartNode, 0.0);
	dijQueueNode.push(firstNode);

	long lVisitedNodes = 0;
	long lExpandedNodes = 0;
	long lOpenSetMaxSize = 0;
	long lAlreadyProcessed = 0;

	while (!dijQueueNode.empty())
	{
		if (dijQueueNode.size() > lOpenSetMaxSize)
			lOpenSetMaxSize = dijQueueNode.size();

		auto currentNode = dijQueueNode.top();
		auto current = currentNode.GetNodeId();
		dijQueueNode.pop();

		if (m_closedList[current] == true) // juz rozpatrzylismy ten wierzcholek
		{
			++lAlreadyProcessed;
			continue;
		}

		m_closedList[current] = true;
		++lExpandedNodes;
		
		if (current == iGoalNode) break;

		auto nextNeighbour = graph[current].GetNeighbours().begin();
		while (nextNeighbour != graph[current].GetIterEnd())
		{
			double new_cost = m_distSoFar[current] + nextNeighbour->GetWeight();
			if (m_closedList[nextNeighbour->GetNodeId()] != true
				&& m_distSoFar[nextNeighbour->GetNodeId()] > new_cost)
			{
				++lVisitedNodes;
				m_distSoFar[nextNeighbour->GetNodeId()] = new_cost;
				m_previous[nextNeighbour->GetNodeId()] = current;
				nextNeighbour->SetF(new_cost);

				dijQueueNode.push(*nextNeighbour);
			}
			nextNeighbour++;
		}
	}

	int iNodeId = iGoalNode;
	while (m_previous[iNodeId] != -1)
	{
		m_shortestPath.push_back(iNodeId);
		iNodeId = m_previous[iNodeId];
	}

	clock_t endClockTime = clock();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::chrono::high_resolution_clock::time_point endHighRes = std::chrono::high_resolution_clock::now();

	double timeElapsedClock = (endClockTime - startClockTime) * 1000 / CLOCKS_PER_SEC;
	double timeElapsedMS = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	double timeElapsedNS = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	double timeElapsedHighResMS = std::chrono::duration_cast<std::chrono::microseconds>(endHighRes - beginHighRes).count();

	m_Statistics.SetSearchTimeChrono(timeElapsedMS);
	m_Statistics.SetSearchTimeClock(timeElapsedClock);
	m_Statistics.SetNodesExpanded(lExpandedNodes);
	m_Statistics.SetNodesVisited(lVisitedNodes);
	m_Statistics.SetOpenSetMaxSize(lOpenSetMaxSize);
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
