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
		, m_iMaxEntranceLength(6)
		, m_iMaxLevel(1)
	{

	}
	~CHPAStar()
	{
	}

	void SetLowLevelGraphPtr(CMapManager *mapManager) //set MapManagerPtr
	{
		m_GraphLowLevelPtr = mapManager;
	}

	void Preprocessing();
	void BuildFirstLevel();

private:
	std::vector<CHPAAbstrLevel> m_AllAbstrLevels; //Higher abstraction levels with clusters
	CMapManager *m_GraphLowLevelPtr;


	int m_iMaxLevel;
	int m_iFirstLvlClusterWidth;
	int m_iFirstLvlClusterHeight;
	int m_iMaxEntranceLength;
};

