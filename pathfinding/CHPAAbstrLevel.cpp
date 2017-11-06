#include "stdafx.h"
#include "CHPAAbstrLevel.h"


void CHPAAbstrLevel::BuildClusters(int iClusterWidth, int iClusterHeight)
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();

	int iCurrClusterHeight, iCurrClusterWidth;
	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iClusterHeight)
	{
		//prevent creating too small clusters, if next clusted would end up being smaller than half of wanted size, glue two clusters together
		iHeight + 3/2 * iClusterHeight >= iMapHeight ? iCurrClusterHeight = iMapHeight - iHeight : iCurrClusterHeight = iHeight;
		for (int iWidth = 0; iWidth < iMapWidth; iWidth += iClusterWidth)
		{
			// again, prevention
			iWidth + 3 / 2 * iClusterWidth >= iMapWidth ? iCurrClusterWidth = iMapWidth - iWidth : iCurrClusterWidth = iWidth;

			CCluster cluster(iCurrClusterWidth, iCurrClusterHeight);
			cluster.SetXPos(iWidth); // most left-up point in cluster
			cluster.SetYpos(iHeight);
			cluster.SetXCenter(iWidth + iCurrClusterHeight / 2); // for A* heuristic
			cluster.SetYCenter(iHeight + iCurrClusterWidth / 2);
			m_Clusters.push_back(cluster);
		}
	}
}

void CHPAAbstrLevel::BuildEntrances(int iClusterWidth, int iClusterHeight, int iMaxEntranceLength)
{
	int iMapHeight = m_GraphLowLevelPtr->GetMapHeight();
	int iMapWidth = m_GraphLowLevelPtr->GetMapWidth();

	int iLeftClusterXPos, iRightClusterXPos;
	// build vertical entrances
	for (int iHeight = 0; iHeight < iMapHeight; iHeight += iClusterHeight)
	{
		for (int iWidth = 0; iWidth < iMapWidth; iWidth += iClusterWidth)
		{
			iRightClusterXPos = iWidth + iClusterWidth;
			iLeftClusterXPos = iRightClusterXPos - 1;
			if (iRightClusterXPos >= iMapHeight) // out of range, no cluster on right side
				continue;

			int iEntranceYpos = iHeight;
			int iEntranceLen = 0;
			while (iEntranceYpos < iHeight + iClusterHeight)
			{
				auto& leftNode = m_GraphLowLevelPtr->GetGraphNodeByPosition(iLeftClusterXPos, iEntranceYpos);
				auto& rightNode = m_GraphLowLevelPtr->GetGraphNodeByPosition(iRightClusterXPos, iEntranceYpos);
				if (leftNode.GetGridTileInfo().GetGridTypeEnum() == GridTypePassable
					&& rightNode.GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
					++iEntranceLen;
				

			}
			
		}
	}
}