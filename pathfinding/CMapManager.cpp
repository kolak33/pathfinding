#include "stdafx.h"
#include "CMapManager.h"
#include "Config.h"
#include <fstream>
#include <string>
#include <gl\freeglut.h>
#include "CPriorityQueueNode.h"
#include <regex>


CMapManager::CMapManager(): m_iMapHeight(-1), m_iMapWidth(-1)
{
	m_TerrainMapInfo['.'] = std::make_pair(CellTypePassableTerrain, GridTypePassable);
	m_TerrainMapInfo['G'] = std::make_pair(CellTypePassableTerrain, GridTypePassable);
	m_TerrainMapInfo['S'] = std::make_pair(CellTypeShallowWater, GridTypePassable);

	m_TerrainMapInfo['@'] = std::make_pair(CellTypeOutOfBounds, GridTypeImpassable);
	m_TerrainMapInfo['O'] = std::make_pair(CellTypeOutOfBounds, GridTypeImpassable);
	m_TerrainMapInfo['T'] = std::make_pair(CellTypeTrees, GridTypeImpassable);
	m_TerrainMapInfo['W'] = std::make_pair(CellTypeWater, GridTypeImpassable);

	InitMapFromFile();
	InitGraph();
}

CMapManager::~CMapManager()
{
}

GridTypeEnum CMapManager::GetGridTypeByChar(char chCell)
{
	auto it = m_TerrainMapInfo.find(chCell);
	return (it == m_TerrainMapInfo.end() ? GridTypeNone : (it->second.second));
}

void CMapManager::ColorGridTile(int iTileId, CColor &color, bool bMarkEntrance)
{
	glBegin(GL_QUADS);
	glColor3f(color.R, color.G, color.B);

	if (bMarkEntrance && m_Graph[iTileId].GetUsed() == true) 
		glColor3f(0.5f, 0.5f, 0.5f);
	if(bMarkEntrance)
		m_Graph[iTileId].SetUsed(true);

	CCoords &coords = m_Graph[iTileId].GetGridTileInfo().GetCoords();
	/*glVertex2f(coords.x, coords.y);
	glVertex2f(coords.x + m_fQuadSize, coords.y);
	glVertex2f(coords.x + m_fQuadSize, coords.y + m_fQuadSize);
	glVertex2f(coords.x, coords.y + m_fQuadSize);*/
	glVertex2f(coords.x, coords.y);
	glVertex2f(coords.x, coords.y - m_fQuadSize);
	glVertex2f(coords.x + m_fQuadSize, coords.y - m_fQuadSize);
	glVertex2f(coords.x + m_fQuadSize, coords.y);

	glEnd();
	glFlush();
}

void CMapManager::DrawClusterBorder(CCluster &cluster, CColor &color)
{
	//glutShowOverlay();

	auto& leftUp    = GetGraphNodeByPosition(cluster.GetXPos(), cluster.GetYPos()).GetGridTileInfo();
	auto& leftDown  = GetGraphNodeByPosition(cluster.GetXPos(), cluster.GetYPos() + cluster.GetClusterHeight() - 1).GetGridTileInfo();
	auto& rightUp   = GetGraphNodeByPosition(cluster.GetXPos() + cluster.GetClusterWidth() - 1, cluster.GetYPos()).GetGridTileInfo();
	auto& rightDown = GetGraphNodeByPosition(cluster.GetXPos() + cluster.GetClusterWidth() - 1, cluster.GetYPos() + cluster.GetClusterHeight() - 1).GetGridTileInfo();
	
	CColor colPassable(0.04f, 0.60f, 0.05f), colImPassable(0.04f, 0.20f, 0.05f);
	//draw upper & lower border
	for (int i = 0; i < cluster.GetClusterWidth(); ++i)
	{
		auto &leftUpTile = GetGraphNodeByPosition(cluster.GetXPos() + i, cluster.GetYPos()).GetGridTileInfo();
		auto &leftDownTile = GetGraphNodeByPosition(cluster.GetXPos() + i, cluster.GetYPos() + cluster.GetClusterHeight() - 1).GetGridTileInfo();

		leftUpTile.GetGridTypeEnum() == GridTypePassable ? color = colPassable : color = colImPassable;
		ColorGridTile(leftUpTile.GetGridId(), color);
		leftDownTile.GetGridTypeEnum() == GridTypePassable ? color = colPassable : color = colImPassable;
		ColorGridTile(leftDownTile.GetGridId(), color);
	}
	//draw left & right border
	for (int i = 0; i < cluster.GetClusterHeight(); ++i)
	{
		auto &leftTile = GetGraphNodeByPosition(cluster.GetXPos(), cluster.GetYPos() + i).GetGridTileInfo();
		auto &rightTile = GetGraphNodeByPosition(cluster.GetXPos() + cluster.GetClusterWidth() - 1, cluster.GetYPos() + i).GetGridTileInfo();

		leftTile.GetGridTypeEnum() == GridTypePassable ? color = colPassable : color = colImPassable;
		ColorGridTile(leftTile.GetGridId(), color);
		rightTile.GetGridTypeEnum() == GridTypePassable ? color = colPassable : color = colImPassable;
		ColorGridTile(rightTile.GetGridId(), color);
	}

	//glEnd();
	//glFlush();
}

void CMapManager::DrawLine(int iFirstNode, int iSecNode, CColor &color)
{
	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(color.R, color.G, color.B);

	auto &firstNode = m_Graph[iFirstNode].GetGridTileInfo().GetCoords();
	auto &secNode = m_Graph[iSecNode].GetGridTileInfo().GetCoords();

	glVertex2f(firstNode.x, firstNode.y);
	glVertex2f(secNode.x, secNode.y);

	glEnd();
	glFlush();
}

void CMapManager::DrawMap()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);
	for(int i=0; i<m_Graph.size(); ++i)
	{
		if(m_Graph[i].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			glColor3f(0.94f, 0.94f, 0.94f); //WHITE-GREY
			//glColor3f(0.5f, 0.2f, 0.8f); //PURPLE
		else
			glColor3f(0.07f, 0.45f, 0.05f); //GREENISH
			//glColor3f(1.0f, 0.0f, 0.0f); //RED

		CCoords &coords = m_Graph[i].GetGridTileInfo().GetCoords();

		/*glVertex2f(coords.x, coords.y);
		glVertex2f(coords.x + m_fQuadSize, coords.y);
		glVertex2f(coords.x + m_fQuadSize, coords.y + m_fQuadSize);
		glVertex2f(coords.x, coords.y + m_fQuadSize);*/
		glVertex2f(coords.x, coords.y);
		glVertex2f(coords.x, coords.y - m_fQuadSize);
		glVertex2f(coords.x + m_fQuadSize, coords.y - m_fQuadSize);
		glVertex2f(coords.x + m_fQuadSize, coords.y);	
	}

	glEnd();
	glFlush();//
}

void CMapManager::DrawShortestPath(std::vector<int> &vec)
{
	glBegin(GL_QUADS);
	
	for (int i = 0; i < vec.size(); ++i)
	{
		if(i==0 || i == vec.size() - 1)
			glColor3f(0.0f, 0.0f, 1.0f); //BLUE
		else
			glColor3f(0.0f, 0.0f, 0.0f); //BLACK

		int iRow, iCol;
		iRow = floor(vec[i] / m_iMapWidth);
		iCol = vec[i] - iRow * m_iMapWidth;

		CCoords &coords = m_Graph[vec[i]].GetGridTileInfo().GetCoords();

		/*glVertex2f(coords.x, coords.y);
		glVertex2f(coords.x + m_fQuadSize, coords.y);
		glVertex2f(coords.x + m_fQuadSize, coords.y + m_fQuadSize);
		glVertex2f(coords.x, coords.y + m_fQuadSize);*/
		glVertex2f(coords.x, coords.y);
		glVertex2f(coords.x, coords.y - m_fQuadSize);
		glVertex2f(coords.x + m_fQuadSize, coords.y - m_fQuadSize);
		glVertex2f(coords.x + m_fQuadSize, coords.y);
	}

	glEnd();
	glFlush();
}

void CMapManager::InitMapFromFile()
{
	std::string strMapLocationPath = strMapLocationPrefix + strMapName;
	std::ifstream mapFile;
	mapFile.open(strMapLocationPath.c_str());

	ATLASSERT(mapFile.is_open() == true);
	if (mapFile.is_open())
	{
		std::string strMapType, strTemp;

		mapFile >> strTemp >> strMapType; // type octile
		mapFile >> strTemp >> m_iMapHeight;
		mapFile >> strTemp >> m_iMapWidth;
		mapFile >> strTemp;
		ATLASSERT(strMapType == "octile");
		
		m_iMapNodesCount = m_iMapHeight * m_iMapWidth;
		m_Graph.resize(m_iMapNodesCount);

		//TODO w jednym miejscu quadSize obliczac, moze zapisac jako zmienna statyczna
        int quadCount = max(m_iMapHeight, m_iMapWidth);
		float quadSize = 2.0f / static_cast<float>(1.1f * quadCount);
		m_fQuadSize = quadSize;

		for (int i = 0, id = 0; i < m_iMapHeight; ++i)
		{
			float yPos = 0.9f - i * quadSize;
			for (int j = 0; j < m_iMapWidth; ++j, ++id)
			{
				float xPos = -0.9f + j * quadSize;
				char chCell;
				CGridTile gridTile;
				gridTile.SetGridId(id);
				CCoords coords(xPos, yPos);
				gridTile.SetCoords(coords);
				gridTile.m_iXPos = j;//ze wzgl na troche inne wspolrzedne mapy do zwyklej osi
				gridTile.m_iYPos = i;
	
				mapFile >> chCell;
				GridTypeEnum gridType = GetGridTypeByChar(chCell);
				ATLASSERT(gridType != GridTypeNone);
				gridTile.SetGridTypeEnum(gridType);

				m_Graph[id].SetGridTileInfo(gridTile);
			}
		}
		mapFile.close();

		CreateTestSuiteForMap(strMapName);
	}
}

void CMapManager::InitGraph()
{
	//TODO moze zrobic zliczanie i rezerwowanie potrzebnej ilosci miejsca wczesniej, aby nie bylo zadnych realokacji
	for (int iHeight = 0, iNodeId = 0; iHeight < m_iMapHeight; ++iHeight)
	{
		for (int iWidth = 0; iWidth < m_iMapWidth; ++iWidth, ++iNodeId)
		{
			// jezeli pole jest nieosiagalne to nie tworzymy dla niego listy sasiadow
			if (m_Graph[iNodeId].GetGridTileInfo().GetGridTypeEnum() == GridTypeImpassable)
				continue;
			//1. ADD UPPER NEIGHBOUR
			int iNeighbourId = iNodeId - m_iMapWidth;
			if (iHeight > 0 && m_Graph[iNeighbourId].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(iNeighbourId, STRAIGHT_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//2. ADD BOTTOM NEIGHBOUR
			iNeighbourId = iNodeId + m_iMapWidth;
			if (iHeight + 1 <  m_iMapHeight && m_Graph[iNeighbourId].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(iNeighbourId, STRAIGHT_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}
			//3. ADD LEFT NEIGHBOUR
			iNeighbourId = iNodeId - 1;
			if (iWidth > 0 && m_Graph[iNeighbourId].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(iNeighbourId, STRAIGHT_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//4. ADD RIGHT NEIGHBOUR
			iNeighbourId = iNodeId + 1;
			if (iWidth + 1 < m_iMapWidth && m_Graph[iNeighbourId].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(iNeighbourId, STRAIGHT_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//5. ADD UP-LEFT DIAGONAL
			iNeighbourId = iNodeId - m_iMapWidth - 1;
			if (iHeight > 0 && iWidth > 0
				&& m_Graph[iNodeId - 1].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable
				&& m_Graph[iNodeId - m_iMapWidth].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(iNeighbourId, DIAGONAL_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//6. ADD UP-RIGHT DIAGONAL
			iNeighbourId = iNodeId - m_iMapWidth + 1;
			if (iHeight > 0
				&& iWidth + 1 <  m_iMapWidth
				&& m_Graph[iNodeId - m_iMapWidth].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable
				&& m_Graph[iNodeId + 1].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(iNeighbourId, DIAGONAL_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//7. ADD DOWN-LEFT DIAGONAL
			iNeighbourId = iNodeId + m_iMapWidth - 1;
			if (iHeight + 1 <  m_iMapHeight
				&& iWidth > 0
				&& m_Graph[iNodeId - 1].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable
				&& m_Graph[iNodeId + m_iMapWidth].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(iNeighbourId, DIAGONAL_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}

			//8. ADD DOWN-RIGHT DIAGONAL
			iNeighbourId = iNodeId + m_iMapWidth + 1;
			if (iHeight + 1 <  m_iMapHeight
				&& iWidth + 1 <  m_iMapWidth
				&& m_Graph[iNodeId + 1].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable
				&& m_Graph[iNodeId + m_iMapWidth].GetGridTileInfo().GetGridTypeEnum() == GridTypePassable)
			{
				CPriorityQueueNode node(iNeighbourId, DIAGONAL_WEIGHT);
				m_Graph[iNodeId].AddNeighbour(node);
			}
		}
	}
}

void CMapManager::CreateTestSuiteForMap(std::string strMapName)
{
	//test if file already exists
	strMapName = std::regex_replace(strMapName, std::regex(".map"), ".txt");
	std::string strTestSuiteLocationPath = strTestsLocationPrefix + strMapName;
	std::ifstream testFile(strTestSuiteLocationPath.c_str());
	if (!testFile.good())
	{
		std::cout << "CREATING TEST SUITE FOR MAP: " << strMapName << std::endl;
		std::ofstream testFileSuite(strTestSuiteLocationPath.c_str());
		ATLASSERT(testFileSuite.is_open());
		if (testFileSuite.is_open())
		{
			srand(time(NULL));
			testFileSuite << "testsCount: " << iGenerateTestCount << "\n";
			for (int i = 0; i < iGenerateTestCount; ++i)
			{
				testFileSuite << GenerateRandomAccessibleNodeId() << " " << GenerateRandomAccessibleNodeId() << "\n";
			}
			testFileSuite << fflush;

			testFileSuite.close();
		}
	}
}

int CMapManager::GenerateRandomAccessibleNodeId()
{
	int iRand = rand() % m_iMapNodesCount;
	while (m_Graph[iRand].GetGridTileInfo().GetGridTypeEnum() != GridTypePassable)
		iRand = rand() % m_iMapNodesCount;

	return iRand;
}
