#include "stdafx.h"
#include "CHPAStar.h"

void CHPAStar::Preprocessing()
{
	BuildFirstLevel();
}

void CHPAStar::BuildFirstLevel()
{
	int iFirstLevel = 1;
	CHPAAbstrLevel firstAbstrLevel(iFirstLevel, m_GraphLowLevelPtr);
	firstAbstrLevel.BuildClusters(m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight);
	firstAbstrLevel.BuildEntrances(m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight, m_iMaxEntranceLength);



	m_AllAbstrLevels.push_back(firstAbstrLevel);
}