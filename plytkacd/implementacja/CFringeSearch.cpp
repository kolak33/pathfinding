#include "stdafx.h"
#include "CFringeSearch.h"


CFringeSearch::CFringeSearch()
	: CAstar(std::string("FringeSearch"))
{
}


CFringeSearch::~CFringeSearch()
{
}


void CFringeSearch::FindShortestPath(int iStartNode, int iGoalNode)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	double maxDistance = (std::numeric_limits<double>::max)();
	int iNodesCount = m_MapManagerPtr->GetMapNodesCount();

	auto& graph = m_MapManagerPtr->GetGraph();
	auto& goalGridTile = graph[iGoalNode].GetGridTileInfo();

	m_shortestPath.clear();
	if (!m_bPreallocatedResources)
	{
		int iCnt = m_MapManagerPtr->GetGraph().size();
		m_previous.assign(iCnt, maxDistance);
		m_distSoFar.assign(iCnt, maxDistance);
		m_closedList.clear();
		m_frontierIterPos.clear();
		m_bPreallocatedResources = true;
	}
	else
	{
		m_distSoFar.assign(iNodesCount, maxDistance);
		m_closedList.clear();
		m_frontierIterPos.clear();
	}

	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;

	std::list<CPriorityQueueNode> frontierList;
	CPriorityQueueNode firstNode(iStartNode, 0.0);

	frontierList.push_back(firstNode);
	m_frontierIterPos[iStartNode] = frontierList.begin();

	long lVisitedNodes = 0;
	long lExpandedNodes = 0;
	long lOpenSetMaxSize = 0;
	long lAlreadyProcessed = 0;

	CPriorityQueueNode currentNode;
	int current;
	int currentFVal;
	int cleanCount = 0;
	m_bPathFound = false;
	double fLimit = GetOctileHeuristic(iStartNode, iGoalNode);
	
	while (m_bPathFound == false && !frontierList.empty())
	{
		double fMin = (std::numeric_limits<double>::max)();
		if (frontierList.size() > lOpenSetMaxSize)
			lOpenSetMaxSize = frontierList.size();
		double minFValueInIter = fMin;

		for (auto& frontierIter = frontierList.begin(); frontierIter != frontierList.end(); ++frontierIter)
		{
			double gValFrontier = m_distSoFar[frontierIter->GetNodeId()];
			double newFValue = gValFrontier + GetOctileHeuristic(frontierIter->GetNodeId(), iGoalNode);

			if (newFValue > fLimit)
			{
				fMin = (std::min)(newFValue, fMin);
				++lVisitedNodes;
				continue;
			}

			if (frontierIter->GetNodeId() == iGoalNode)
			{
				m_bPathFound = true;
				break;
			}
			auto &neighbours = graph[frontierIter->GetNodeId()].GetNeighbours();

			for (auto& neigh : neighbours)
			{
				double gValNeigh = gValFrontier + neigh.GetWeight();
				if (gValNeigh >= m_distSoFar[neigh.GetNodeId()])
					continue;

				auto compFVal = gValNeigh + GetOctileHeuristic(neigh.GetNodeId(), iGoalNode);
				minFValueInIter = (std::min)(compFVal, minFValueInIter);

				auto succPosInFrontier = m_frontierIterPos.find(neigh.GetNodeId());
				if (succPosInFrontier != m_frontierIterPos.end())
				{
					frontierList.erase(succPosInFrontier->second);
					m_frontierIterPos.erase(succPosInFrontier);
				}

				auto nextIter = std::next(frontierIter);//frontierIter++;
				auto insertedIter = frontierList.insert(nextIter, neigh); //insert after frontierIter
				m_frontierIterPos[neigh.GetNodeId()] = insertedIter;

				m_distSoFar[neigh.GetNodeId()] = gValNeigh;
				m_previous[neigh.GetNodeId()] = frontierIter->GetNodeId();
			}

			++lExpandedNodes;
			
			m_frontierIterPos.erase(frontierIter->GetNodeId());
			frontierIter = frontierList.erase(frontierIter);
			
			if (frontierIter == frontierList.end())
				break;
		}

		fLimit = (std::min)(fMin, minFValueInIter);
	}

	if (m_bPathFound)
	{
		m_shortestPathLength = m_distSoFar[iGoalNode];
		int iNodeId = iGoalNode;
		while (m_previous[iNodeId] != -1)
		{
			m_shortestPath.push_back(iNodeId);
			iNodeId = m_previous[iNodeId];
		}
		m_shortestPath.push_back(iNodeId);
	}
	else
	{
		std::cout << "\nFringe FAILED";
	}

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	double timeElapsedMS = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	m_Statistics.SetPathLength(m_shortestPathLength);
	m_Statistics.SetSearchTimeChrono(timeElapsedMS);
	m_Statistics.SetNodesExpanded(lExpandedNodes);
	m_Statistics.SetOpenSetMaxSize(lOpenSetMaxSize);
	m_Statistics.SetNodesVisited(lVisitedNodes);
}

