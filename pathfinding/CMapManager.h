#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
#include "CGridTile.h"
#include "CPriorityQueueEdges.h"
#include "CCluster.h"

class CColor
{
public:
	CColor() : R(0.0), G(0.0), B(0.0) { } // BLACK
	CColor(float r, float g, float b) : R(r), G(g), B(b) { }
	float R;
	float G;
	float B;
};

class CMapManager
{
public:
	CMapManager();


	~CMapManager();


	void InitMapFromFile();
	void InitGraph();
	void CreateTestSuiteForMap(std::string strMapName); //without reference, dont want to rename it inside
	void DrawMap();
	void DrawShortestPath(std::vector<int> &vec);
	void ColorGridTile(int iTileId, CColor &color);
	void DrawClusterBorder(CCluster &cluster, CColor &color);

	int GetMapHeight() { return m_iMapHeight; }
	int GetMapWidth() { return m_iMapWidth; }
	int GetMapNodesCount() { return m_iMapNodesCount; }
	//auto& GetGridMap() { return m_GridMap; }
	auto& GetGraph() { return m_Graph; }
	auto& GetGraphNodeByPosition(int iXPos, int iYPos)
	{
		return m_Graph[m_iMapWidth * iYPos + iXPos];
	}
	int GenerateRandomAccessibleNodeId();

protected:
	std::map<char, std::pair<MapCellType,GridTypeEnum> > m_TerrainMapInfo;
	//std::vector< std::vector<CGridTile> > m_GridMap;

	int m_iMapHeight;
	int m_iMapWidth;

	int m_iMapNodesCount;
	float m_fQuadSize;



	std::vector< CPriorityQueueEdges > m_Graph;

private:
	GridTypeEnum GetGridTypeByChar(char chCell);
};

