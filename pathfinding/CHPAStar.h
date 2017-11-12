#pragma once
#include <vector>
#include "CHPAAbstrLevel.h"
#include "CMapManager.h"

class CHPAStar
{
public:
	CHPAStar() //TODO popatrzec rozne parametry, bedzie trzeba potestowac mocno
		: m_iFirstLvlClusterHeight(10)
		, m_iFirstLvlClusterWidth(10)
		, m_iMaxSingleEntranceLen(6)
		, m_iMaxLevel(3)
		, m_iClustersInGroup(2)
	{

	}
	~CHPAStar()
	{
	}

	void SetLowLevelGraphPtr(CMapManager *mapManager) //set MapManagerPtr
	{
		m_GraphLowLevelPtr = mapManager;
	}

	void DrawMap()
	{
		m_GraphLowLevelPtr->DrawMap();
		//m_AllAbstrLevels[0].DrawEntrances();
		//m_AllAbstrLevels[1].DrawClusterBorders();
		//m_AllAbstrLevels[1].DrawEntrances();
		m_AllAbstrLevels[2].DrawEntrances();
	}

	void Preprocessing();
	void BuildFirstLevel();
	void AddNextLevels();

private:
	std::vector<CHPAAbstrLevel> m_AllAbstrLevels; //Higher abstraction levels with clusters
	CMapManager *m_GraphLowLevelPtr;

	int m_iClustersInGroup; // grouping together nxn adjacent clusters where n = m_iCLustersInGroup
	int m_iMaxLevel;
	int m_iFirstLvlClusterWidth;
	int m_iFirstLvlClusterHeight;
	int m_iMaxSingleEntranceLen;
};

