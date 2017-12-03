#include "stdafx.h"
#include "CJPSearch.h"



CJPSearch::CJPSearch()
	: CAstar(std::string("JPSearch"))
{
}


CJPSearch::~CJPSearch()
{
}

void CJPSearch::FindShortestPath(int iStartNode, int iGoalNode)
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
		//m_previous.resize(iCnt);
		//m_distSoFar.assign(iCnt, maxDistance);
		//m_closedList.assign(iCnt, false);
		m_previous.clear();
		m_distSoFar2.clear();
		m_closedList.clear();
		m_bPreallocatedResources = true;
	}
	else
	{
		m_previous.clear();
		m_distSoFar2.clear();
		m_closedList.clear();
		//std::fill_n(m_distSoFar.begin(), iNodesCount, maxDistance);
		//std::fill_n(m_closedList.begin(), iNodesCount, false);
	}

	m_previous[iStartNode] = -1;
	m_distSoFar2[iStartNode] = 0.0f;

	std::priority_queue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;
	CPriorityQueueNode firstNode(iStartNode, 0.0);

	dijQueueNode.push(firstNode);

	long lVisitedNodes = 0;
	long lExpandedNodes = 0;
	long lOpenSetMaxSize = 0;
	long lAlreadyProcessed = 0;


	CPriorityQueueNode currentNode;
	int current;
	int currentFVal;
	int cleanCount = 0;
	m_bPathFound = false;
	while (!dijQueueNode.empty())
	{
		if (dijQueueNode.size() > lOpenSetMaxSize)
			lOpenSetMaxSize = dijQueueNode.size();

		auto currentNode = dijQueueNode.top();
		current = currentNode.GetNodeId();
		currentFVal = currentNode.GetF();
		dijQueueNode.pop();	

		if (m_closedList.find(current) != m_closedList.end()) // juz rozpatrzylismy ten wierzcholek
		{
			++lAlreadyProcessed;
			continue;
		}

		m_closedList[current] = true;
		++lExpandedNodes; // todo pomyslec czy to w tym miejscu

		if (current == iGoalNode)
		{
			m_bPathFound = true;
			break;
		}

		IdentifySuccessors(current, iStartNode, iGoalNode, dijQueueNode);
	}

	if (m_bPathFound)
	{
		m_shortestPathLength = m_distSoFar2.find(iGoalNode)->second;
		//std::cout << "JPS shortestPath: " << m_shortestPathLength << "\n";
		ATLASSERT(m_distSoFar2.find(iGoalNode) != m_distSoFar2.end());
		int iNodeId = iGoalNode;
		ATLASSERT(m_previous.find(iNodeId) != m_previous.end());
		while (m_previous.find(iNodeId)->second != -1)
		{
			//std::cout << "JPS distSoFar: " << m_distSoFar2.find(iNodeId)->second << "\n";
			m_shortestPath.push_back(iNodeId);
			ATLASSERT(m_previous.find(iNodeId) != m_previous.end());
			iNodeId = m_previous.find(iNodeId)->second;
		}
		m_shortestPath.push_back(iNodeId);
	}
	else
	{
		std::cout << "\nJPS FAILED";
//		ATLASSERT(false);
	}
	//std::cout << "\n";

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	double timeElapsedMS = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	m_Statistics.SetPathLength(m_shortestPathLength);
	m_Statistics.SetSearchTimeChrono(timeElapsedMS);
	m_Statistics.SetNodesExpanded(lExpandedNodes);
	m_Statistics.SetOpenSetMaxSize(lOpenSetMaxSize);
}


void CJPSearch::IdentifySuccessors(int iCurrNode, int iStartNode, int iGoalNode,
	std::priority_queue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> &dijQueue)
{
	std::vector<int> neighbours; 
	FindNeighbours(neighbours, iCurrNode); //TODO zeby sie nie kopiowalo

	for (auto iNeighbour : neighbours)
	{
		int iJumpNode = Jump(iCurrNode, iNeighbour, iGoalNode);

		if (iJumpNode == -1 || m_closedList.find(iJumpNode) != m_closedList.end())
			continue;

		if (iJumpNode == 28609)
			int a = 5;
		double distBetween = GetOctileHeuristic(iCurrNode, iJumpNode); // no obstacles between nodes so the value is not heuristic
		ATLASSERT(m_distSoFar2.find(iCurrNode) != m_distSoFar2.end());
		double newDistSoFar = m_distSoFar2.find(iCurrNode)->second + distBetween;
		double newFVal = newDistSoFar + GetOctileHeuristic(iJumpNode, iGoalNode);

		auto iter = m_distSoFar2.find(iJumpNode);
		if (iter == m_distSoFar2.end() || iter->second > newDistSoFar)
		{
			if (iter == m_distSoFar2.end())
				m_distSoFar2[iJumpNode] = newDistSoFar;
			else
				iter->second = newDistSoFar;
			m_previous[iJumpNode] = iCurrNode;

			CPriorityQueueNode jumpNode(iJumpNode, -1.0); //weight is not important here
			jumpNode.SetF(newFVal);

			dijQueue.push(jumpNode);
		}
	}
}

int CJPSearch::Jump(int iCurrNode, int iNeighbour, int iGoal)
{
	if (iNeighbour == -1)
		return -1;
	if (m_MapManagerPtr->GetTileById(iNeighbour).GetGridTypeEnum() == GridTypeImpassable)
		return -1;
	if (iNeighbour == iGoal) return iGoal;
	ATLASSERT(iCurrNode != -1);
	ATLASSERT(iNeighbour != -1);
	ATLASSERT(iGoal != -1);

	auto &currTile = m_MapManagerPtr->GetTileById(iCurrNode);
	auto &neighTile = m_MapManagerPtr->GetTileById(iNeighbour);

	int dx = neighTile.m_iXPos - currTile.m_iXPos;
	int dy = neighTile.m_iYPos - currTile.m_iYPos;

	if (dx != 0 && dy != 0) 
	{
		auto *neigh1 = m_MapManagerPtr->GetNodeIfExists(neighTile.m_iXPos + dx, neighTile.m_iYPos);
		auto *neigh2 = m_MapManagerPtr->GetNodeIfExists(neighTile.m_iXPos, neighTile.m_iYPos + dy);

		if ((neigh1 != nullptr && Jump(iNeighbour, neigh1->GetGridId(), iGoal) != -1)
			|| (neigh2 != nullptr && Jump(iNeighbour, neigh2->GetGridId(), iGoal) != -1)) // ||
			return iNeighbour;
	}
	else
	{
		if (dx != 0)
		{
			if ((m_MapManagerPtr->IsPassable(neighTile.m_iXPos, neighTile.m_iYPos + 1)
				&& !m_MapManagerPtr->IsPassable(neighTile.m_iXPos - dx, neighTile.m_iYPos + 1))
				|| (m_MapManagerPtr->IsPassable(neighTile.m_iXPos, neighTile.m_iYPos - 1)
					&& !m_MapManagerPtr->IsPassable(neighTile.m_iXPos - dx, neighTile.m_iYPos - 1)))
				return iNeighbour;
		}
		else if(dy != 0)
		{
			if ((m_MapManagerPtr->IsPassable(neighTile.m_iXPos + 1, neighTile.m_iYPos)
				&& !m_MapManagerPtr->IsPassable(neighTile.m_iXPos + 1, neighTile.m_iYPos - dy))
				|| (m_MapManagerPtr->IsPassable(neighTile.m_iXPos - 1, neighTile.m_iYPos)
					&& !m_MapManagerPtr->IsPassable(neighTile.m_iXPos - 1, neighTile.m_iYPos - dy)))
				return iNeighbour;
		}
	}

	// jump diagonally towards our goal
	if ( m_MapManagerPtr->IsPassable(neighTile.m_iXPos + dx, neighTile.m_iYPos)
		&& m_MapManagerPtr->IsPassable(neighTile.m_iXPos, neighTile.m_iYPos + dy) )
	{
		auto *neigh = m_MapManagerPtr->GetNodeIfExists(neighTile.m_iXPos + dx, neighTile.m_iYPos + dy);
		if (neigh == nullptr)
			return -1;
		else
			return Jump(iNeighbour, neigh->GetGridId(), iGoal);
	}
	else
		return -1;
}

void CJPSearch::FindNeighbours(std::vector<int> &neighbours, int iCurrNode)
{
	auto &currTile = m_MapManagerPtr->GetTileById(iCurrNode);
	bool bPassable = true;
	bool bImpassable = false;
	auto iter = m_previous.find(iCurrNode);
	//TODO zwrocic uwage na przeszkody
	if (iter == m_previous.end() || iter->second == -1) // node doesn't have a parent, add all neighbours to be checked
	{	
		bool bUpAdded = AddNeighbourOnCondition(neighbours, currTile.m_iXPos, currTile.m_iYPos + 1, bPassable, currTile.m_iXPos, currTile.m_iYPos + 1);
		bool bRightAdded = AddNeighbourOnCondition(neighbours, currTile.m_iXPos + 1, currTile.m_iYPos, bPassable, currTile.m_iXPos + 1, currTile.m_iYPos);
		bool bDownAdded = AddNeighbourOnCondition(neighbours, currTile.m_iXPos, currTile.m_iYPos - 1, bPassable, currTile.m_iXPos, currTile.m_iYPos - 1);
		bool bLeftAdded = AddNeighbourOnCondition(neighbours, currTile.m_iXPos - 1, currTile.m_iYPos, bPassable, currTile.m_iXPos - 1, currTile.m_iYPos);

		if (bUpAdded && bRightAdded)
		{
			AddNeighbourOnCondition(neighbours, currTile.m_iXPos + 1, currTile.m_iYPos + 1, bPassable, currTile.m_iXPos + 1, currTile.m_iYPos + 1);
		}
		if (bRightAdded && bDownAdded)
		{
			AddNeighbourOnCondition(neighbours, currTile.m_iXPos + 1, currTile.m_iYPos - 1, bPassable, currTile.m_iXPos + 1, currTile.m_iYPos - 1);
		}
		if (bDownAdded && bLeftAdded)
		{
			AddNeighbourOnCondition(neighbours, currTile.m_iXPos - 1, currTile.m_iYPos - 1, bPassable, currTile.m_iXPos - 1, currTile.m_iYPos - 1);
		}
		if (bLeftAdded && bUpAdded)
		{
			AddNeighbourOnCondition(neighbours, currTile.m_iXPos - 1, currTile.m_iYPos + 1, bPassable, currTile.m_iXPos - 1, currTile.m_iYPos + 1);
		}
	}
	else // prune neighbours
	{
		int iParentNode = m_previous.find(iCurrNode)->second;
		ATLASSERT(m_previous.find(iCurrNode) != m_previous.end());
		auto& parentTile = m_MapManagerPtr->GetTileById(iParentNode);

		//direction of travel
		int dx = (currTile.m_iXPos - parentTile.m_iXPos) / (std::max)(std::abs(currTile.m_iXPos - parentTile.m_iXPos), 1); //TODO usunac nadmiar
		int dy = (currTile.m_iYPos - parentTile.m_iYPos) / (std::max)(std::abs(currTile.m_iYPos - parentTile.m_iYPos), 1);

		if (dx != 0 && dy != 0) //check diagonally
		{
			bool bFirstAdded = AddNeighbourOnCondition(neighbours, currTile.m_iXPos, currTile.m_iYPos + dy, bPassable, currTile.m_iXPos, currTile.m_iYPos + dy);
			bool bSecAdded = AddNeighbourOnCondition(neighbours, currTile.m_iXPos + dx, currTile.m_iYPos, bPassable, currTile.m_iXPos + dx, currTile.m_iYPos);

			if (bFirstAdded && bSecAdded)
			{
				int nodeId = m_MapManagerPtr->GetGraphNodeByPosition(currTile.m_iXPos + dx, currTile.m_iYPos + dy).GetGridTileInfo().GetGridId();
				neighbours.push_back(nodeId);
			}
		}
		else
		{
			if (dy != 0)
			{
				bool bNeighPassable = m_MapManagerPtr->IsPassable(currTile.m_iXPos, currTile.m_iYPos + dy);

				if (bNeighPassable)
				{
					int nodeId = m_MapManagerPtr->GetGraphNodeByPosition(currTile.m_iXPos, currTile.m_iYPos + dy).GetGridTileInfo().GetGridId();
					neighbours.push_back(nodeId);

					AddNeighbourOnCondition(neighbours, currTile.m_iXPos + 1, currTile.m_iYPos, bPassable, currTile.m_iXPos + 1, currTile.m_iYPos + dy); // on right
					AddNeighbourOnCondition(neighbours, currTile.m_iXPos - 1, currTile.m_iYPos, bPassable, currTile.m_iXPos - 1, currTile.m_iYPos + dy); // on left
				}

				AddNeighbourOnCondition(neighbours, currTile.m_iXPos + 1, currTile.m_iYPos, bPassable, currTile.m_iXPos + 1, currTile.m_iYPos); // on right
				AddNeighbourOnCondition(neighbours, currTile.m_iXPos - 1, currTile.m_iYPos, bPassable, currTile.m_iXPos - 1, currTile.m_iYPos); // on left
			}
			else if (dx != 0) // TODO usunac (dx != 0)
			{
				bool bNeighPassable = m_MapManagerPtr->IsPassable(currTile.m_iXPos + dx, currTile.m_iYPos);

				if (bNeighPassable)
				{
					int nodeId = m_MapManagerPtr->GetGraphNodeByPosition(currTile.m_iXPos + dx, currTile.m_iYPos).GetGridTileInfo().GetGridId();
					neighbours.push_back(nodeId);

					AddNeighbourOnCondition(neighbours, currTile.m_iXPos, currTile.m_iYPos + 1, bPassable, currTile.m_iXPos + dx, currTile.m_iYPos + 1); // up
					AddNeighbourOnCondition(neighbours, currTile.m_iXPos, currTile.m_iYPos - 1, bPassable, currTile.m_iXPos + dx, currTile.m_iYPos - 1); // down
				}

				AddNeighbourOnCondition(neighbours, currTile.m_iXPos, currTile.m_iYPos + 1, bPassable, currTile.m_iXPos, currTile.m_iYPos + 1); // up
				AddNeighbourOnCondition(neighbours, currTile.m_iXPos, currTile.m_iYPos - 1, bPassable, currTile.m_iXPos, currTile.m_iYPos - 1); // down			
			}
		}
	}
}

bool CJPSearch::AddNeighbourOnCondition(std::vector<int> &neighbours, int iXPos, int iYPos, bool bExpectedVal, int iXPosNeigh, int iYPosNeigh)
{
	CGridTile *tile;
	CGridTile *neighbourTile;
	tile = m_MapManagerPtr->GetNodeIfExists(iXPos, iYPos);
	if (tile != nullptr)
	{
		bool bVal = tile->GetGridTypeEnum() != GridTypeImpassable;
		if (bVal == bExpectedVal)
		{
			neighbourTile = m_MapManagerPtr->GetNodeIfExists(iXPosNeigh, iYPosNeigh);
			if (neighbourTile != nullptr)
			{
				neighbours.push_back(neighbourTile->GetGridId());
				return true;
			}
		}
	}
	return false;
}

void CJPSearch::AddNeighbourIfExists(std::vector<int> &neighbours, int iCurrNode)
{
	if (iCurrNode != -1)
	{
		neighbours.push_back(iCurrNode);
	}
}