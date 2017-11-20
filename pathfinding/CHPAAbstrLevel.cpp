#include "stdafx.h"
#include "CHPAAbstrLevel.h"

void CHPAAbstrLevel::BuildClusters()
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();

	int iCurrClusterHeight = m_iFirstLvlClusterHeight, iCurrClusterWidth = m_iFirstLvlClusterWidth;

	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight, ++m_iClustersColCount)
		//iHeight + 3 * m_iFirstLvlClusterHeight / 2 >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = m_iFirstLvlClusterHeight;
	{
		if (iHeight + m_iFirstLvlClusterHeight >= iMapHeight)
		{
			if (iMapHeight - iHeight > m_iFirstLvlClusterHeight)
				m_bGluedHeightCluster = true;
			iCurrClusterHeight = iMapHeight - iHeight;
		}
		else
			iCurrClusterHeight = m_iFirstLvlClusterHeight;
	}
	//m_iClustersColCount = iMapHeight / m_iFirstLvlClusterHeight + 1;

	for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, ++m_iClustersRowCount)
		//iWidth + 3 * m_iFirstLvlClusterWidth / 2 >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = m_iFirstLvlClusterWidth;
	{
		if (iWidth + m_iFirstLvlClusterWidth >= iMapWidth)
		{
			if (iMapWidth - iWidth > m_iFirstLvlClusterWidth)
				m_bGluedWidthCluster = true;
			iCurrClusterWidth = iMapWidth - iWidth;
		}
		else
			iCurrClusterWidth = m_iFirstLvlClusterWidth;
	}
	//m_iClustersRowCount = iMapWidth / m_iFirstLvlClusterWidth + 1;

	int iLeftClusterId = 0;
	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight/*, ++iLeftClusterId*/)
	{
		//prevent creating too small clusters, if next clusted would end up being smaller than half of wanted size, glue two clusters together
		//iHeight + m_iFirstLvlClusterHeight > iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = m_iFirstLvlClusterHeight;
		iHeight + m_iFirstLvlClusterHeight >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = m_iFirstLvlClusterHeight;
		for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, ++iLeftClusterId)
		{
			// again, prevention
			iWidth + m_iFirstLvlClusterWidth >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = m_iFirstLvlClusterWidth;
			 
	//		if (iWidth + iClusterWidth >= iMapWidth) 
	//			break; // out of range, no cluster on right side
		

			CCluster cluster(iCurrClusterWidth, iCurrClusterHeight);
			cluster.SetXPos(iWidth); // most left-up point in cluster
			cluster.SetYpos(iHeight);
			//cluster.SetXCenter(iWidth + iCurrClusterHeight / 2); // for A* heuristic
			//cluster.SetYCenter(iHeight + iCurrClusterWidth / 2);
			cluster.SetId(iLeftClusterId);
			m_Clusters.push_back(cluster);
		}
	}
}

void CHPAAbstrLevel::ClusterByIdPrepocessing()
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();
	int iCurrClusterHeight = m_iFirstLvlClusterHeight, iCurrClusterWidth = m_iFirstLvlClusterWidth;
	m_ClusterIdByTile.resize(m_GraphLowLevelPtr->GetMapNodesCount());

	int iLeftClusterId = 0;
	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight)
	{
		iHeight + iCurrClusterHeight >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = (std::pow(m_iClustersLen, m_iCurrLevel - 1) * m_iFirstLvlClusterHeight);
		for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, ++iLeftClusterId)
		{
			iWidth + iCurrClusterWidth >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = (std::pow(m_iClustersLen, m_iCurrLevel - 1) * m_iFirstLvlClusterWidth);
			for (int j = iHeight; j < iHeight + iCurrClusterHeight; ++j)
			{
				for (int i = iWidth; i < iWidth + iCurrClusterWidth; ++i)
				{
					m_ClusterIdByTile[m_GraphLowLevelPtr->GetGraphNodeByPosition(i, j).GetGridTileInfo().GetGridId()] = iLeftClusterId;
				}
			}
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

//has to take into account that some clusters might be larger than others
int CHPAAbstrLevel::CalcNewClusterId(CGridTile &tile)
{
	int width = tile.m_iXPos / (std::pow(m_iClustersLen, m_iCurrLevel - 1) * m_iFirstLvlClusterWidth);
	int height = tile.m_iYPos / (std::pow(m_iClustersLen, m_iCurrLevel - 1) * m_iFirstLvlClusterHeight);

	int clId = height * m_iClustersRowCount + width;
	ATLASSERT(clId == m_ClusterIdByTile[tile.GetGridId()]);

	return (height * m_iClustersRowCount + width);
}

void CHPAAbstrLevel::BuildEntrancesNextLevel(CHPAAbstrLevel *lowerAbstrLevelPtr)
{
	auto& lowerClusters = lowerAbstrLevelPtr->GetClusters();
	auto& lowerGraph = lowerAbstrLevelPtr->GetGraph();
	int lowerLvlClustersColCount = lowerAbstrLevelPtr->GetClustersColCount();
	int lowerLvlClustersRowCount = lowerAbstrLevelPtr->GetClustersRowCount();
	int clWidth = m_iFirstLvlClusterWidth * m_iClustersLen;
	int clHeight = m_iFirstLvlClusterHeight * m_iClustersLen;

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
					int iNodeClusterId = CalcNewClusterId(m_GraphLowLevelPtr->GetGraph()[neighbours.GetZeroLevelNodeId()].GetGridTileInfo());
					auto nodeLowerGraphPos = std::find_if(lowerGraph.begin(), lowerGraph.end(), 
						[&neighbours](CHierarchicalEdge &edge) -> bool { return edge.GetId() == neighbours.GetZeroLevelNodeId(); });
					if (nodeLowerGraphPos == lowerGraph.end())
						int a = 5;

					//CHierarchicalNode node(iNodeClusterId, INTER_EDGE_WEIGHT, std::distance(lowerGraph.begin(), nodeLowerGraphPos), true, neighbours.GetZeroLevelNodeId());
					int iEdgeClusterId = CalcNewClusterId(tile);
					
					if (m_iCurrLevel == 4 && tile.m_iXPos >= 160)
						int a = 5;

					if (iNodeClusterId == iEdgeClusterId)
						continue;

					CHierarchicalEdge edge1, edge2;
					//first create edge then corresponding node //TODO usunac to: to bedzie taki X, skrzyzowana zaleznosc
					if (!alreadyCreatedEdgeNode[std::distance(lowerGraph.begin(), graphNode)])
					{
						//CHierarchicalEdge edge(tile.GetGridId(), m_Graph.size());
						edge1.SetId(tile.GetGridId());
						edge1.SetOwnId(m_Graph.size());
						edge1.SetClusterId(iEdgeClusterId);
						//edge.AddNeighbour(node);
						m_Graph.push_back(edge1);
						m_Clusters[iEdgeClusterId].GetEntrances().push_back(tile.GetGridId());
						alreadyCreatedEdgeNode[std::distance(lowerGraph.begin(), graphNode)] = true;
					}
					int edge1Id = tile.GetGridId();
                    auto edge1PosInGraphIter = std::find_if(m_Graph.begin(), m_Graph.end(),
						[edge1Id](CHierarchicalEdge &edge) -> bool { return edge.GetId() == edge1Id; });
					int edge1PosInGraphId = std::distance(m_Graph.begin(), edge1PosInGraphIter);

					bool addNode1 = false, addNode2 = false;
					if (!NeighbourAlreadyAdded(*edge1PosInGraphIter, neighbours.GetZeroLevelNodeId()))
						addNode1 = true;

					//edgeNode in adjacent cluster
					std::swap(iNodeClusterId, iEdgeClusterId);
					CHierarchicalNode node2(iNodeClusterId, INTER_EDGE_WEIGHT, std::distance(lowerGraph.begin(), graphNode), true, tile.GetGridId(), edge1PosInGraphId);
					if (!alreadyCreatedEdgeNode[std::distance(lowerGraph.begin(), nodeLowerGraphPos)])
					{
						//CHierarchicalEdge edge(neighbours.GetZeroLevelNodeId(), m_Graph.size());
						edge2.SetId(neighbours.GetZeroLevelNodeId());
						edge2.SetOwnId(m_Graph.size());
						edge2.SetClusterId(iEdgeClusterId);
						//edge.AddNeighbour(node2);
						m_Graph.push_back(edge2);
						m_Clusters[iEdgeClusterId].GetEntrances().push_back(neighbours.GetZeroLevelNodeId());
						alreadyCreatedEdgeNode[std::distance(lowerGraph.begin(), nodeLowerGraphPos)] = true;
					}
					int edge2Id = neighbours.GetZeroLevelNodeId();
                    auto edge2PosInGraphIter = std::find_if(m_Graph.begin(), m_Graph.end(),
						[edge2Id](CHierarchicalEdge &edge) -> bool { return edge.GetId() == edge2Id; });
					int edge2PosInGraphId = std::distance(m_Graph.begin(), edge2PosInGraphIter);
					std::swap(iNodeClusterId, iEdgeClusterId);
					CHierarchicalNode node1(iNodeClusterId, INTER_EDGE_WEIGHT, std::distance(lowerGraph.begin(), nodeLowerGraphPos), true, neighbours.GetZeroLevelNodeId(), edge2PosInGraphId);

					if (!NeighbourAlreadyAdded(*edge2PosInGraphIter, node2.GetZeroLevelNodeId()))
						addNode2 = true;

					if(addNode1)
						m_Graph[edge1PosInGraphId].AddNeighbour(node1);
					if (addNode2)
						m_Graph[edge2PosInGraphId].AddNeighbour(node2);
				}
			}
		}	
	}
}

bool CHPAAbstrLevel::NeighbourAlreadyAdded(CHierarchicalEdge &edge, int neighbourId)
{
	for (auto& neighbour : edge.GetNeighbours())
	{
		if (neighbour.GetZeroLevelNodeId() == neighbourId)
			return true;
	}
	return false;
}

void CHPAAbstrLevel::BuildHorizontalEntrances(int iMaxSingleEntranceLen)
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();
	int iCurrClusterHeight, iCurrClusterWidth;
	int iLeftClusterXPos, iRightClusterXPos;
	int iLeftClusterId = 0;
	
	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight/*, ++iLeftClusterId*/)
	{
		//prevent creating too small clusters, if next clusted would end up being smaller than half of wanted size, glue two clusters together
		iHeight + m_iFirstLvlClusterHeight >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = m_iFirstLvlClusterHeight;
		for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, ++iLeftClusterId)
		{
			iWidth + m_iFirstLvlClusterWidth >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = m_iFirstLvlClusterWidth;

			iRightClusterXPos = iWidth + m_iFirstLvlClusterWidth;
			iLeftClusterXPos = iRightClusterXPos - 1;
			if (iWidth + iCurrClusterWidth >= iMapWidth) // out of range, no cluster on right side
				continue;

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
				}
				else if(iEntranceLen > 0)// only one entrance in the middle
				{
					auto& leftNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iLeftClusterXPos, iEntranceYposLocal - 1 - iEntranceLen / 2);
					auto& rightNodeFirstEntrance = m_GraphLowLevelPtr->GetGraphNodeByPosition(iRightClusterXPos, iEntranceYposLocal - 1 - iEntranceLen / 2);
					ConnectNodes(leftNodeFirstEntrance, iLeftClusterId, rightNodeFirstEntrance, iLeftClusterId + 1);
				}
			}		
		}
	}
}

void CHPAAbstrLevel::BuildVerticalEntrances(int iMaxSingleEntranceLen)
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();
	int iCurrClusterHeight, iCurrClusterWidth;
	int iUpperClusterYPos, iLowerClusterYPos;
	int iUpperClusterId = 0;
	// build vertical entrances
	for (int iWidth = 0; iWidth < iMapWidth; iWidth += iCurrClusterWidth, iUpperClusterId -= (m_iClustersColCount - 1) * m_iClustersRowCount - 1) //(m_iClustersColCount-1) * m_iClustersRowCount - 1
	{
		//prevent creating too small clusters, if next clusted would end up being smaller than half of wanted size, glue two clusters together
		iWidth + m_iFirstLvlClusterWidth >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = m_iFirstLvlClusterWidth;
		for (int iHeight = 0; iHeight < iMapHeight; iHeight += iCurrClusterHeight, iUpperClusterId += m_iClustersRowCount)
		{		
			iHeight + m_iFirstLvlClusterHeight >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = m_iFirstLvlClusterHeight;
			iLowerClusterYPos = iHeight + m_iFirstLvlClusterHeight;
			iUpperClusterYPos = iLowerClusterYPos - 1;
			if (iHeight + iCurrClusterHeight >= iMapHeight) // out of range, no lower cluster
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

	for (CCluster &cluster : m_Clusters)
		for (int iEntranceId : cluster.GetEntrances())
		{
            ATLASSERT(m_GraphLowLevelPtr->GetGraph()[iEntranceId].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable);
			m_GraphLowLevelPtr->ColorGridTile(iEntranceId, color, true);
		}
}

void CHPAAbstrLevel::DrawStartGoalNodes(int iStart, int iGoal)
{
	CColor color(1.0f, 0.0f, 0.0f); //RED

	auto &startCluster = m_Clusters[CalcClusterId(iStart)];
	auto &goalCluster = m_Clusters[CalcClusterId(iGoal)];

	ATLASSERT(m_GraphLowLevelPtr->GetGraph()[iStart].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable);
	ATLASSERT(m_GraphLowLevelPtr->GetGraph()[iGoal].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable);
	m_GraphLowLevelPtr->ColorGridTile(iStart, color);
	m_GraphLowLevelPtr->ColorGridTile(iGoal, color);

	for (auto neighId : m_Graph.back().GetNeighbours())
		m_GraphLowLevelPtr->DrawLine(iGoal, neighId.GetZeroLevelNodeId(), color);

	for (auto neighId : m_Graph[m_Graph.size()-2].GetNeighbours())
		m_GraphLowLevelPtr->DrawLine(iStart, neighId.GetZeroLevelNodeId(), color);
}

void CHPAAbstrLevel::DrawGraphConnections()
{
	CColor color(0.2f, 0.2f, 0.2f);
	for (auto &edge : m_Graph)
	{
		for (auto &node : edge.GetNeighbours())
		{
			m_GraphLowLevelPtr->DrawLine(edge.GetId(), node.GetZeroLevelNodeId(), color);
		}
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

	CHierarchicalNode firstNodeNeighbour(iSecondClId, INTER_EDGE_WEIGHT, secondE.GetGridTileInfo().GetGridId(), true, secondE.GetGridTileInfo().GetGridId(), secondEPosInGraph);
	CHierarchicalNode secondNodeNeighbour(iFirstClId, INTER_EDGE_WEIGHT, firstE.GetGridTileInfo().GetGridId(), true, firstE.GetGridTileInfo().GetGridId(), firstEPosInGraph);

	m_Graph[firstEPosInGraph].AddNeighbour(firstNodeNeighbour);
	m_Graph[secondEPosInGraph].AddNeighbour(secondNodeNeighbour);
}

// return true when added at the end of m_Graph
bool CHPAAbstrLevel::AddEntranceToClusterAndGraph(CPriorityQueueEdges &entrance, int iClusterId)
{
	ATLASSERT(entrance.GetGridTileInfo().GetGridTypeEnum() == GridTypePassable);

	auto en = m_Clusters[iClusterId].GetEntrances();
	// if cluster already has entrance added before, do not add it again
	auto it = std::find_if(m_Clusters[iClusterId].GetEntrances().begin(), m_Clusters[iClusterId].GetEntrances().end(), 
		[&entrance](auto entranceId) -> bool { return entranceId == entrance.GetGridTileInfo().GetGridId(); });
	if (it == m_Clusters[iClusterId].GetEntrances().end())
	{
		m_Clusters[iClusterId].GetEntrances().push_back(entrance.GetGridTileInfo().GetGridId());

		CHierarchicalEdge edge(entrance.GetGridTileInfo().GetGridId(), m_Graph.size());
		edge.SetClusterId(iClusterId);
		//CHierarchicalNode node(iClusterId, INTER_EDGE_WEIGHT, entrance.GetGridTileInfo().GetGridId());
		//edge.AddNeighbour(node);
		m_Graph.push_back(edge);
		return true;
	}
	return false;
}

void CHPAAbstrLevel::BuildIntraEdges(std::vector<CHierarchicalEdge> &graphLowerLevel)
{
	CAstar AStar;
	AStar.SetMapManagerPtr(m_GraphLowLevelPtr);

	int lowerLevelfirstNodeId = -1;
	int lowerLevelsecondNodeId = -1;
	int iSearchCount = 0;
	double longestSearch = 0.0;
	std::cout << "BUILD INTA EDGES, lvl: " << m_iCurrLevel << std::endl;
	int iProgress = 0;
	int iPercent = 0;
	for (CCluster &cluster : m_Clusters)
	{
        for (auto firstNodeId = cluster.GetEntrances().begin(); firstNodeId != cluster.GetEntrances().end(); firstNodeId++)
		{
            for (auto secondNodeId = firstNodeId + 1; secondNodeId < cluster.GetEntrances().end(); secondNodeId++)
			{
				CreateIntraEdges(AStar, cluster, graphLowerLevel, *firstNodeId, *secondNodeId, false);
			}
		}
		++iProgress;
		if (iProgress % (m_Clusters.size() / 10 + 1) == 0)
		{
			iPercent += 10;
			std::cout << "lvl: " << m_iCurrLevel << ", completed: " << iPercent << "%" << std::endl;
		}
	}
	std::cout << "BUILD INTRA EDGES COMPLETED FOR LVL: " << m_iCurrLevel << std::endl;

	m_EdgePositions.resize(m_GraphLowLevelPtr->GetGraph().size());
	for (auto &edge : m_Graph)
		m_EdgePositions[edge.GetId()] = edge.GetOwnId();
}

bool CHPAAbstrLevel::AddStartAndGoalNodesToGraph(CAstar &AStar, int iStartId, int iGoalId, std::vector<CHierarchicalEdge> &graphLowerLevel)
{
	//check if same cluster, if so, return(there is no need to add them here)

	if (CalcClusterId(iStartId) == CalcClusterId(iGoalId))
		return false;

	AddNodeToGraph(AStar, iStartId, graphLowerLevel);
	AddNodeToGraph(AStar, iGoalId, graphLowerLevel);

	return true;
}

void CHPAAbstrLevel::AddNodeToGraph(CAstar &AStar, int iNodeId, std::vector<CHierarchicalEdge> &graphLowerLevel)
{
	if (IsEntrance(iNodeId))
		return;
		
	CHierarchicalEdge edge;
	int iClusterId = CalcClusterId(iNodeId);

	edge.SetId(iNodeId);
	edge.SetOwnId(m_Graph.size());
	edge.SetClusterId(iClusterId);
	m_Graph.push_back(edge);
	m_EdgePositions[iNodeId] = m_Graph.size() - 1;
	m_AddedEdges++;

	auto &cluster = m_Clusters[iClusterId];
	for (auto &entranceId : cluster.GetEntrances())
	{
		CreateIntraEdges(AStar, cluster, graphLowerLevel, iNodeId, entranceId, true);
	}
}

void CHPAAbstrLevel::DeleteStartAndGoalNodesFromGraph(int iStartId, int iGoalId)
{
	if(!IsEntrance(iStartId))
		DeleteStartOrGoal(iStartId);
	if(!IsEntrance(iGoalId))
		DeleteStartOrGoal(iGoalId);
	ATLASSERT(m_AddedEdges == 0);
}

void CHPAAbstrLevel::DeleteStartOrGoal(int iNodeId)
{
	//if start/goal node was added must be either at the end of m_Graph or one before the end

	if (m_Graph.back().GetId() == iNodeId)
	{
		for (auto &startGoalNeigh : m_Graph.back().GetNeighbours())
		{
			auto &edgeNeigh = m_Graph[startGoalNeigh.GetOwnId()].GetNeighbours();
			auto iter = std::find_if(edgeNeigh.begin(), edgeNeigh.end(),
				[iNodeId](CHierarchicalNode &node) -> bool {return node.GetZeroLevelNodeId() == iNodeId; });
			edgeNeigh.erase(iter);
		}
		m_Graph.pop_back();
		m_AddedEdges--;
	}
	else if (m_Graph.size() >= 2 && m_Graph[m_Graph.size() - 2].GetId() == iNodeId)
	{
		for (auto &startGoalNeigh : m_Graph[m_Graph.size() - 2].GetNeighbours())
		{
			auto &edgeNeigh = m_Graph[startGoalNeigh.GetOwnId()].GetNeighbours();
			auto iter = std::find_if(edgeNeigh.begin(), edgeNeigh.end(),
				[iNodeId](CHierarchicalNode &node) -> bool {return node.GetZeroLevelNodeId() == iNodeId; });
			edgeNeigh.erase(iter);
		}
		m_Graph.erase(m_Graph.begin() + m_Graph.size() - 2);
		m_AddedEdges--;
	}
}

bool CHPAAbstrLevel::IsEntrance(int iNodeId)
{
	auto clusterId = CalcClusterId(iNodeId);
	auto &entrances = m_Clusters[clusterId].GetEntrances();
	auto iter = std::find_if(entrances.begin(), entrances.end(),
		[iNodeId](int entranceId) -> bool {return entranceId == iNodeId; });
	return (iter != entrances.end());
}

int CHPAAbstrLevel::CalcClusterId(int iNodeId)
{
	auto &tile = m_GraphLowLevelPtr->GetGraph()[iNodeId].GetGridTileInfo();
	return CalcNewClusterId(tile);
}

int CHPAAbstrLevel::FindEdgePositionInGraph(std::vector<CHierarchicalEdge> &graph, int iTileId, bool bAddStart, bool bFindInCurrentGraph)
{
	if (!bAddStart)
	{
		auto lowerLvlFst = std::find_if(graph.begin(), graph.end(), [iTileId](CHierarchicalEdge &edge) -> bool { return edge.GetId() == iTileId; });
		return std::distance(graph.begin(), lowerLvlFst);
	}
	else if (bFindInCurrentGraph)
	{
		return m_EdgePositions[iTileId];
	}
	else
		return (*m_EdgePosLowerLvlPtr)[iTileId];
}

void CHPAAbstrLevel::AddIntraEdgesToGraph(double dShortestPathLength, int iClusterId, int iFirstNodeId, int iSecondNodeId,
					int iLowerLvlFirstNodeId, int iLowerLvlSecondNodeId, bool bAddStart)
{
	int firstOwnId = FindEdgePositionInGraph(m_Graph, iFirstNodeId, bAddStart, true);//std::distance(m_Graph.begin(), firstIter);
	int secondOwnId = FindEdgePositionInGraph(m_Graph, iSecondNodeId, bAddStart, true);//std::distance(m_Graph.begin(), secIter);

	CHierarchicalNode firstNode(iClusterId, dShortestPathLength, iLowerLvlSecondNodeId, false, iSecondNodeId, secondOwnId); //lowerLevelsecondNodeId
	CHierarchicalNode secondNode(iClusterId, dShortestPathLength, iLowerLvlFirstNodeId, false, iFirstNodeId, firstOwnId); //lowerLevelfirstNodeId

	if (!NeighbourAlreadyAdded(m_Graph[firstOwnId], iSecondNodeId))
		m_Graph[firstOwnId].AddNeighbour(firstNode);//add goal neigh to start
	if (!NeighbourAlreadyAdded(m_Graph[secondOwnId], iFirstNodeId))
		m_Graph[secondOwnId].AddNeighbour(secondNode);//add start neigh to goal
}

void CHPAAbstrLevel::CreateIntraEdges(CAstar &AStar, CCluster &cluster, std::vector<CHierarchicalEdge> &graphLowerLevel,
	 int iFirstNodeId, int iSecondNodeId, bool bAddStart)
{
	int lowerLevelFirstNodeId, lowerLevelSecondNodeId;
	if (m_iCurrLevel == 1)
	{
		AStar.FindShortestPathInCluster(iFirstNodeId, iSecondNodeId, cluster);
	}
	else
	{
		lowerLevelFirstNodeId = FindEdgePositionInGraph(graphLowerLevel, iFirstNodeId, bAddStart, false);
		lowerLevelSecondNodeId = FindEdgePositionInGraph(graphLowerLevel, iSecondNodeId, bAddStart, false);
		AStar.FindShortestPathInClusterAbstract(iFirstNodeId, iSecondNodeId, cluster, graphLowerLevel, lowerLevelFirstNodeId, lowerLevelSecondNodeId);
	}

	if (!AStar.GetPathFound())
		return;

	if(m_iCurrLevel == 1)
	{
		lowerLevelFirstNodeId = iFirstNodeId;
		lowerLevelSecondNodeId = iSecondNodeId;
	}

	AddIntraEdgesToGraph(AStar.GetShortestPathLength(), cluster.GetId(), iFirstNodeId, iSecondNodeId, lowerLevelFirstNodeId, lowerLevelSecondNodeId, bAddStart); //iClusterId bylo
}
