#include "stdafx.h"
#include "CAstar.h"


CAstar::CAstar(std::string algName)
 : CDijkstra(algName)
{
	m_HEUR_FACTOR = 1.0;
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

template <class T, class A, class B>
class CMyPriorityQueue : public std::priority_queue<T, A, B>
{
public:
	
//	auto& impl() { return c; }

};

int CAstar::CalcClusterIdByTile(CGridTile &tile, int iFirstLvlClusterWidth, int iFirstLvlClusterHeight, int iClustersRowCount)
{
	//TODO to samo jest w CalcNewClusterId
	int width = tile.m_iXPos / iFirstLvlClusterWidth;
	int height = tile.m_iYPos / iFirstLvlClusterHeight;

	return height * iClustersRowCount + width;
}

void CAstar::FindShortestPath(int iStartNode, int iGoalNode, bool bSimpleSearch, CCluster *clusterPtr)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	double maxDistance = (std::numeric_limits<double>::max)();
	int iNodesCount = m_MapManagerPtr->GetMapNodesCount();

	auto& graph = m_MapManagerPtr->GetGraph();
	auto& goalGridTile = graph[iGoalNode].GetGridTileInfo();

	m_shortestPath.clear(); //deallocate memory for shortestpath
	if (!m_bPreallocatedResources)
	{
		int iCnt = m_MapManagerPtr->GetGraph().size();
		m_previous.resize(iCnt);
		m_distSoFar.assign(iCnt, maxDistance);
		m_closedList.assign(iCnt, false);
		m_bPreallocatedResources = true;
	}
	else if(bSimpleSearch) 
	{
		ATLASSERT(clusterPtr != nullptr); // parameter not set
		InitValues(m_distSoFar, clusterPtr, maxDistance);
		InitValues(m_closedList, clusterPtr, false);	
	}
	else
	{
		std::fill_n(m_distSoFar.begin(), iNodesCount, maxDistance);
		std::fill_n(m_closedList.begin(), iNodesCount, false);
	}
	std::chrono::steady_clock::time_point endAllocationTime = std::chrono::steady_clock::now();
	m_timeAllocation = std::chrono::duration_cast<std::chrono::nanoseconds>(endAllocationTime - begin).count();

	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;

	CMyPriorityQueue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;
	CPriorityQueueNode firstNode(iStartNode, 0.0);
	dijQueueNode.push(firstNode);

	long lVisitedNodes = 0;
	long lExpandedNodes = 0;
	long lOpenSetMaxSize = 0;
	long lAlreadyProcessed = 0;

	bool bFoundBetterNeighbour = false;
	std::vector<CPriorityQueueNode>::iterator directNextNodePtr;

	CPriorityQueueNode currentNode;
	int current;
	int currentFVal;
	int cleanCount = 0;
	m_bPathFound = false;
	while (!dijQueueNode.empty() || bFoundBetterNeighbour)
	{
		if (dijQueueNode.size() > lOpenSetMaxSize)
			lOpenSetMaxSize = dijQueueNode.size();

		if (!bFoundBetterNeighbour)
		{
			auto currentNode = dijQueueNode.top();
			current = currentNode.GetNodeId();
			currentFVal = currentNode.GetF();
			dijQueueNode.pop();
		}
		else
		{
			current = directNextNodePtr->GetNodeId();
			currentFVal = directNextNodePtr->GetF();
		}

		if (m_closedList[current] == true) // juz rozpatrzylismy ten wierzcholek
		{
			++lAlreadyProcessed;
			continue;
		}

		m_closedList[current] = true;
		++lExpandedNodes;

		if (current == iGoalNode)
		{
			m_bPathFound = true;
			break;
		}

		bFoundBetterNeighbour = false;
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

				auto &currGridTile = graph[nextNeighbour->GetNodeId()].GetGridTileInfo();

				double newFValue = GetOctileHeuristic(currGridTile.GetGridId(), goalGridTile.GetGridId());

				nextNeighbour->SetF(new_cost + newFValue);

				if (dijQueueNode.size() == 0 || dijQueueNode.top().GetF() >= nextNeighbour->GetF())
				{
					if (bFoundBetterNeighbour)
					{
						dijQueueNode.push(*directNextNodePtr);
					}
					directNextNodePtr = nextNeighbour;
					bFoundBetterNeighbour = true;
				}
				else
				{
					dijQueueNode.push(*nextNeighbour);
				}
			}
			nextNeighbour++;
		}
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

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	double timeElapsedMS = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

	m_Statistics.SetPathLength(m_shortestPathLength);
	m_Statistics.SetSearchTimeChrono(timeElapsedMS);
	m_Statistics.SetNodesExpanded(lExpandedNodes);
	m_Statistics.SetNodesVisited(lVisitedNodes);
	m_Statistics.SetOpenSetMaxSize(lOpenSetMaxSize);
}

double CAstar::GetOctileHeuristic(int iStartNode, int iGoalNode)
{
	auto& currGridTile = m_MapManagerPtr->GetGraph()[iStartNode].GetGridTileInfo();
	auto& goalGridTile = m_MapManagerPtr->GetGraph()[iGoalNode].GetGridTileInfo();

	int deltaX = std::abs(currGridTile.m_iXPos - goalGridTile.m_iXPos);
	int deltaY = std::abs(currGridTile.m_iYPos - goalGridTile.m_iYPos);

	double newFValue = m_HEUR_FACTOR * ( max(deltaX, deltaY) + DIAG_HEUR_VAL * min(deltaX, deltaY) );
	//double newFValue = deltaX + deltaY;
	//double newFValue = std::sqrt(deltaX * deltaX + deltaY * deltaY);

	return newFValue;
}

//for getting correct distance value in JPS, when not admissible octile heuristic is used
double CAstar::GetOctileAdmissible(int iStartNode, int iGoalNode)
{
	auto& currGridTile = m_MapManagerPtr->GetGraph()[iStartNode].GetGridTileInfo();
	auto& goalGridTile = m_MapManagerPtr->GetGraph()[iGoalNode].GetGridTileInfo();

	int deltaX = std::abs(currGridTile.m_iXPos - goalGridTile.m_iXPos);
	int deltaY = std::abs(currGridTile.m_iYPos - goalGridTile.m_iYPos);

	double newFValue = ( max(deltaX, deltaY) + DIAG_HEUR_VAL * min(deltaX, deltaY) );
	return newFValue;
}

bool CAstar::NeighbourIsInClusterArea(/*CPriorityQueueEdges &neighbour*/CGridTile &tile, CCluster &cluster)
{
	//auto &tile = neighbour.GetGridTileInfo();
	return (tile.m_iXPos >= cluster.GetXPos() && tile.m_iXPos < cluster.GetXPos() + cluster.GetClusterWidth()
		&& tile.m_iYPos >= cluster.GetYPos() && tile.m_iYPos < cluster.GetYPos() + cluster.GetClusterHeight());
}

void CAstar::FindShortestPathInCluster(int iStartNode, int iGoalNode, CCluster &cluster)
{
	double maxDistance = (std::numeric_limits<double>::max)();
	int iNodesCount = m_MapManagerPtr->GetMapNodesCount();
	m_shortestPath.clear(); //deallocate memory for shortestpath
	if (!m_bPreallocatedResources)
	{
		int iCnt = m_MapManagerPtr->GetGraph().size();
		m_previous.resize(iCnt);
		//m_distSoFar.resize(iCnt);
		m_distSoFar.assign(iCnt, maxDistance);
		m_closedList.assign(iCnt, false);
		m_bPreallocatedResources = true;
	}
	else
	{
		//m_distSoFar.assign(iNodesCount, maxDistance);
		//m_closedList.assign(iNodesCount, false);
		InitValues(m_distSoFar, &cluster, maxDistance);
		InitValues(m_closedList, &cluster, false);
	}
	/*if (!m_bPreallocatedHash)
	{
		int iCnt = m_MapManagerPtr->GetGraph().size();
		m_previous.resize(iCnt);
		m_closedListHash.reserve(iCnt);
		m_distSoFarHash.reserve(iCnt);

		InitHashMapValues<bool>(m_closedListHash, cluster, false);
		InitHashMapValues<double>(m_distSoFarHash, cluster, maxDistance);
		m_bPreallocatedHash = true;
	}
	else
	{
		InitHashMapValues<bool>(m_closedListHash, cluster, false);
		InitHashMapValues<double>(m_distSoFarHash, cluster, maxDistance);
	}*/

	auto& graph = m_MapManagerPtr->GetGraph();
	auto& goalGridTile = graph[iGoalNode].GetGridTileInfo();

	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;

	CMyPriorityQueue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;
	CPriorityQueueNode firstNode(iStartNode, 0.0);
	//firstNode.SetF(newFValue);
	dijQueueNode.push(firstNode);

	long lVisitedNodes = 0;
	long lExpandedNodes = 0;
	long lOpenSetMaxSize = 0;
	long lAlreadyProcessed = 0;

	bool bFoundBetterNeighbour = false;
	std::vector<CPriorityQueueNode>::iterator directNextNodePtr;

	CPriorityQueueNode currentNode;
	int current;
	int currentFVal;
	int cleanCount = 0;
	m_bPathFound = false;
	while (!dijQueueNode.empty() || bFoundBetterNeighbour)
	{
		if (dijQueueNode.size() > lOpenSetMaxSize)
			lOpenSetMaxSize = dijQueueNode.size();

		if (!bFoundBetterNeighbour)
		{
			auto currentNode = dijQueueNode.top();
			current = currentNode.GetNodeId();
			currentFVal = currentNode.GetF();
			dijQueueNode.pop();
		}
		else
		{
			current = directNextNodePtr->GetNodeId();
			currentFVal = directNextNodePtr->GetF();
		}

		if (m_closedList[current] == true) // juz rozpatrzylismy ten wierzcholek
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

		bFoundBetterNeighbour = false;
		//auto nextNeighbour = graph[current].GetNeighbours().begin();
		//while (nextNeighbour != graph[current].GetIterEnd() || NeighbourIsInClusterArea(graph[nextNeighbour->GetNodeId()].GetGridTileInfo(), cluster))
		for (auto nextNeighbour = graph[current].GetNeighbours().begin();
			 nextNeighbour != graph[current].GetIterEnd(); nextNeighbour++)
		{
			if (NeighbourIsInClusterArea(graph[nextNeighbour->GetNodeId()].GetGridTileInfo(), cluster))
			{
				double new_cost = m_distSoFar[current] + nextNeighbour->GetWeight();
				if (m_closedList[nextNeighbour->GetNodeId()] != true
					&& m_distSoFar[nextNeighbour->GetNodeId()] > new_cost)
				{
					++lVisitedNodes;
					m_distSoFar[nextNeighbour->GetNodeId()] = new_cost;
					m_previous[nextNeighbour->GetNodeId()] = current;

					auto &currGridTile = graph[nextNeighbour->GetNodeId()].GetGridTileInfo();

					int deltaX = std::abs(currGridTile.m_iXPos - goalGridTile.m_iXPos);
					int deltaY = std::abs(currGridTile.m_iYPos - goalGridTile.m_iYPos);
                    double newFValue = max(deltaX, deltaY) + DIAG_HEUR_VAL * min(deltaX, deltaY);

					nextNeighbour->SetF(new_cost + newFValue);

					if (dijQueueNode.size() == 0 || dijQueueNode.top().GetF() >= nextNeighbour->GetF())
					{
						if (bFoundBetterNeighbour)
						{
							dijQueueNode.push(*directNextNodePtr);
						}
						directNextNodePtr = nextNeighbour;
						bFoundBetterNeighbour = true;
					}
					else
					{
						dijQueueNode.push(*nextNeighbour);
					}
				}
			}
		}

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
	}	

	//m_Statistics.SetPathLength(m_shortestPathLength);
	//m_Statistics.SetSearchTimeChrono(timeElapsedMS);
	//m_Statistics.SetSearchTimeClock(timeElapsedClock);
	m_Statistics.SetNodesExpanded(lExpandedNodes);
	m_Statistics.SetNodesVisited(lVisitedNodes);
	m_Statistics.SetOpenSetMaxSize(lOpenSetMaxSize);
	//std::vector<int>().swap(m_shortestPath); //deallocate memory for shortestpath
}

//iStartNode i iGoalNode sa Id'ami z lowLvlGrafu, trzeba je najpierw znalezc w abstrGraph
void CAstar::FindShortestPathInClusterAbstract(int iStartNode, int iGoalNode, CCluster &cluster, std::vector<CHierarchicalEdge> &abstrGraph,
			int iStartOwnPos, int iGoalOwnPos)
{
	double maxDistance = (std::numeric_limits<double>::max)();
	int iNodesCount = abstrGraph.size();

	//std::vector<int>().swap(m_shortestPath); //deallocate memory for shortestpath
	//m_previous.resize(iNodesCount);
	//m_distSoFar.resize(iNodesCount);
	//m_distSoFar.assign(iNodesCount, maxDistance);
	//m_closedList.assign(iNodesCount, false);

	m_shortestPath.clear(); //deallocate memory for shortestpath
	if (!m_bPreallocatedResources)
	{
		int iCnt = m_MapManagerPtr->GetGraph().size();
		m_previous.resize(iCnt);
		//m_distSoFar.resize(iCnt);
		m_distSoFar.assign(iCnt, maxDistance);
		m_closedList.assign(iCnt, false);
		m_bPreallocatedResources = true;
	}
	else
	{
		//TODO
		//m_distSoFar.assign(iNodesCount, maxDistance);
		//m_closedList.assign(iNodesCount, false);
		//InitValues(m_distSoFar, &cluster, maxDistance);
		//InitValues(m_closedList, &cluster, false);
		std::fill_n(m_distSoFar.begin(), iNodesCount, maxDistance);
		std::fill_n(m_closedList.begin(), iNodesCount, false);
	}

	
	auto& lowLvlgraph = m_MapManagerPtr->GetGraph();
	auto& goalGridTile = lowLvlgraph[iGoalNode].GetGridTileInfo();

	/*auto startNodeAbstrPos = std::find_if(abstrGraph.begin(), abstrGraph.end(), 
		[iStartNode](CHierarchicalEdge &edge) -> bool { return edge.GetId() == iStartNode; });
	auto goalNodeAbstrPos = std::find_if(abstrGraph.begin(), abstrGraph.end(),
		[iGoalNode](CHierarchicalEdge &edge) -> bool { return edge.GetId() == iGoalNode; });*/

	iGoalNode = iGoalOwnPos;//goalNodeAbstrPos->GetOwnId();
	iStartNode = iStartOwnPos;//startNodeAbstrPos->GetOwnId();

	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;

	CMyPriorityQueue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;
	CPriorityQueueNode firstNode(iStartNode, 0.0);
	dijQueueNode.push(firstNode);

	long lVisitedNodes = 0;
	long lExpandedNodes = 0;
	long lOpenSetMaxSize = 0;
	long lAlreadyProcessed = 0;

	CPriorityQueueNode currentNode;
	int current;
	//int currentFVal;
	int cleanCount = 0;
	m_bPathFound = false;
	while (!dijQueueNode.empty())
	{
		if (dijQueueNode.size() > lOpenSetMaxSize)
			lOpenSetMaxSize = dijQueueNode.size();

		auto currentNode = dijQueueNode.top();
		current = currentNode.GetNodeId();
		//currentFVal = currentNode.GetF();
		dijQueueNode.pop();

		if (m_closedList[current] == true) // juz rozpatrzylismy ten wierzcholek
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

		//auto nextNeighbour = abstrGraph[current].GetNeighbours().begin();
		//while (nextNeighbour != abstrGraph[current].GetIterEnd() 
		//	|| NeighbourIsInClusterArea(lowLvlgraph[nextNeighbour->GetZeroLevelNodeId()].GetGridTileInfo(), cluster))
		for (auto nextNeighbour = abstrGraph[current].GetNeighbours().begin();
			nextNeighbour != abstrGraph[current].GetIterEnd(); nextNeighbour++)
		{
			if (NeighbourIsInClusterArea(lowLvlgraph[nextNeighbour->GetZeroLevelNodeId()].GetGridTileInfo(), cluster))
			{
				double new_cost = m_distSoFar[current] + nextNeighbour->GetWeight();
				if (m_closedList[nextNeighbour->GetOwnId()] != true
					&& m_distSoFar[nextNeighbour->GetOwnId()] > new_cost)
				{
					++lVisitedNodes;
					m_distSoFar[nextNeighbour->GetOwnId()] = new_cost;
					m_previous[nextNeighbour->GetOwnId()] = current;

					auto &currGridTile = lowLvlgraph[nextNeighbour->GetZeroLevelNodeId()].GetGridTileInfo();

					int deltaX = std::abs(currGridTile.m_iXPos - goalGridTile.m_iXPos);
					int deltaY = std::abs(currGridTile.m_iYPos - goalGridTile.m_iYPos);
                    double newFValue = max(deltaX, deltaY) + DIAG_HEUR_VAL * min(deltaX, deltaY);

					CPriorityQueueNode node(nextNeighbour->GetOwnId(), nextNeighbour->GetWeight());
					node.SetF(new_cost + newFValue);
					//nextNeighbour->SetF(new_cost + newFValue);

					dijQueueNode.push(node);
				}
			}
		}

	}
	if (m_bPathFound)
	{
		m_shortestPathLength = m_distSoFar[iGoalNode];
		/*int iNodeId = iGoalNode;
		while (m_previous[iNodeId] != -1)
		{
			m_shortestPath.push_back(iNodeId);
			iNodeId = m_previous[iNodeId];
		}*/
	}

	//m_Statistics.SetPathLength(m_shortestPathLength);
	//m_Statistics.SetSearchTimeChrono(timeElapsedMS);
	//m_Statistics.SetSearchTimeClock(timeElapsedClock);
	m_Statistics.SetNodesExpanded(lExpandedNodes);
	m_Statistics.SetNodesVisited(lVisitedNodes);
	m_Statistics.SetOpenSetMaxSize(lOpenSetMaxSize);
	//std::vector<int>().swap(m_shortestPath); //deallocate memory for shortestpath
}

//iStartNode i iGoalNode sa Id'ami z lowLvlGrafu
void CAstar::FindShortestPathAbstract(int iStartNode, int iGoalNode, std::vector<CHierarchicalEdge> &abstrGraph, int iStartOwnPos, int iGoalOwnPos)
{
	double maxDistance = (std::numeric_limits<double>::max)();
	int iNodesCount = abstrGraph.size();
	m_shortestPath.clear(); //deallocate memory for shortestpath
	if (!m_bPreallocatedResources)
	{
		int iCnt = m_MapManagerPtr->GetGraph().size();
		m_previous.resize(iCnt);
		//m_distSoFar.resize(iCnt);
		m_distSoFar.assign(iCnt, maxDistance);
		m_closedList.assign(iCnt, false);
		m_bPreallocatedResources = true;
	}
	else
	{
		//m_distSoFar.assign(iNodesCount, maxDistance);
		//m_closedList.assign(iNodesCount, false);
		std::fill_n(m_distSoFar.begin(), iNodesCount, maxDistance);
		std::fill_n(m_closedList.begin(), iNodesCount, false);
	}

	auto& lowLvlgraph = m_MapManagerPtr->GetGraph();
	auto& goalGridTile = lowLvlgraph[iGoalNode].GetGridTileInfo();

	/*auto startNodeAbstrPos = std::find_if(abstrGraph.begin(), abstrGraph.end(),
		[iStartNode](CHierarchicalEdge &edge) -> bool { return edge.GetId() == iStartNode; });
	auto goalNodeAbstrPos = std::find_if(abstrGraph.begin(), abstrGraph.end(),
		[iGoalNode](CHierarchicalEdge &edge) -> bool { return edge.GetId() == iGoalNode; });*/

	iGoalNode = iGoalOwnPos; // goalNodeAbstrPos->GetOwnId();
	iStartNode = iStartOwnPos; //startNodeAbstrPos->GetOwnId();

	m_previous[iStartNode] = -1;
	m_distSoFar[iStartNode] = 0.0f;

	CMyPriorityQueue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> dijQueueNode;
	CPriorityQueueNode firstNode(iStartNode, 0.0);
	dijQueueNode.push(firstNode);

	long lVisitedNodes = 0;
	long lExpandedNodes = 0;
	long lOpenSetMaxSize = 0;
	long lAlreadyProcessed = 0;

	CPriorityQueueNode currentNode;
	int current;
	//int currentFVal;
	int cleanCount = 0;
	m_bPathFound = false;
	while (!dijQueueNode.empty())
	{
		if (dijQueueNode.size() > lOpenSetMaxSize)
			lOpenSetMaxSize = dijQueueNode.size();

		auto currentNode = dijQueueNode.top();
		current = currentNode.GetNodeId();
		//currentFVal = currentNode.GetF();
		dijQueueNode.pop();

		if (m_closedList[current] == true) // juz rozpatrzylismy ten wierzcholek
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

		//auto nextNeighbour = abstrGraph[current].GetNeighbours().begin();
		//while (nextNeighbour != abstrGraph[current].GetIterEnd() 
		//	|| NeighbourIsInClusterArea(lowLvlgraph[nextNeighbour->GetZeroLevelNodeId()].GetGridTileInfo(), cluster))
		for (auto nextNeighbour = abstrGraph[current].GetNeighbours().begin();
			nextNeighbour != abstrGraph[current].GetIterEnd(); nextNeighbour++)
		{
			double new_cost = m_distSoFar[current] + nextNeighbour->GetWeight();
			if (m_closedList[nextNeighbour->GetOwnId()] != true
				&& m_distSoFar[nextNeighbour->GetOwnId()] > new_cost)
			{
				++lVisitedNodes;
				m_distSoFar[nextNeighbour->GetOwnId()] = new_cost;
				m_previous[nextNeighbour->GetOwnId()] = current;

				auto &currGridTile = lowLvlgraph[nextNeighbour->GetZeroLevelNodeId()].GetGridTileInfo();

				int deltaX = std::abs(currGridTile.m_iXPos - goalGridTile.m_iXPos);
				int deltaY = std::abs(currGridTile.m_iYPos - goalGridTile.m_iYPos);
                double newFValue = max(deltaX, deltaY) + DIAG_HEUR_VAL * min(deltaX, deltaY);

				CPriorityQueueNode node(nextNeighbour->GetOwnId(), nextNeighbour->GetWeight());
				node.SetF(new_cost + newFValue);
				//nextNeighbour->SetF(new_cost + newFValue);

				dijQueueNode.push(node);
			}
		}

	}
	if (m_bPathFound)
	{
		m_shortestPathLength = m_distSoFar[iGoalNode];
		int iNodeId = iGoalNode;
		while (m_previous[iNodeId] != -1)
		{
			m_shortestPath.push_back(abstrGraph[iNodeId].GetId());
			iNodeId = m_previous[iNodeId];
		}
		m_shortestPath.push_back(abstrGraph[iNodeId].GetId());
	}

	//std::vector<int>().swap(m_shortestPath); //deallocate memory for shortestpath
	//m_Statistics.SetPathLength(m_shortestPathLength);
	//m_Statistics.SetSearchTimeChrono(timeElapsedMS);
	//m_Statistics.SetSearchTimeClock(timeElapsedClock);
	m_Statistics.SetNodesExpanded(lExpandedNodes);
	m_Statistics.SetNodesVisited(lVisitedNodes);
	m_Statistics.SetOpenSetMaxSize(lOpenSetMaxSize);
}
