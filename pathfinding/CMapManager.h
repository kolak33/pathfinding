#pragma once
#include <memory>
#include <vector>
#include <map>
#include "CGridTile.h"

class CMapManager
{
public:
	CMapManager();


	~CMapManager();


	void InitMapFromFile();
	void DrawMap();
	void DrawShortestPath(std::vector<int> &vec);

	int GetMapHeight() { return m_iMapHeight; }
	int GetMapWidth() { return m_iMapWidth; }
	int GetMapNodesCount() { return m_iMapNodesCount; }
	auto& GetGridMap() { return m_GridMap; }

protected:
	std::map<char, std::pair<MapCellType,GridTypeEnum> > m_TerrainMapInfo;
	std::vector< std::vector<CGridTile> > m_GridMap;

	int m_iMapHeight;
	int m_iMapWidth;

	int m_iMapNodesCount;

private:
	GridTypeEnum GetGridTypeByChar(char chCell);
};

