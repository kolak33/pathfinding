#include "stdafx.h"
#include "CHPAAbstrLevel.h"
#include "CAstar.h"

void CHPAAbstrLevel::BuildClusters(int iClusterWidth, int iClusterHeight)
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();

	int iCurrClusterHeight = iClusterHeight, iCurrClusterWidth = iClusterWidth;

	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight, ++m_iClustersColCount)
		iHeight + 3 / 2 * iClusterHeight >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = iClusterHeight;

	for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, ++m_iClustersRowCount)
		iWidth + 3 * iClusterWidth / 2 >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = iClusterWidth;

	int iLeftClusterId = 0;
	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight/*, ++iLeftClusterId*/)
	{
		//prevent creating too small clusters, if next clusted would end up being smaller than half of wanted size, glue two clusters together
		iHeight + 3 * iClusterHeight / 2 >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = iClusterHeight;
		for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, ++iLeftClusterId)
		{
			// again, prevention
			iWidth + 3 * iClusterWidth / 2 >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = iClusterWidth;
			 
	//		if (iWidth + iClusterWidth >= iMapWidth) 
	//			break; // out of range, no cluster on right side

			CCluster cluster(iCurrClusterWidth, iCurrClusterHeight);
			cluster.SetXPos(iWidth); // most left-up point in cluster
			cluster.SetYpos(iHeight);
			cluster.SetXCenter(iWidth + iCurrClusterHeight / 2); // for A* heuristic
			cluster.SetYCenter(iHeight + iCurrClusterWidth / 2);
			cluster.SetId(iLeftClusterId);
			m_Clusters.push_back(cluster);
		}
	}
}

void CHPAAbstrLevel::BuildClustersNextLevel(CHPAAbstrLevel *lowerAbstrLevelPtr)
{
	auto& lowerClusters = lowerAbstrLevelPtr->GetClusters();
	auto& lowerGraph = lowerAbstrLevelPtr->GetGraph();
	int lowerLvlClustersColCount = lowerAbstrLevelPtr->GetClustersColCount();
	int lowerLvlClustersRowCount = lowerAbstrLevelPtr->GetClustersRowCount();

	//first create all new clusters
	int oldClusterId = 0;
	int newClusterId = 0;
	int newWidth, newHeight;

	m_iClustersRowCount = std::ceil((double)lowerLvlClustersRowCount / (double)m_iClustersLen);
	m_iClustersColCount = std::ceil((double)lowerLvlClustersColCount / (double)m_iClustersLen);

	for (int iColCluster = 0; iColCluster < lowerLvlClustersColCount; iColCluster += m_iClustersLen)
	{
		for(int iRowCluster = 0; iRowCluster < lowerLvlClustersRowCount; iRowCluster += m_iClustersLen, newClusterId++)
		{
			int rowLength = iRowCluster + m_iClustersLen < lowerLvlClustersRowCount ? m_iClustersLen : lowerLvlClustersRowCount - iRowCluster;
			if (iRowCluster + m_iClustersLen < lowerLvlClustersRowCount) // full length new cluster
				newWidth = lowerClusters[oldClusterId].GetClusterWidth() * rowLength;
			else //not full length cluster
			{
				newWidth = rowLength == 1 ? lowerClusters[oldClusterId].GetClusterWidth()
					: (rowLength - 1) * lowerClusters[oldClusterId].GetClusterWidth() + lowerClusters[oldClusterId + (rowLength - 1)].GetClusterWidth();
			}

			int colLength = iColCluster + m_iClustersLen < lowerLvlClustersColCount ? m_iClustersLen : lowerLvlClustersColCount - iColCluster;
			if (iColCluster + m_iClustersLen < lowerLvlClustersRowCount) // full length new cluster
				newHeight = lowerClusters[oldClusterId].GetClusterHeight() * colLength;
			else //not full length cluster
			{
				newHeight = colLength == 1 ? lowerClusters[oldClusterId].GetClusterHeight()
					: (colLength - 1) * lowerClusters[oldClusterId].GetClusterHeight() + lowerClusters[oldClusterId + lowerLvlClustersRowCount].GetClusterHeight();
			}
		
			CCluster cluster(newWidth, newHeight);
			cluster.SetId(newClusterId);
			cluster.SetXPos(lowerClusters[oldClusterId].GetXPos());
			cluster.SetYpos(lowerClusters[oldClusterId].GetYPos());
			m_Clusters.push_back(cluster);

			oldClusterId += rowLength;
		}
		oldClusterId += (m_iClustersLen - 1) * lowerLvlClustersRowCount;
	}
}

int CHPAAbstrLevel::CalcNewClusterId(CGridTile &tile, int iFirstLvlClusterWidth, int iFirstLvlClusterHeight)
{
	int width = tile.m_iXPos / (m_iClustersLen * (m_iCurrLevel - 1) * iFirstLvlClusterWidth);
	int height = tile.m_iYPos / (m_iClustersLen * (m_iCurrLevel - 1) * iFirstLvlClusterHeight);

	return (height * m_iClustersRowCount + width);
}

void CHPAAbstrLevel::BuildHorizontalEntrancesNextLevel(CHPAAbstrLevel *lowerAbstrLevelPtr, int iFirstLvlClusterWidth, int iFirstLvlClusterHeight)
{
	auto& lowerClusters = lowerAbstrLevelPtr->GetClusters();
	auto& lowerGraph = lowerAbstrLevelPtr->GetGraph();
	int lowerLvlClustersColCount = lowerAbstrLevelPtr->GetClustersColCount();
	int lowerLvlClustersRowCount = lowerAbstrLevelPtr->GetClustersRowCount();
	int clWidth = iFirstLvlClusterWidth * m_iClustersLen;
	int clHeight = iFirstLvlClusterHeight * m_iClustersLen;

	std::vector<bool> alreadyCreatedEdgeNode;
	alreadyCreatedEdgeNode.assign(lowerGraph.size(), false);

	for (auto graphNode = lowerGraph.begin(); graphNode != lowerGraph.end(); graphNode++)
	{
		auto &tile = m_GraphLowLevelPtr->GetGraph()[graphNode->GetId()].GetGridTileInfo();
		if (tile.m_iXPos != 0 && tile.m_iXPos % clWidth == 0
			|| tile.m_iYPos != 0 && tile.m_iYPos % clHeight == 0) // is vertical border's right side or horizontal border's lower side
		{
			for (auto &neighbours : graphNode->GetNeighbours())
			{
				if (neighbours.GetIsInterEdge()) // add edges if they aren't already created and neighbour nodes
				{
					if (neighbours.GetZeroLevelNodeId() == 8238 && m_iCurrLevel == 2)
						int a = 5;
					int iNodeClusterId = CalcNewClusterId(m_GraphLowLevelPtr->GetGraph()[neighbours.GetZeroLevelNodeId()].GetGridTileInfo(), iFirstLvlClusterWidth, iFirstLvlClusterHeight);
					auto nodeLowerGraphPos = std::find_if(lowerGraph.begin(), lowerGraph.end(), 
						[&neighbours](CHierarchicalEdge &edge) -> bool { return edge.GetId() == neighbours.GetZeroLevelNodeId(); });
					if (nodeLowerGraphPos == lowerGraph.end())
						int a = 5;

					//CHierarchicalNode node(iNodeClusterId, INTER_EDGE_WEIGHT, std::distance(lowerGraph.begin(), graphNode), true, graphNode->GetId());
					CHierarchicalNode node(iNodeClusterId, INTER_EDGE_WEIGHT, std::distance(lowerGraph.begin(), nodeLowerGraphPos), true, neighbours.GetZeroLevelNodeId());
					int iEdgeClusterId = CalcNewClusterId(tile, iFirstLvlClusterWidth, iFirstLvlClusterHeight);
					
					if (iNodeClusterId == iEdgeClusterId)
						continue;

					if (!alreadyCreatedEdgeNode[std::distance(lowerGraph.begin(), graphNode)])
					{
						CHierarchicalEdge edge(tile.GetGridId());					
						edge.SetClusterId(iEdgeClusterId);
						edge.AddNeighbour(node);
						m_Graph.push_back(edge);
						m_Clusters[iEdgeClusterId].GetEntrances().push_back(tile.GetGridId());
						alreadyCreatedEdgeNode[std::distance(lowerGraph.begin(), graphNode)] = true;
					}

					//edgeNode in adjacent cluster
					std::swap(iNodeClusterId, iEdgeClusterId);
					CHierarchicalNode node2(iNodeClusterId, INTER_EDGE_WEIGHT, std::distance(lowerGraph.begin(), graphNode), true, tile.GetGridId());
					if (!alreadyCreatedEdgeNode[std::distance(lowerGraph.begin(), nodeLowerGraphPos)])
					{
						CHierarchicalEdge edge(neighbours.GetZeroLevelNodeId());
						edge.SetClusterId(iEdgeClusterId);
						edge.AddNeighbour(node2);
						m_Graph.push_back(edge);
						m_Clusters[iEdgeClusterId].GetEntrances().push_back(neighbours.GetZeroLevelNodeId());
						alreadyCreatedEdgeNode[std::distance(lowerGraph.begin(), nodeLowerGraphPos)] = true;
					}
				}
			}
		}	
	}
}

void CHPAAbstrLevel::BuildHorizontalEntrances(int iClusterWidth, int iClusterHeight, int iMaxSingleEntranceLen)
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();
	int iCurrClusterHeight, iCurrClusterWidth;
	int iLeftClusterXPos, iRightClusterXPos;
	int iLeftClusterId = 0;
	// build vertical entrances
	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight/*, ++iLeftClusterId*/)
	{
		//prevent creating too small clusters, if next clusted would end up being smaller than half of wanted size, glue two clusters together
		iHeight + 3 * iClusterHeight / 2 >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = iClusterHeight;
		for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, ++iLeftClusterId)
		{
			iWidth + 3 * iClusterWidth / 2 >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = iClusterWidth;

			iRightClusterXPos = iWidth + iClusterWidth;
			iLeftClusterXPos = iRightClusterXPos - 1;
			if (iRightClusterXPos >= iMapWidth) // out of range, no cluster on right side
				break;

			int iEntranceYpos = iHeight;
			int iEntranceLen = 0;
			while (iEntranceYpos < iHeight + iCurrClusterHeight)
			{
				iEntranceLen = 0;
				bool bBreaked = false;
				while (iEntranceYpos < iHeight + iCurrClusterHeight)
				{
					auto& leftNode = m_GraphLowLevelPtr->GetGraphNodeByPosition(iLeftClusterXPos, iEntranceYpos);
					auto& rightNode = m_GraphLowLevelPtr->GetGraphNodeByPosition(iRightClusterXPos, iEntranceYpos);
					++iEntranceYpos;
					if (leftNode.GetGridTileInfo().GetGridTypeEnum() == GridTypePassable
						&& rightNode.GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
						++iEntranceLen;
					else
					{
						bBreaked = true;
						break;
					}
				}

				int iEntranceYposLocal = iEntranceYpos;
				if (bBreaked)
					--iEntranceYposLocal;
				if (iEntranceLen >= iMaxSingleEntranceLen) // make two entrances on sides
				{	
					// first entrance is in position: [ iLeftClusterXPos,iEntranceYpos - 1]
					// second entrance is iEntranceLen - 1 position earlier
					auto& leftNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iLeftClusterXPos, iEntranceYposLocal - 1);
					auto& leftNodeSecondEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iLeftClusterXPos, iEntranceYposLocal - 1 - (iEntranceLen - 1));	
					auto& rightNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iRightClusterXPos, iEntranceYposLocal - 1);
					auto& rightNodeSecondEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iRightClusterXPos, iEntranceYposLocal - 1 - (iEntranceLen - 1));
					ConnectNodes(leftNodeFirstEntrance, iLeftClusterId, rightNodeFirstEntrance, iLeftClusterId + 1);
					ConnectNodes(leftNodeSecondEntrance, iLeftClusterId, rightNodeSecondEntrance, iLeftClusterId + 1);

					//AddEntranceToClusterAndGraph(leftNodeFirstEntrance, iLeftClusterId);
					//AddEntranceToClusterAndGraph(leftNodeSecondEntrance, iLeftClusterId);
					
					// now the same for cluster on right side
					
					//AddEntranceToClusterAndGraph(rightNodeFirstEntrance, iLeftClusterId + 1);
					//AddEntranceToClusterAndGraph(rightNodeSecondEntrance, iLeftClusterId + 1);
				}
				else if(iEntranceLen > 0)// only one entrance in the middle
				{
					auto& leftNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iLeftClusterXPos, iEntranceYposLocal - 1 - iEntranceLen / 2);
					//AddEntranceToClusterAndGraph(leftNodeFirstEntrance, iLeftClusterId);
					auto& rightNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iRightClusterXPos, iEntranceYposLocal - 1 - iEntranceLen / 2);
					ConnectNodes(leftNodeFirstEntrance, iLeftClusterId, rightNodeFirstEntrance, iLeftClusterId + 1);
					//AddEntranceToClusterAndGraph(rightNodeFirstEntrance, iLeftClusterId + 1);
				}
			}		
		}
	}
}

void CHPAAbstrLevel::BuildVerticalEntrances(int iClusterWidth, int iClusterHeight, int iMaxSingleEntranceLen)
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();
	int iCurrClusterHeight, iCurrClusterWidth;
	int iUpperClusterYPos, iLowerClusterYPos;
	int iUpperClusterId = 0;
	// build vertical entrances
	for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, iUpperClusterId -= (m_iClustersColCount-1) * m_iClustersRowCount - 1)
	{
		//prevent creating too small clusters, if next clusted would end up being smaller than half of wanted size, glue two clusters together
		iWidth + 3 * iClusterWidth / 2 >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = iClusterWidth;
		for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight, iUpperClusterId += m_iClustersRowCount)
		{		
			iHeight + 3 * iClusterHeight / 2 >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = iClusterHeight;
			iLowerClusterYPos = iHeight + iClusterHeight;
			iUpperClusterYPos = iLowerClusterYPos - 1;
			if (iLowerClusterYPos >= iMapHeight) // out of range, no lower cluster
				break;

			int iEntranceXpos = iWidth;
			int iEntranceLen = 0;
			while (iEntranceXpos < iWidth + iCurrClusterWidth)
			{
				iEntranceLen = 0;
				bool bBreaked = false;
				while (iEntranceXpos < iWidth + iCurrClusterWidth)
				{
					auto& upperNode = m_GraphLowLevelPtr->GetGraphNodeByPosition(iEntranceXpos, iUpperClusterYPos);
					auto& lowerNode = m_GraphLowLevelPtr->GetGraphNodeByPosition(iEntranceXpos, iLowerClusterYPos);
					++iEntranceXpos;
					if (upperNode.GetGridTileInfo().GetGridTypeEnum() == GridTypePassable
						&& lowerNode.GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
						++iEntranceLen;
					else
					{
						bBreaked = true;
						break;
					}
					if (upperNode.GetGridTileInfo().GetGridId() == 1988)
						int a = 5;
					if(lowerNode.GetGridTileInfo().GetGridId() == 1988)
						int a = 5;
				}

				int iEntranceXposLocal = iEntranceXpos;
				if (bBreaked)
					--iEntranceXposLocal;
				if (iEntranceLen >= iMaxSingleEntranceLen) // make two entrances on sides
				{
					auto& upperNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iEntranceXposLocal - 1, iUpperClusterYPos);
					auto& upperNodeSecondEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iEntranceXposLocal - 1 - (iEntranceLen - 1), iUpperClusterYPos);
					auto& lowerNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iEntranceXposLocal - 1, iLowerClusterYPos);
					auto& lowerNodeSecondEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iEntranceXposLocal - 1 - (iEntranceLen - 1), iLowerClusterYPos);
					ConnectNodes(upperNodeFirstEntrance, iUpperClusterId, lowerNodeFirstEntrance, iUpperClusterId + m_iClustersRowCount);
					ConnectNodes(upperNodeSecondEntrance, iUpperClusterId, lowerNodeSecondEntrance, iUpperClusterId + m_iClustersRowCount);

					//AddEntranceToClusterAndGraph(upperNodeFirstEntrance, iUpperClusterId);
					//AddEntranceToClusterAndGraph(upperNodeSecondEntrance, iUpperClusterId);					
					//AddEntranceToClusterAndGraph(lowerNodeFirstEntrance, iUpperClusterId + m_iClustersRowCount);
					//AddEntranceToClusterAndGraph(lowerNodeSecondEntrance, iUpperClusterId + m_iClustersRowCount);
				}
				else if (iEntranceLen > 0)// only one entrance in the middle
				{
					auto& upperNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iEntranceXposLocal - 1 - iEntranceLen / 2, iUpperClusterYPos);
					auto& lowerNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iEntranceXposLocal - 1 - iEntranceLen / 2, iLowerClusterYPos);
					ConnectNodes(upperNodeFirstEntrance, iUpperClusterId, lowerNodeFirstEntrance, iUpperClusterId + m_iClustersRowCount);
				}
			}
		}
	}
}

void CHPAAbstrLevel::DrawEntrances()
{
	CColor color(1.0f, 0.0f, 0.0f); //RED
	if (m_iCurrLevel == 2)
		color.B = color.R = color.G = 0.0f;

	for (CCluster &cluster : m_Clusters)
		for (int iEntranceId : cluster.GetEntrances())
		{
			if (m_GraphLowLevelPtr->GetGraph()[iEntranceId].GetGridTileInfo().GetGridId() == 1988)
				int a = 2;
			ATLASSERT(m_GraphLowLevelPtr->GetGraph()[iEntranceId].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable);
			m_GraphLowLevelPtr->ColorGridTile(iEntranceId, color);
		}
}

void CHPAAbstrLevel::DrawClusterBorders()
{
	CColor color(0.5f, 0.5f, 0.5f);
	for (CCluster &cluster : m_Clusters)
	{
		m_GraphLowLevelPtr->DrawClusterBorder(cluster, color);
	}
}

int CHPAAbstrLevel::CalcPosInGraph(CPriorityQueueEdges &firstE, int iFirstClId)
{
	return std::distance(m_Graph.begin(),
		std::find_if(m_Graph.begin(), m_Graph.end(), [&firstE](CHierarchicalEdge &edge) -> bool { return edge.GetId() == firstE.GetGridTileInfo().GetGridId(); }));
}

void CHPAAbstrLevel::ConnectNodes(CPriorityQueueEdges &firstE, int iFirstClId, CPriorityQueueEdges &secondE, int iSecondClId)
{
	int firstEPosInGraph, secondEPosInGraph;
	firstEPosInGraph = AddEntranceToClusterAndGraph(firstE, iFirstClId) ? m_Graph.size() - 1 // if added at the end, otherwise search for it
		: CalcPosInGraph(firstE, iFirstClId);

	secondEPosInGraph = AddEntranceToClusterAndGraph(secondE, iSecondClId) ? m_Graph.size() - 1
		: CalcPosInGraph(secondE, iSecondClId);

	CHierarchicalNode firstNodeNeighbour(iSecondClId, INTER_EDGE_WEIGHT, secondE.GetGridTileInfo().GetGridId(), true, secondE.GetGridTileInfo().GetGridId());
	CHierarchicalNode secondNodeNeighbour(iFirstClId, INTER_EDGE_WEIGHT, firstE.GetGridTileInfo().GetGridId(), true, firstE.GetGridTileInfo().GetGridId());

	m_Graph[firstEPosInGraph].AddNeighbour(firstNodeNeighbour);
	m_Graph[secondEPosInGraph].AddNeighbour(secondNodeNeighbour);
}

// return true when added at the end of m_Graph
bool CHPAAbstrLevel::AddEntranceToClusterAndGraph(CPriorityQueueEdges &entrance, int iClusterId)
{
	ATLASSERT(entrance.GetGridTileInfo().GetGridTypeEnum() == GridTypePassable);

	if (entrance.GetGridTileInfo().GetGridId() == 1988)
		int a = 5;

	auto en = m_Clusters[iClusterId].GetEntrances();
	// if cluster already has entrance added before, do not add it again
	auto it = std::find_if(m_Clusters[iClusterId].GetEntrances().begin(), m_Clusters[iClusterId].GetEntrances().end(), 
		[&entrance](auto entranceId) -> bool { return entranceId == entrance.GetGridTileInfo().GetGridId(); });
	if (it == m_Clusters[iClusterId].GetEntrances().end())
	{
		m_Clusters[iClusterId].GetEntrances().push_back(entrance.GetGridTileInfo().GetGridId());

		CHierarchicalEdge edge(entrance.GetGridTileInfo().GetGridId());
		edge.SetClusterId(iClusterId);
		//CHierarchicalNode node(iClusterId, INTER_EDGE_WEIGHT, entrance.GetGridTileInfo().GetGridId());
		//edge.AddNeighbour(node);
		m_Graph.push_back(edge);
		return true;
	}
	return false;
}

void CHPAAbstrLevel::BuildIntraEdges() //TODO teraz tylko na 1 lvlu jest
{
	CAstar AStar;
	AStar.SetMapManagerPtr(m_GraphLowLevelPtr);
	AStar.PreAllocateResources();

	int iClusterId = 0;
	int lowerLevelfirstNodeId = -1;
	int lowerLevelsecondNodeId = -1;
	int iSearchCount = 0;
	double longestSearch = 0.0;
	for (CCluster &cluster : m_Clusters)
	{
		for (auto &firstNodeId = cluster.GetEntrances().begin(); firstNodeId != cluster.GetEntrances().end(); firstNodeId++)
		{
			for (auto &secondNodeId = firstNodeId + 1; secondNodeId < cluster.GetEntrances().end(); secondNodeId++)
			{
				//AStar.FindShortestPathInCluster(*firstNodeId, *secondNodeId, cluster);
				//if (!AStar.GetPathFound())
				//	continue;
				//if (AStar.GetStatistics().GetStats()["searchTimeClock"] > longestSearch)
				//	longestSearch = AStar.GetStatistics().GetStats()["searchTimeClock"];
				//++iSearchCount;
				//if (AStar.GetPathFound() == false)
				//	int a = 5;


				if (m_iCurrLevel > 1)
				{
					ATLASSERT(false);
				}
				else
				{
					lowerLevelfirstNodeId = *firstNodeId;
					lowerLevelsecondNodeId = *secondNodeId;
				}

				CHierarchicalNode firstNode(iClusterId, AStar.GetShortestPathLength(), lowerLevelsecondNodeId, false, lowerLevelsecondNodeId);
				CHierarchicalNode secondNode(iClusterId, AStar.GetShortestPathLength(), lowerLevelfirstNodeId, false, lowerLevelfirstNodeId);

				auto firstIter = std::find_if(m_Graph.begin(), m_Graph.end(), [lowerLevelfirstNodeId](CHierarchicalEdge &edge) -> bool { return edge.GetId() == lowerLevelfirstNodeId; });
				auto secIter = std::find_if(m_Graph.begin(), m_Graph.end(), [lowerLevelsecondNodeId](CHierarchicalEdge &edge) -> bool { return edge.GetId() == lowerLevelsecondNodeId; });

				m_Graph[std::distance(m_Graph.begin(), firstIter)].AddNeighbour(firstNode);
				m_Graph[std::distance(m_Graph.begin(), secIter)].AddNeighbour(secondNode);
			}
		}
		iClusterId++;
	}
}