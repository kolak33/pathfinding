#include "stdafx.h"
#include "CHPAStar.h"

void CHPAStar::Preprocessing()
{
	BuildFirstLevel();
	AddNextLevels();
}

void CHPAStar::BuildFirstLevel()
{
	int iFirstLevel = 1;
	CHPAAbstrLevel firstAbstrLevel(iFirstLevel, m_iClustersInGroup, m_GraphLowLevelPtr);
	firstAbstrLevel.BuildClusters(m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight);
	firstAbstrLevel.BuildVerticalEntrances(m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight, m_iMaxSingleEntranceLen);
	firstAbstrLevel.BuildHorizontalEntrances(m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight, m_iMaxSingleEntranceLen);
	firstAbstrLevel.BuildIntraEdges();

	m_AllAbstrLevels.push_back(firstAbstrLevel);
}

void CHPAStar::AddNextLevels()
{
	for (int iLvl = 2; iLvl <= m_iMaxLevel; ++iLvl)
	{
		CHPAAbstrLevel nextLevel(iLvl, m_iClustersInGroup, m_GraphLowLevelPtr);
		nextLevel.BuildClustersNextLevel(&m_AllAbstrLevels[m_AllAbstrLevels.size() - 1]);
		nextLevel.BuildHorizontalEntrancesNextLevel(&m_AllAbstrLevels[m_AllAbstrLevels.size() - 1], m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight);
		//
		//


		m_AllAbstrLevels.push_back(nextLevel);
	}

}