#include "stdafx.h"
#include "CAstar.h"


CAstar::CAstar()
{
}


CAstar::~CAstar()
{
}

struct ComparePriorityQueueNode
{
	bool operator()(const CPriorityQueueNode &lhs, const CPriorityQueueNode &rhs)
	{
		return (lhs.GetF() > rhs.GetF());
	}
};

void CAstar::FindShortestPath(int iStartNode, int iGoalNode)
{
	int iNodesCount = m_MapManagerPtr->GetMapNodesCount();
	m_previous.resize(iNodesCount);
	m_distSoFar.resize(iNodesCount);
	double maxDistance = (std::numeric_limits<double>::max)();
	m_distSoFar.assign(iNodesCount, maxDistance);
	m_closedList.assign(iNodesCount, false);

	clock_t startClockTime = clock();
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); //TODO check high resolution clock vs standard one
	//std::chrono::high_resolution_clock::time_point beginHighRes = std::chrono::high_resolution_clock::now();

	auto graph = m_MapManagerPtr->GetGraph();
	auto goalGridTile = graph[iGoalNode].GetGridTileInfo();

	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;

	std::priority_queue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;

	/*auto currGridTile = graph[iStartNode].GetGridTileInfo();

	int deltaX = std::abs(currGridTile.m_iXPos - goalGridTile.m_iXPos);
	int deltaY = std::abs(currGridTile.m_iYPos - goalGridTile.m_iYPos);
	double newFValue = max(deltaX, deltaY) + DIAG_HEUR_VAL * min(deltaX, deltaY);*/

	CPriorityQueueNode firstNode(iStartNode, 0.0);
	//firstNode.SetF(newFValue);
	dijQueueNode.push(firstNode);

	long lVisitedNodes = 0;
	long lExpandedNodes = 0;
	long lOpenSetMaxSize = 0;
	long lAlreadyProcessed = 0;

	/*bool bDirectNextNodeAccess = false;
	bool bFoundBetterNeighbour = false;
	std::vector<CPriorityQueueNode>::iterator directNextNodePtr;
	double dDirectMinPathLength = (std::numeric_limits<double>::max)();
	std::vector<CPriorityQueueNode> directAccessVec;
	directAccessVec.reserve(GRID_BRANCHING_FACTOR);

	CPriorityQueueNode currentNode;
	int current;
	int currentFVal;*/

	while (!dijQueueNode.empty())
	{
		if (dijQueueNode.size() > lOpenSetMaxSize)
			lOpenSetMaxSize = dijQueueNode.size();

		//if (!bDirectNextNodeAccess)
		//{
			auto currentNode = dijQueueNode.top();
			auto current = currentNode.GetNodeId();
			//currentFVal = currentNode.GetF();
			dijQueueNode.pop();
			//++lExpandedNodes; //todo usunac stad
	/*	}
		else
		{
			current = directNextNodePtr->GetNodeId();
			currentFVal = directNextNodePtr->GetF();
		}*/

		if (m_closedList[current] == true) // juz rozpatrzylismy ten wierzcholek
		{
			++lAlreadyProcessed;
			continue;
		}

		m_closedList[current] = true;
		++lExpandedNodes; // todo pomyslec czy to w tym miejscu

		if (current == iGoalNode) break;

		/*bDirectNextNodeAccess = false;
		bFoundBetterNeighbour = false;*/
		auto nextNeighbour = graph[current].GetNeighbours().begin();
		while (nextNeighbour != graph[current].GetIterEnd() /*&& nextNeighbour->GetNodeId() != current*/) // do not put parent node we came from
		{
			double new_cost = m_distSoFar[current] + nextNeighbour->GetWeight();
			if (m_closedList[nextNeighbour->GetNodeId()] != true
				&& m_distSoFar[nextNeighbour->GetNodeId()] > new_cost)
			{
				++lVisitedNodes;
				m_distSoFar[nextNeighbour->GetNodeId()] = new_cost;
				m_previous[nextNeighbour->GetNodeId()] = current;

				auto currGridTile = graph[nextNeighbour->GetNodeId()].GetGridTileInfo();

				int deltaX = std::abs(currGridTile.m_iXPos - goalGridTile.m_iXPos);
				int deltaY = std::abs(currGridTile.m_iYPos - goalGridTile.m_iYPos);
				double newFValue =  max(deltaX, deltaY) + DIAG_HEUR_VAL * min(deltaX, deltaY);

				nextNeighbour->SetF(new_cost + newFValue);

				/*if (dijQueueNode.size() == 0 || dijQueueNode.top().GetF() > nextNeighbour->GetF())
				{	
					if (bFoundBetterNeighbour)
					{
						directAccessVec.push_back(*directNextNodePtr);
					}
					directNextNodePtr = nextNeighbour;
					bFoundBetterNeighbour = true;
				}
				else
				{
					directAccessVec.push_back(*nextNeighbour);
				}*/
				dijQueueNode.push(*nextNeighbour);
				//dijQueueNode.emplace(*nextNeighbour);
			}
			nextNeighbour++;
		}

		//for (size_t i=0; i<directAccessVec.size(); ++i) // push from directAccess
		//	dijQueueNode.push(directAccessVec[i]);
		//directAccessVec.clear(); // capacity == 8?
		//bDirectNextNodeAccess = bFoundBetterNeighbour;
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
	double timeElapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	//double timeElapsedNS = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	//double timeElapsedHighResMS = std::chrono::duration_cast<std::chrono::microseconds>(endHighRes - beginHighRes).count();

	//TODO only in experiments
	std::vector<int>().swap(m_shortestPath); //deallocate memory for shortestpath

	m_Statistics.SetSearchTimeChrono(timeElapsedMS);
	m_Statistics.SetSearchTimeClock(timeElapsedClock);
	m_Statistics.SetNodesExpanded(lExpandedNodes);
	m_Statistics.SetNodesVisited(lVisitedNodes);
	m_Statistics.SetOpenSetMaxSize(lOpenSetMaxSize);
}