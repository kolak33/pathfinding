#pragma once
#include <vector>
class CCluster
{
public:
	CCluster();
	~CCluster();

	CCluster(int iWidth, int iHeight)
		: m_iWidth(iWidth)
		, m_iHeight(iHeight)
	{
	}


	void SetClusterWidth(int iWidth) { m_iWidth = iWidth; }
	void SetClusterHeight(int iHeight) { m_iHeight = iHeight; }
	void SetXPos(int iVal) { m_xPos = iVal; }
	void SetYpos(int iVal) { m_yPos = iVal; }
	void SetXCenter(int iVal) { m_xCenter = iVal; }
	void SetYCenter(int iVal) { m_yCenter = iVal; }

private:
	std::vector<int> m_entrancesIds;
	int m_iWidth;
	int m_iHeight;
	int m_xPos; //most upright-left position in cluster
	int m_yPos;
	

	int m_xCenter; //TODO przemyslec; do heurystyki przy A* na wyzszych poziomach abstrakcji
	int m_yCenter;
};

