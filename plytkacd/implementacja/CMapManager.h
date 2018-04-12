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
	CMapManager()
	{
	}
	CMapManager(std::string &strMapLocationPath, std::string strTestSuiteLocationPath);


	~CMapManager();


	void InitMapFromFile(std::string &strMapLocationPath, std::string strTestSuiteLocationPath);
	void InitGraph();
	void CreateTestSuiteForMap(std::string strTestSuiteLocationPath); //without reference, dont want to rename it inside
	void DrawMap();
	void ColorSelectedTiles(std::vector<int> &vec, CColor *color);
	void DrawShortestPath(std::vector<int> &vec, CColor *color = nullptr);
	void ColorGridTile(int iTileId, CColor &color, bool bMarkEntrance = false);
	void DrawClusterBorder(CCluster &cluster, CColor &color);
	void DrawLine(int iFirstNode, int iSecNode, CColor &color);

	int GetMapHeight() { return m_iMapHeight; }
	int GetMapWidth() { return m_iMapWidth; }
	int GetMapNodesCount() { return m_iMapNodesCount; }
	//auto& GetGridMap() { return m_GridMap; }
	auto& GetGraph() { return m_Graph; }
	auto& GetGraphNodeByPosition(int iXPos, int iYPos)
	{
		ATLASSERT(iXPos >= 0 && iXPos < m_iMapWidth);
		ATLASSERT(iYPos >= 0 && iYPos < m_iMapHeight);
		return m_Graph[m_iMapWidth * iYPos + iXPos];
	}
	CGridTile* GetNodeIfExists(int iXPos, int iYPos)
	{
		if (iXPos < 0 || iXPos >= m_iMapWidth
			|| iYPos < 0 || iYPos >= m_iMapHeight)
		{
			//std::cout << "\nNODE DOES NOT EXIST\n" << std::endl; //TODO usunac to
			return nullptr;
		}

		return &(m_Graph[m_iMapWidth * iYPos + iXPos].GetGridTileInfo());
	}

	bool IsPassable(int iXPos, int iYPos)
	{
		auto *tile = GetNodeIfExists(iXPos, iYPos);
		return ( tile == nullptr ? false : (tile->GetGridTypeEnum() == GridTypePassable) );
	}

	auto& GetTileById(int iTileId)
	{
		return m_Graph[iTileId].GetGridTileInfo();
	}

	int GenerateRandomAccessibleNodeId();

protected:
	std::map<char, std::pair<MapCellType,GridTypeEnum> > m_TerrainMapInfo;
	//std::vector< std::vector<CGridTile> > m_GridMap;

	int m_iMapHeight;
	int m_iMapWidth;

	int m_iMapNodesCount;
	float m_fQuadSize;

	std::vector<int> m_PassableNodeIds;

	std::vector< CPriorityQueueEdges > m_Graph;

private:
	GridTypeEnum GetGridTypeByChar(char chCell);
};

